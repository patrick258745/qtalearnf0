/*
 * search.h
 *
 *  Created on: 10 July 2017
 *      Author: Patrick Schmager
 */

#ifndef SEARCH_H_
#define SEARCH_H_

#include "types.h"
#include "utilities.h"

class CdlpFilter {
public:
	// constructors
	CdlpFilter () : m_filterOrder(0), m_initTime(0.0) {};
	CdlpFilter (const state_v& initState, const double& initTime = 0.0);

	// public member functions
	void initialize (const state_v& initState, const double& initTime = 0.0);
	void calc_f0 (signal_s& freqResp, const param_v& qtaParams) const;
	void calc_state (state_v& currState, const double& currTime, const param_v& qtaParams) const;

private:
	// private member functions
	void calc_filter_coeffs (coeff_v& filterCoeffs, const param_v& qtaParams) const;

	// data members
	unsigned	m_filterOrder;
	state_v		m_initState;
	double		m_initTime;
};

class QtaErrorFunction {
public:
	// constructors
	QtaErrorFunction () {};
	QtaErrorFunction (const signal_s& origF0, const state_v& initState, const double& initTime = 0.0);

	// public member functions
	void initialize (const signal_s& origF0, const state_v& initState, const double& initTime = 0.0);
	void get_filter_state (state_v& currState, const double& currTime, const param_v& qtaParams) const;

	double operator() ( const param_v& arg) const;
	double correlation_coeff (const param_v& qtaParams) const;
	double root_mean_squared_error (const param_v& qtaParams) const;

private:
	// private member functions
	double mean_squared_error (const param_v& qtaParams) const;
	double maximum_norm_error (const param_v& qtaParams) const;

	// data members
	CdlpFilter	m_lowPassFilter;
	signal_s	m_origF0;
};

class PraatFileIo {
public:
	// constructors
	PraatFileIo () : m_sylEndTime(0.0) {};
	PraatFileIo (const double& sylEndTime) : m_sylEndTime(sylEndTime) {};

	// public member functions
	void read_praat_file(QtaErrorFunction& qtaError, bound_s& searchSpace, const std::string corpusDir);
	void read_praat_file(QtaErrorFunction& qtaError, param_v& optParams, const std::string corpusDir);
	void write_praat_file(const QtaErrorFunction& qtaError, const param_v& optParams, const std::string corpusDir) const;

private:
	// data members
	time_t m_sylEndTime;
};

class Optimizer {
public:
	// public member functions
	static void optimize(param_v& optParams, const QtaErrorFunction& qtaError, const bound_s& searchSpace, const unsigned& randIters = 10);
};

#endif /* SEARCH_H_ */
