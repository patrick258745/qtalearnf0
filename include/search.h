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
	// public member functions
	CdlpFilter (const unsigned& filterOrder, const state_v& initState, const stime_t& initTime = 0.0);
	void set_order (const unsigned& filterOrder) { m_filterOrder = filterOrder; };
	void set_init_state (const state_v& initState) { m_initState = initState; };
	void set_init_time (const state_v& initTime) { m_initState = initTime; };

	void calc_f0 (freq_v& freqResp, const stime_v& sampleTimes, const param_s& qtaParams) const;
	void calc_state (state_v& currState, const stime_t& currTime, const param_s& qtaParams) const;

private:
	// private member functions
	void calc_filter_coeffs (coeff_v& filterCoeffs, const param_s& qtaParams) const;

	// data members
	unsigned	m_filterOrder;
	state_v		m_initState;
	stime_t		m_initTime;
};

class QtaErrorFunction {
public:
	// public member functions
	QtaErrorFunction (const unsigned& filterOrder, const state_v& initialState, const stime_t& sampleTimes, const freq_v& origF0);
	void set_filter (unsigned filterOrder, state_v initialState);
	void set_sample_times (stime_v& sampleTimes) { m_sampleTimes = sampleTimes; };
	void set_orig_f0 (freq_v& origF0) { m_origF0 = origF0; };
	void get_filter_state (state_v& currState, const stime_t& currTime, const param_s& qtaParams) const;

	double sum_square_error (const param_s& qtaParams) const;
	double maximum_norm_error (const param_s& qtaParams) const;
	double root_mean_square_error (const param_s& qtaParams) const;
	double correlation_coeff (const param_s& qtaParams) const;

	double operator() ( const param_s& arg) const;

private:
	// data members
	CdlpFilter	m_lowPassFilter;
	stime_v		m_sampleTimes;
	freq_v		m_origF0;
};

class PraatFileIo {
public:
	// public member functions
	void read_praat_file(QtaErrorFunction& paramSearch, bound_s& searchSpace, const std::string corpusDir);
	void read_praat_file(QtaErrorFunction& paramPredict, param_s& optParams, const std::string corpusDir);
	void write_praat_file(const QtaErrorFunction& paramSearchPredict, const param_s& optParams, const std::string corpusDir) const;

private:
	// data members
	time_t sylEndTime;
};

class Optimizer {

};


#endif /* SEARCH_H_ */
