#include <dlib/string.h>
#include <dlib/threads.h>
#include <dlib/optimization.h>
#include "model.h"

/********** utilities **********/
// calculate binomial coefficient
static double binomial (const unsigned n, const unsigned k)
{
	double result = 1;
	unsigned int tmp = k;

	if ( tmp > n - tmp )
		tmp = n - tmp;

	for (unsigned i = 0; i < tmp; ++i)
	{
		result *= (n - i);
		result /= (i + 1);
	}

	return result;
}

// calculate factorial recursively
static double factorial (unsigned n)
{
	return (n == 1 || n == 0) ? 1 : factorial(n - 1) * n;
}

/********** class CdlpFilter **********/

CdlpFilter::CdlpFilter (const state_v& initState, const time_v& intervalBounds)
	: m_filterOrder(initState.size()), m_initState(initState), m_intervalBounds(intervalBounds)
{
	if (initState.size() == 0)
	{
		throw dlib::error("[CdlpFilter] Filter can't be initialized with an empty state!");
	}
}

void CdlpFilter::initialize (const state_v& initState, const time_v& intervalBounds)
{
	if (initState.size() == 0)
	{
		throw dlib::error("[initialize] Filter can't be initialized with an empty state!");
	}

	m_filterOrder = initState.size();
	m_initState = initState;
	m_intervalBounds = intervalBounds;
}

void CdlpFilter::calc_filter_coeffs (coeff_v& filterCoeffs, const pitch_target_s& qtaParams, const state_v& startState) const
{
	if (filterCoeffs.size() != m_filterOrder)
	{
		throw dlib::error("[calc_filter_coeffs] Wrong size of coefficient vector! " + std::to_string(filterCoeffs.size()) + " != " + std::to_string(m_filterOrder));
	}

	filterCoeffs[0] = startState[0] - qtaParams.b;	// 0th coefficient
	for (unsigned n=1; n<m_filterOrder; ++n)	// all other coefficients
	{
		double acc (0.0);
		for (unsigned i=0; i<n; ++i)
		{
			acc += ( filterCoeffs[i]*std::pow(-qtaParams.l,n-i)*binomial(n,i)*factorial(i) );
		}

		if (n==1)
		{
			acc += qtaParams.m; // adaption for linear targets; minus changes in following term!
		}

		filterCoeffs[n] = (startState[n] - acc)/factorial(n);
	}
}

void CdlpFilter::calc_f0 (signal_s& freqResp, const target_v& qtaVector) const
{
	if (qtaVector.size() != m_intervalBounds.size()-1)
	{
		throw dlib::error("[calc_f0] Wrong size of qta parameter vector! " + std::to_string(qtaVector.size()) + " != " + std::to_string(m_intervalBounds.size()-1));
	}

	freqResp.sampleValues.set_size(freqResp.sampleTimes.size());

	// keep state at syllable bound
	state_v currentState (m_initState);

	// keep index of current sample
	unsigned sampleIndex (0);

	for (unsigned i=0; i<qtaVector.size(); ++i)
	{

		// filter coefficients
		coeff_v c (m_filterOrder, 0.0);
		calc_filter_coeffs(c, qtaVector[i], currentState);

		while (freqResp.sampleTimes[sampleIndex] <= m_intervalBounds[i+1])
		{
			double acc (0.0);
			double t (freqResp.sampleTimes[sampleIndex] - m_intervalBounds[i]);	// current samplePoint, time shift
			for (unsigned n=0; n<m_filterOrder; ++n)
			{
				acc += (c[n] * std::pow(t,n));
			}

			freqResp.sampleValues(sampleIndex) = acc * std::exp(-qtaVector[i].l*t) + qtaVector[i].m*t + qtaVector[i].b;

			sampleIndex++;
			if (sampleIndex >= freqResp.sampleTimes.size())
			{
				break;
				throw dlib::error("[calc_f0] Sample index exceeds sample time vector! " + std::to_string(freqResp.sampleTimes.size()));
			}
		}

		// update filter state
		calc_state(currentState, m_intervalBounds[i+1], m_intervalBounds[i], qtaVector[i]);
	}
}

