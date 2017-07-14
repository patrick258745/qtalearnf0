#include <dlib/string.h>
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

CdlpFilter::CdlpFilter (const state_v& initState, const double& initTime)
	: m_filterOrder(initState.size()), m_initState(initState), m_initTime(initTime)
{
	if (initState.size() == 0)
	{
		throw dlib::error("[CdlpFilter] Filter can't be initialized with an empty state!");
	}
}

void CdlpFilter::initialize (const state_v& initState, const double& initTime)
{
	if (initState.size() == 0)
	{
		throw dlib::error("[initialize] Filter can't be initialized with an empty state!");
	}

	m_filterOrder = initState.size();
	m_initState = initState;
	m_initTime = initTime;
}

void CdlpFilter::calc_filter_coeffs (coeff_v& filterCoeffs, const pitchTarget_s& qtaParams) const
{
	if (filterCoeffs.size() != m_filterOrder)
	{
		throw dlib::error("[calc_filter_coeffs] Wrong size of coefficient vector! " + std::to_string(filterCoeffs.size()) + " != " + std::to_string(m_filterOrder));
	}

	filterCoeffs[0] = m_initState[0] - qtaParams.b;	// 0th coefficient
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

		filterCoeffs[n] = (m_initState[n] - acc)/factorial(n);
	}
}

void CdlpFilter::calc_f0 (signal_s& freqResp, const pitchTarget_s& qtaParams) const
{
	freqResp.sampleValues.set_size(freqResp.sampleTimes.size());

	// filter coefficients
	coeff_v c (m_filterOrder, 0.0);
	calc_filter_coeffs(c, qtaParams);

	unsigned cnt (0);
	for (double t : freqResp.sampleTimes)
	{
		double acc (0.0);
		t -= m_initTime;	// time shift for sample time
		for (unsigned n=0; n<m_filterOrder; ++n)
		{
			acc += (c[n] * std::pow(t,n));
		}

		freqResp.sampleValues(cnt) = acc * std::exp(-qtaParams.l*t) + qtaParams.m*t + qtaParams.b;
		cnt++;
	}
}

