#include <dlib/optimization.h>
#include "search.h"

/********** class CdlpFilter **********/

CdlpFilter::CdlpFilter (const unsigned& filterOrder, const state_v& initState, const double& initTime)
	: m_filterOrder(filterOrder), m_initState(initState), m_initTime(initTime)
{
	if (m_filterOrder != m_initState.size())
	{
		throw util::ExitOnError("[CdlpFilter] Initial state size doesn't match filter order: " + std::to_string(m_filterOrder) + " != " + std::to_string(m_initState.size()));
	}
}

void CdlpFilter::calc_f0 (signal_s& freqResp, const param_v& qtaParams) const
{
	if (freqResp.sampleTimes.size() == 0)
	{
		throw util::ExitOnError("[calc_f0] Can't calculate F0 because there are no sample times!");
	}

	if (m_filterOrder != m_initState.size())
	{
		throw util::ExitOnError("[calc_f0] Can't calculate F0 because filter order doesn't match initial state!");
	}
}

void CdlpFilter::calc_state (state_v& currState, const double& currTime, const param_v& qtaParams) const
{

}

void CdlpFilter::calc_filter_coeffs (coeff_v& filterCoeffs, const param_v& qtaParams) const
{

}

/********** class QtaErrorFunction **********/

QtaErrorFunction::QtaErrorFunction (const unsigned& filterOrder, const state_v& initialState, const signal_s& origF0)
: m_lowPassFilter(filterOrder, initialState), m_origF0(origF0)
{
	if (m_origF0.sampleTimes.size() != m_origF0.sampleValues.size())
	{
		throw util::ExitOnError("[QtaErrorFunction] Original f0 samples doesn't match its sample times: " + std::to_string(m_origF0.sampleValues.size()) + " != " + std::to_string(m_origF0.sampleTimes.size()));
	}
}

void QtaErrorFunction::set_filter (const unsigned& filterOrder, const state_v& initialState)
{

}

void QtaErrorFunction::set_orig_f0 (const signal_s& origF0)
{

}

void QtaErrorFunction::get_filter_state (state_v& currState, const double& currTime, const param_v& qtaParams) const
{

}

double QtaErrorFunction::operator() ( const param_v& arg) const
{
	return mean_squared_error(arg);
}

double QtaErrorFunction::mean_squared_error (const param_v& qtaParams) const
{
	signal_s filteredF0;
	filteredF0.sampleTimes = m_origF0.sampleTimes;
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
	m_lowPassFilter.calc_f0(filteredF0, qtaParams);
	return dlib::max( dlib::abs(filteredF0.sampleValues - m_origF0.sampleValues));
}

double QtaErrorFunction::correlation_coeff (const param_v& qtaParams) const
{
	signal_s filteredF0;
	filteredF0.sampleTimes = m_origF0.sampleTimes;
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

void Optimizer::optimize(param_v& optParams, const QtaErrorFunction& qtaError, const bound_s& searchSpace)
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