void CdlpFilter::calc_state (state_v& currState, const double& endTime, const double& startTime, const pitch_target_s& qtaParams) const
{
	// setup
	double t (endTime - startTime); // sample time
	const unsigned& N (m_filterOrder);
	//currState = state_v(N, 0.0);

	// filter coefficients
	coeff_v c (N, 0.0);
	calc_filter_coeffs(c, qtaParams, currState);

	for (unsigned n=0; n<N; ++n)
	{
		// calculate value of nth derivative
		double acc (0.0);
		for (unsigned i=0; i<N; ++i)
		{
			// pre-calculate q-value
			double q (0.0);
			for (unsigned k=0; k<std::min(N-i,n+1); ++k)
			{
				q += (std::pow(-qtaParams.l,n-k)*binomial(n,k)*c[i+k]*factorial(k+i)/factorial(i));
			}

			acc += (std::pow(t,i)*q);
		}

		currState[n] = acc * std::exp(-qtaParams.l*t);
	}

	// correction for linear targets
	if (N > 1)
	{
		currState[0] += (qtaParams.b+qtaParams.m*t);
	}
	if (N > 2)
	{
		currState[1] += qtaParams.m;
	}
}

/********** class QtaErrorFunction **********/

QtaErrorFunction::QtaErrorFunction (const signal_s& origF0, const state_v& initState, const time_v& intervalBounds)
: m_lowPassFilter(initState, intervalBounds), m_origF0(origF0), m_lambda(0.0)
{
	if (m_origF0.sampleTimes.size() != (unsigned)m_origF0.sampleValues.size())
	{
		throw dlib::error("[QtaErrorFunction] Original f0 samples doesn't match its sample times: " + std::to_string(m_origF0.sampleValues.size()) + " != " + std::to_string(m_origF0.sampleTimes.size()));
	}
}

void QtaErrorFunction::set_orig_f0(const signal_s& origF0)
{
	if (origF0.sampleTimes.size() != (unsigned)origF0.sampleValues.size())
	{
		throw dlib::error("[set_orig_f0] Original f0 samples doesn't match its sample times: " + std::to_string(origF0.sampleValues.size()) + " != " + std::to_string(origF0.sampleTimes.size()));
	}

	m_origF0 = origF0;
}

void QtaErrorFunction::initialize_filter (const state_v& initState, const time_v& intervalBounds)
{
	m_lowPassFilter.initialize(initState, intervalBounds);
}

void QtaErrorFunction::get_filter_response (signal_s& filteredF0, const target_v& qtaVector) const
{
	m_lowPassFilter.calc_f0(filteredF0, qtaVector);
}

double QtaErrorFunction::operator() ( const la_col_vec& arg) const
{
	target_v qtaVector;
	for (int i=0; i<arg.size(); i+=3)
	{
		pitch_target_s tmp;
		tmp.m = arg(i+0);
		tmp.b = arg(i+1);
		tmp.l = arg(i+2);
		qtaVector.push_back(tmp);
	}

	return cost_function(qtaVector);
}

double QtaErrorFunction::mean_absolute_error (const target_v& qtaVector) const
{
	signal_s filteredF0;
	filteredF0.sampleTimes = m_origF0.sampleTimes;
	m_lowPassFilter.calc_f0(filteredF0, qtaVector);

	// return error between filtered and original f0
	return dlib::mean(dlib::abs(filteredF0.sampleValues - m_origF0.sampleValues));
}

double QtaErrorFunction::mean_squared_error (const target_v& qtaVector) const
{
	signal_s filteredF0;
	filteredF0.sampleTimes = m_origF0.sampleTimes;
	m_lowPassFilter.calc_f0(filteredF0, qtaVector);

	// return error between filtered and original f0
	return dlib::mean(dlib::squared(filteredF0.sampleValues - m_origF0.sampleValues));
}

double QtaErrorFunction::root_mean_squared_error (const target_v& qtaVector) const
{
	// return error between filtered and original f0
	return std::sqrt( mean_squared_error(qtaVector) );
}

