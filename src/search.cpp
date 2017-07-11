#include <dlib/optimization.h>
#include "search.h"

/********** class CdlpFilter **********/

CdlpFilter::CdlpFilter (const state_v& initState, const double& initTime)
	: m_filterOrder(initState.size()), m_initState(initState), m_initTime(initTime)
{
	if (initState.size() == 0)
	{
		throw util::ExitOnError("[CdlpFilter] Filter can't be initialized with an empty state!");
	}
}

void CdlpFilter::initialize (const state_v& initState, const double& initTime)
{
	if (initState.size() == 0)
	{
		throw util::ExitOnError("[initialize] Filter can't be initialized with an empty state!");
	}

	m_filterOrder = initState.size();
	m_initState = initState;
	m_initTime = initTime;
}

void CdlpFilter::calc_filter_coeffs (coeff_v& filterCoeffs, const param_v& qtaParams) const
{
	// parameters
	const double& m (param_v(0));
	const double& b (param_v(1));
	const double& l (param_v(2));

	filterCoeffs(0) = m_initState(0) - b;		// 0th coefficient
	for (unsigned n=1; n<m_filterOrder; ++n)	// all other coefficients
	{
		double acc (0.0);
		for (unsigned i=0; i<n; ++i)
		{
			acc += ( filterCoeffs(i)*std::pow(-l,n-i)*util::binomial(n,i)*util::factorial(i) );
		}

		if (n==1)
		{
			acc += m; // correction for linear targets; minus changes in following term!
		}

		filterCoeffs(n) = (m_initState(n) - acc)/util::factorial(n);
	}
}

void CdlpFilter::calc_f0 (signal_s& freqResp, const param_v& qtaParams) const
{
	// parameters
	const double& m (param_v(0));
	const double& b (param_v(1));
	const double& l (param_v(2));

	// filter coefficients
	coeff_v c(m_filterOrder);
	calc_filter_coeffs(c, qtaParams);

	unsigned cnt (0);
	for (double t : freqResp.sampleTimes)
	{
		double acc (0.0);
		t -= m_initTime;	// time shift for sample time
		for (unsigned n=0; n<m_filterOrder; ++n)
		{
			acc += (c(n) * std::pow(t,n));
		}

		freqResp.sampleValues(cnt) = acc * std::exp(-l*t) + m*t + b;
		cnt++;
	}
}

void CdlpFilter::calc_state (state_v& currState, const double& currTime, const param_v& qtaParams) const
{
	if (currTime < m_initTime)
	{
		throw util::ExitOnError("[calc_state] Specified time point is smaller than filter initialization time: " + std::to_string(currTime) + " < " + std::to_string(m_initTime));
	}

	// parameters
	const double& m (param_v(0));
	const double& b (param_v(1));
	const double& l (param_v(2));

	// sample time
	double t (currTime - m_initTime);

	// filter coefficients
	coeff_v c(m_filterOrder);
	calc_filter_coeffs(c, qtaParams);

	for (unsigned n=0; n<m_filterOrder; ++n)
	{
		// calculate value of nth derivative
		double acc (0.0);
		for (unsigned i=0; i<m_filterOrder; ++i)
		{
			// pre-calculate q-value
			double q (0.0);
			for (unsigned k=0; k<std::min(n-i,n-1); ++k)
			{
				q += (std::pow(-l,n-k)*util::binomial(n,k)*c(i+k)*util::factorial(k+i)/util::factorial(i));
			}

			acc += (std::pow(t,i)*q);
		}

		currState(n) = acc * std::exp(-l*t);
	}

	// correction for linear targets
	if (m_filterOrder > 1)
	{
		currState(0) += (b+m*t);
	}
	if (m_filterOrder > 2)
	{
		currState(1) += m;
	}
}

/********** class QtaErrorFunction **********/