void CdlpFilter::calc_state (state_v& currState, const double& currTime, const pitchTarget_s& qtaParams) const
{
	if (currTime < m_initTime)
	{
		throw dlib::error("[calc_state] Specified time point is smaller than filter initialization time: " + std::to_string(currTime) + " < " + std::to_string(m_initTime));
	}

	// setup
	double t (currTime - m_initTime); // sample time
	const unsigned& N (m_filterOrder);
	currState = state_v(N, 0.0);

	// filter coefficients
	coeff_v c (N, 0.0);
	calc_filter_coeffs(c, qtaParams);

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

QtaErrorFunction::QtaErrorFunction (const signal_s& origF0, const state_v& initState, const double& initTime)
: m_lowPassFilter(initState, initTime), m_origF0(origF0)
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

void QtaErrorFunction::initialize_filter (const state_v& initState, const double& initTime)
{
	m_lowPassFilter.initialize(initState, initTime);
}

void QtaErrorFunction::get_filter_state (state_v& currState, const double& currTime, const pitchTarget_s& qtaParams) const
{
	m_lowPassFilter.calc_state(currState, currTime, qtaParams);
}

void QtaErrorFunction::get_filter_response (signal_s& filteredF0, const pitchTarget_s& qtaParams) const
{
	m_lowPassFilter.calc_f0(filteredF0, qtaParams);
}

double QtaErrorFunction::operator() ( const la_col_vec& arg) const
{
	pitchTarget_s qtaParams {arg(0),arg(1),arg(2)}; // slope, offset, strength
	return mean_squared_error(qtaParams);
}

double QtaErrorFunction::mean_squared_error (const pitchTarget_s& qtaParams) const
{
	signal_s filteredF0;
	filteredF0.sampleTimes = m_origF0.sampleTimes;
	m_lowPassFilter.calc_f0(filteredF0, qtaParams);

	// return error between filtered and original f0
	return dlib::mean(dlib::squared(filteredF0.sampleValues - m_origF0.sampleValues));
}

double QtaErrorFunction::root_mean_squared_error (const pitchTarget_s& qtaParams) const
{
	// return error between filtered and original f0
	return std::sqrt( mean_squared_error(qtaParams) );
}

double QtaErrorFunction::maximum_norm_error (const pitchTarget_s& qtaParams) const
{
	signal_s filteredF0;
	filteredF0.sampleTimes = m_origF0.sampleTimes;
	m_lowPassFilter.calc_f0(filteredF0, qtaParams);

	// return error between filtered and original f0
	return dlib::max( dlib::abs(filteredF0.sampleValues - m_origF0.sampleValues));
}

double QtaErrorFunction::correlation_coeff (const pitchTarget_s& qtaParams) const
{
	signal_s filteredF0;
	filteredF0.sampleTimes = m_origF0.sampleTimes;
	m_lowPassFilter.calc_f0(filteredF0, qtaParams);

	// return correlation between filtered and original f0
	la_col_vec x = m_origF0.sampleValues - dlib::mean(m_origF0.sampleValues);
	la_col_vec y = filteredF0.sampleValues - dlib::mean(filteredF0.sampleValues);
	return (dlib::dot(x,y)) / ( (std::sqrt( dlib::sum(dlib::squared(x))) ) * (std::sqrt( dlib::sum(dlib::squared(y))) ) );
}

/********** class PraatFileIo **********/

void PraatFileIo::read_input(QtaErrorFunction& qtaError, bound_s& searchSpace, std::ifstream& fin)
{
	try{
		// container for string values
		std::string line;
		std::vector<std::string> tokens;

		// first line
		std::getline(fin, line);
		tokens = dlib::split(line, " ");
		searchSpace.lower.m = std::stod(tokens[0]); // m_min
		searchSpace.upper.m = std::stod(tokens[1]); // m_max

		// second line
		std::getline(fin, line);
		tokens = dlib::split(line, " ");
		searchSpace.lower.b = std::stod(tokens[0]); // b_min
		searchSpace.upper.b = std::stod(tokens[1]); // b_max

		// third third
		std::getline(fin, line);
		tokens = dlib::split(line, " ");
		searchSpace.lower.l = std::stod(tokens[0]); // lambda_min
		searchSpace.upper.l = std::stod(tokens[1]); // lambda_max

		// fourth line
		std::getline(fin, line); // filterOrder -> implicitly given by number of states
		unsigned M (std::stoi(line));	// filterOrder

		// fifth line
		std::getline(fin, line);
		state_v state;
		tokens = dlib::split(line, " ");
		for (unsigned int i=0; i<M; ++i)
		{
			state.push_back(std::stod(tokens[i]));	//initial states
		}

		// sixth line
		std::getline(fin, line);
		tokens = dlib::split(line, " "); // syllable bounds
		double sylBeginTime (std::stod(tokens[0]));
		m_sylEndTime = std::stod(tokens[1]);

		// 7th line
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

		// write results
		calc_sample_times(m_outSampleTimes, sylBeginTime, m_sylEndTime);
		qtaError.initialize_filter(state, sylBeginTime);
		qtaError.set_orig_f0(f0);
	}
	catch (std::invalid_argument& e)
	{
		throw dlib::error("[read_config_file] invalid argument exceptions occurred while using std::stod!\n" + std::string(e.what()));
	}
}

void PraatFileIo::read_praat_file(QtaErrorFunction& qtaError, bound_s& searchSpace, const std::string& inputFile)
{
	// create a file-reading object
	std::ifstream fin;
	fin.open(inputFile); // open data file
	if (!fin.good())
	{
		throw dlib::error("[read_data_file] input file not found!");
	}

	read_input(qtaError, searchSpace, fin);
}

void PraatFileIo::read_praat_file(QtaErrorFunction& qtaError, pitchTarget_s& optParams, const std::string& inputFile)
{
	bound_s tmp;
	read_praat_file(qtaError, tmp, inputFile);
	optParams = tmp.lower;
}

void PraatFileIo::calc_sample_times(time_v& sampleTimes, const double& begin, const double& end) const
{
	double sampleRate (100); // Hz
	sampleTimes.clear();
	for (double t=begin; t<=end; t+=(1/sampleRate))
	{
		sampleTimes.push_back(t);
	}
}

void PraatFileIo::write_praat_file(const QtaErrorFunction& qtaError, const pitchTarget_s& optParams, const std::string& outputFile) const
{
	// create output file and write results to it
	std::ofstream fout;
	fout.open (outputFile);
	fout << std::fixed << std::setprecision(6);

	// line 1: optimal variables
	fout << optParams.m << " " << optParams.b << " " << optParams.l << std::endl;

	// line 2: final state (derivatives)
	state_v state;
	qtaError.get_filter_state(state, m_sylEndTime, optParams);
	for (double d : state)
	{
		fout << d << " ";
	}

	// line 3: root mean square error and correlation coefficient
	fout << std:: endl << qtaError.root_mean_squared_error(optParams) << " " << qtaError.correlation_coeff(optParams) << std::endl;

	// line 4: number of samples
	fout << m_outSampleTimes.size() << std::endl;

	// from line 5: sample points and filtered F0
	signal_s filteredF0;
	filteredF0.sampleTimes = m_outSampleTimes;
	qtaError.get_filter_response(filteredF0, optParams);
	for (unsigned i=0; i<filteredF0.sampleTimes.size(); ++i)
	{
		fout << filteredF0.sampleTimes[i] << " " << filteredF0.sampleValues(i) << std::endl;
	}

	// close output file
	fout.close();
}

/********** class Optimizer **********/
void Optimizer::optimize(pitchTarget_s& optParams, const QtaErrorFunction& qtaError, const bound_s& searchSpace, const unsigned& randIters) const
{
	// optmization setup
	la_col_vec lowerBound {searchSpace.lower.m, searchSpace.lower.b, searchSpace.lower.l}; // lower bound constraint
	la_col_vec upperBound {searchSpace.upper.m, searchSpace.upper.b, searchSpace.upper.l}; // upper bound constraint
	long npt (2*lowerBound.size()+1);	// number of interpolation points
	const double rho_begin (5); // initial trust region radius
	const double rho_end (1e-6); // stopping trust region radius -> accuracy
	const long max_f_evals (10000); // max number of objective function evaluations

	// random iterations
	la_col_vec x = lowerBound; // optimization variables
	double fmin (qtaError(x));
	double ftmp (fmin);
	unsigned itNum (10);

	for (unsigned i=0; i<itNum; ++i)
	{
		x = get_rand(searchSpace.lower.m,searchSpace.upper.m),get_rand(searchSpace.lower.b,searchSpace.upper.b),get_rand(searchSpace.lower.l,searchSpace.upper.l);
		try
		{
			// optimization algorithm: BOBYQA
			ftmp = dlib::find_min_bobyqa(qtaError,x,npt,lowerBound,upperBound,	rho_begin,rho_end,max_f_evals);
		}
		catch (dlib::bobyqa_failure& err)
		{
			std::cerr << "WARNING: no convergence during optimization in iteration: " << i << std::endl << err.info << std::endl;
		}

		// write optimization results back
		if (ftmp < fmin)
		{
			fmin = ftmp;
			optParams = {x(0), x(1), x(2)}; // slope, offset, strength
		}

		// random initialization for next round
		x = get_rand(searchSpace.lower.m,searchSpace.upper.m),get_rand(searchSpace.lower.b,searchSpace.upper.b),get_rand(searchSpace.lower.l,searchSpace.upper.l);
	}

	// DEBUG: print results
	std::cout << "mse(" << optParams.m << "," << optParams.b << "," << optParams.l << ") = " << fmin << std::endl;
}

double Optimizer::get_rand (const double& min, const double& max) const
{
	return min + ((double)rand()/RAND_MAX)*(max-min);
}