double QtaErrorFunction::maximum_norm_error (const target_v& qtaVector) const
{
	signal_s filteredF0;
	filteredF0.sampleTimes = m_origF0.sampleTimes;
	m_lowPassFilter.calc_f0(filteredF0, qtaVector);

	// return error between filtered and original f0
	return dlib::max( dlib::abs(filteredF0.sampleValues - m_origF0.sampleValues));
}

double QtaErrorFunction::correlation_coeff (const target_v& qtaVector) const
{
	signal_s filteredF0;
	filteredF0.sampleTimes = m_origF0.sampleTimes;
	m_lowPassFilter.calc_f0(filteredF0, qtaVector);

	// return correlation between filtered and original f0
	la_col_vec x = m_origF0.sampleValues - dlib::mean(m_origF0.sampleValues);
	la_col_vec y = filteredF0.sampleValues - dlib::mean(filteredF0.sampleValues);
	return (dlib::dot(x,y)) / ( (std::sqrt( dlib::sum(dlib::squared(x))) ) * (std::sqrt( dlib::sum(dlib::squared(y))) ) );
}

double QtaErrorFunction::cost_function (const target_v& qtaVector) const
{
	// setup regularization
	double N (qtaVector.size());
	const pitch_target_s& low = m_searchBounds[0].lower;
	const pitch_target_s& up = m_searchBounds[0].upper;

	// calculate error
	double sseSlope (0.0), sseOffset(0.0), sseStrength(0.0);
	for (auto t : qtaVector)
	{
		// scale data
		double normedSlope = 2*((t.m-low.m)/(up.m-low.m))-1;
		double normedOffset = 2*((t.b-low.b)/(up.b-low.b))-1;
		double normedStrength = ((t.l-low.l)/(up.l-low.l))-1;

		// accumulate summed error
		sseSlope += (normedSlope*normedSlope);
		sseOffset += (normedOffset*normedOffset);
		sseStrength += (normedStrength*normedStrength);
	}

	// penalty term
	double penalty = sseSlope + (0.25)*sseOffset + (0.25)*sseStrength;

	// cost function to minimize
	return mean_squared_error(qtaVector) + (1.0/N)*m_lambda*penalty;
}

double QtaErrorFunction::max_velocity (const target_v& qtaVector) const
{
	signal_s filteredF0;
	filteredF0.sampleTimes = m_origF0.sampleTimes;

	// filter
	m_lowPassFilter.calc_f0(filteredF0, qtaVector);

	// numerical derivation
	la_col_vec deriv; deriv.set_size(filteredF0.sampleValues.size());
	deriv(deriv.size()-1) = 0.0;
	for (int i=0; i<deriv.size()-1; ++i)
	{
		deriv(i) = (filteredF0.sampleValues(i+1) - filteredF0.sampleValues(i))/(filteredF0.sampleTimes[i+1]-filteredF0.sampleTimes[i]);
	}

	return dlib::max(deriv);
}

/********** class PraatFileIo **********/