QtaErrorFunction::QtaErrorFunction (const signal_s& origF0, const state_v& initState, const double& initTime)
: m_lowPassFilter(initState, initTime), m_origF0(origF0)
{
	if (m_origF0.sampleTimes.size() != m_origF0.sampleValues.size())
	{
		throw util::ExitOnError("[QtaErrorFunction] Original f0 samples doesn't match its sample times: " + std::to_string(m_origF0.sampleValues.size()) + " != " + std::to_string(m_origF0.sampleTimes.size()));
	}
}

void QtaErrorFunction::initialize (const signal_s& origF0, const state_v& initState, const double& initTime)
{
	if (origF0.sampleTimes.size() != origF0.sampleValues.size())
	{
		throw util::ExitOnError("[set_orig_f0] Original f0 samples doesn't match its sample times: " + std::to_string(origF0.sampleValues.size()) + " != " + std::to_string(origF0.sampleTimes.size()));
	}

	m_origF0 = origF0;
	m_lowPassFilter.initialize(initState, initTime);
}

void QtaErrorFunction::get_filter_state (state_v& currState, const double& currTime, const param_v& qtaParams) const
{
	m_lowPassFilter.calc_state(currState, currTime, qtaParams);
}

double QtaErrorFunction::operator() ( const param_v& arg) const
{
	return mean_squared_error(arg);
}

double QtaErrorFunction::mean_squared_error (const param_v& qtaParams) const
{
	signal_s filteredF0;
	filteredF0.sampleTimes = m_origF0.sampleTimes;
	filteredF0.sampleValues.set_size(filteredF0.sampleTimes.size());
	m_lowPassFilter.calc_f0(filteredF0, qtaParams);
	return dlib::mean(dlib::squared(filteredF0.sampleValues - m_origF0.sampleValues));
}

double QtaErrorFunction::root_mean_squared_error (const param_v& qtaParams) const
{
	return std::sqrt( mean_squared_error(qtaParams) );
}

double QtaErrorFunction::maximum_norm_error (const param_v& qtaParams) const
{
	signal_s filteredF0;
	filteredF0.sampleTimes = m_origF0.sampleTimes;
	filteredF0.sampleValues.set_size(filteredF0.sampleTimes.size());
	m_lowPassFilter.calc_f0(filteredF0, qtaParams);
	return dlib::max( dlib::abs(filteredF0.sampleValues - m_origF0.sampleValues));
}

double QtaErrorFunction::correlation_coeff (const param_v& qtaParams) const
{
	signal_s filteredF0;
	filteredF0.sampleTimes = m_origF0.sampleTimes;
	filteredF0.sampleValues.set_size(filteredF0.sampleTimes.size());
	m_lowPassFilter.calc_f0(filteredF0, qtaParams);

	col_vec x = m_origF0.sampleValues - dlib::mean(m_origF0.sampleValues);
	col_vec y = filteredF0.sampleValues - dlib::mean(filteredF0.sampleValues);
	return (dlib::sum(x*y)) / ( (std::sqrt( dlib::sum(dlib::squared(x))) ) * (std::sqrt( dlib::sum(dlib::squared(y))) ) );
}

/********** class Optimizer **********/

void PraatFileIo::read_praat_file(QtaErrorFunction& qtaError, bound_s& searchSpace, const std::string corpusDir)
{

}

void PraatFileIo::read_praat_file(QtaErrorFunction& qtaError, param_v& optParams, const std::string corpusDir)
{

}

void PraatFileIo::write_praat_file(const QtaErrorFunction& qtaError, const param_v& optParams, const std::string corpusDir) const
{

}

/********** class Optimizer **********/

void Optimizer::optimize(param_v& optParams, const QtaErrorFunction& qtaError, const bound_s& searchSpace, const unsigned& randIters)
{
	dlib::find_min_bobyqa(qtaError,
			optParams,
			7,    // number of interpolation points
			searchSpace.lower,  // lower bound constraint
			searchSpace.upper,   // upper bound constraint
			10,    // initial trust region radius
			1e-6,  // stopping trust region radius
			100    // max number of objective function evaluations
	);
}