void PraatFileIo::read_input(QtaErrorFunction& qtaError, std::vector<bound_s>& searchSpace, std::ifstream& fin)
{
	try{
		// container for string values
		std::string line;
		std::vector<std::string> tokens;

		// container for input values
		state_v initState;
		searchSpace.clear();
		time_v syllableBounds;

		// first line
		std::getline(fin, line);
		unsigned M (std::stoi(line));	// filterOrder

		// second line
		std::getline(fin, line);
		tokens = dlib::split(line, " ");
		double fInit (std::stod(tokens[0]));	// initial f0 - speaker value
		bool use_fInit (std::stoi(tokens[1]));

		// third line
		std::getline(fin, line);
		unsigned I (std::stoi(line));	// number of intervals

		// fourth line
		std::getline(fin, line);
		tokens = dlib::split(line, " ");
		// search bounds/targets
		pitch_target_s lowerBound, upperBound;
		lowerBound.m = std::stod(tokens[0]);
		lowerBound.b = std::stod(tokens[2]);
		lowerBound.l = std::stod(tokens[4]);
		upperBound.m = std::stod(tokens[1]);
		upperBound.b = std::stod(tokens[3]);
		upperBound.l = std::stod(tokens[5]);
		searchSpace.push_back({lowerBound, upperBound});
		// interval information
		syllableBounds.push_back(std::stod(tokens[6]));
		syllableBounds.push_back(std::stod(tokens[7]));

		// following lines: further targets
		for (unsigned int i=1; i<I; ++i)
		{
			std::getline(fin, line);
			tokens = dlib::split(line, " ");
			syllableBounds.push_back(std::stod(tokens[7]));
			lowerBound.m = std::stod(tokens[0]);
			lowerBound.b = std::stod(tokens[2]);
			lowerBound.l = std::stod(tokens[4]);
			upperBound.m = std::stod(tokens[1]);
			upperBound.b = std::stod(tokens[3]);
			upperBound.l = std::stod(tokens[5]);
			searchSpace.push_back({lowerBound, upperBound});
		}

		// next line
		std::getline(fin, line);
		unsigned N (std::stoi(line));	// number of samples

		// resulting lines
		signal_s f0;
		f0.sampleValues.set_size(N);
		for (unsigned int i=0; i<N; ++i)
		{
			std::getline(fin, line);
			tokens = dlib::split(line, " ");
			f0.sampleTimes.push_back(std::stod(tokens[0]));
			f0.sampleValues(i) = std::stod(tokens[1]);
		}

		// correct wrong segmentation (word ends before last F0 value)
		if (syllableBounds[I] < f0.sampleTimes[N-1])
		{
			syllableBounds[I] = f0.sampleTimes[N-1];
		}

		// calculate initial state
		if (use_fInit)
		{
			initState.push_back(fInit);
		}
		else
		{
			initState.push_back(f0.sampleValues(0));
		}

		for (unsigned int i=1; i<M; ++i)
		{
			initState.push_back(0.0);
		}

		// write results
		calc_sample_times(m_outSampleTimes, syllableBounds[0], syllableBounds[I]);
		qtaError.initialize_filter(initState, syllableBounds);
		qtaError.set_orig_f0(f0);
	}
	catch (std::invalid_argument& e)
	{
		throw dlib::error("[read_config_file] invalid argument exceptions occurred while using std::stod!\n" + std::string(e.what()));
	}
}

void PraatFileIo::read_praat_file(QtaErrorFunction& qtaError, std::vector<bound_s>& searchSpace, const std::string& inputFile)
{
	// create a file-reading object
	std::ifstream fin;
	fin.open(inputFile); // open data file
	if (!fin.good())
	{
		throw dlib::error("[read_data_file] input file not found!");
	}

	target_v qtaTargets;
	read_input(qtaError, searchSpace, fin);
}

void PraatFileIo::read_praat_file(QtaErrorFunction& qtaError, target_v& optParams, const std::string& inputFile)
{
	std::vector<bound_s> tmp;
	read_praat_file(qtaError, tmp, inputFile);
	optParams.clear();
	for (bound_s s : tmp)
	{
		optParams.push_back(s.lower);
	}
}

void PraatFileIo::calc_sample_times(time_v& sampleTimes, const double& begin, const double& end) const
{
	double sampleRate (100); // Hz
	sampleTimes.clear();
	for (double t=begin; t<=end; t+=(1/sampleRate))
	{
		sampleTimes.push_back(t);
	}
	sampleTimes.push_back(end);
}

void PraatFileIo::write_praat_file(const QtaErrorFunction& qtaError, const target_v& optParams, const std::string& outputFile) const
{
	// create output file and write results to it
	std::ofstream fout;
	fout.open (outputFile);
	fout << std::fixed << std::setprecision(6);

	// line 1: mae + rmse + corr
	fout << qtaError.mean_absolute_error(optParams) << " " << qtaError.root_mean_squared_error(optParams) << " " << qtaError.correlation_coeff(optParams) << std::endl;

	// from line 2: optimal parameters
	for (pitch_target_s p : optParams)
	{
		fout << p.m << " " << p.b << " " << p.l << std::endl;
	}

	// next line: number of samples
	fout << m_outSampleTimes.size() << std::endl;

	// remaining lines: sample points and filtered F0
	signal_s filteredF0;
	filteredF0.sampleTimes = m_outSampleTimes;
	qtaError.get_filter_response(filteredF0, optParams);
	for (unsigned i=0; i<filteredF0.sampleTimes.size(); ++i)
	{
		fout << filteredF0.sampleTimes[i] << " " << filteredF0.sampleValues(i) << std::endl;
	}

	// close output file
	fout.close();

	// DEBUG message
	#ifdef DEBUG_MSG
	//std::cout << "\t[write_praat_file] rmse = " << qtaError.root_mean_squared_error(optParams) << std::endl;
	#endif
}

/********** class Optimizer **********/
void Optimizer::optimize(target_v& optParams, QtaErrorFunction& qtaError, const std::vector<bound_s>& searchSpace, const unsigned& randIters) const
{
	// constraints
	unsigned nIntervals = searchSpace.size();
	double mmin = searchSpace[0].lower.m;
	double mmax = searchSpace[0].upper.m;
	double bmin = searchSpace[0].lower.b;
	double bmax = searchSpace[0].upper.b;
	double lmin = searchSpace[0].lower.l;
	double lmax = searchSpace[0].upper.l;

	la_col_vec lowerBound;
	la_col_vec upperBound;
	lowerBound.set_size(nIntervals*3);
	upperBound.set_size(nIntervals*3);

	for (unsigned i=0; i<nIntervals; ++i)
	{
		lowerBound(3*i+0) = mmin;
		lowerBound(3*i+1) = bmin;
		lowerBound(3*i+2) = lmin;
		upperBound(3*i+0) = mmax;
		upperBound(3*i+1) = bmax;
		upperBound(3*i+2) = lmax;
	}

	// optmization setup
	long npt (2*lowerBound.size()+1);	// number of interpolation points
	const double rho_begin ( (std::min(std::min(mmax-mmin, bmax-bmin),lmax-lmin)-1)/2.0 ); // initial trust region radius
	const double rho_end (1e-6); // stopping trust region radius -> accuracy
	const long max_f_evals (1e6); // max number of objective function evaluations

	// initialize
	double fmin (1e6);
	la_col_vec xtmp; double ftmp;
	unsigned itNum (10+nIntervals*5);
	dlib::mutex mu;

	dlib::parallel_for(0, itNum, [&](unsigned it)
	{
		// random initialization
		la_col_vec x;
		x.set_size(nIntervals*3);
		for (unsigned i=0; i<nIntervals; ++i)
		{
			x(3*i+0) = get_rand(mmin,mmax);
			x(3*i+1) = get_rand(bmin,bmax);
			x(3*i+2) = get_rand(lmin,lmax);
		}

		try
		{
			// optimization algorithm: BOBYQA
			ftmp = dlib::find_min_bobyqa(qtaError,x,npt,lowerBound,upperBound,rho_begin,rho_end,max_f_evals);
		}
		catch (dlib::bobyqa_failure& err)
		{
			// DEBUG message
			#ifdef DEBUG_MSG
			std::cout << "\t[optimize] WARNING: no convergence during optimization in iteration: " << it << std::endl << err.info << std::endl;
			#endif
		}

		// write optimization results back
		dlib::auto_mutex lock(mu);
		if (ftmp < fmin && ftmp > 0.0)	// opt returns 0 by error
		{
			fmin = ftmp;
			xtmp = x;
		}
	});

	if (fmin == 1e6)
	{
		throw dlib::error("[optimize] BOBYQA algorithms didn't converge! Try to increase number of evaluations");
	}

	// convert result to target_v
	optParams.clear();
	for (unsigned i=0; i<nIntervals; ++i)
	{
		pitch_target_s opt;
		opt.m = xtmp(3*i+0);
		opt.b = xtmp(3*i+1);
		opt.l = xtmp(3*i+2);
		optParams.push_back(opt);
	}

	// DEBUG message
	#ifdef DEBUG_MSG
	//std::cout << "\t[optimize] mse = " << fmin << std::endl;
	#endif
}

double Optimizer::get_rand (const double& min, const double& max) const
{
	return min + ((double)rand()/RAND_MAX)*(max-min);
}
