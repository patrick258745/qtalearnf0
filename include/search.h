/*
 * search.h
 *
 *  Created on: 10 July 2017
 *      Author: Patrick Schmager
 */

#ifndef SEARCH_H_
#define SEARCH_H_

#include <cstdlib>
#include <time.h>
#include "types.h"

class CdlpFilter {
public:
	// constructors
	CdlpFilter () : m_filterOrder(0), m_initTime(0.0) {};
	CdlpFilter (const state_v& initState, const double& initTime = 0.0);

	// public member functions
	void initialize (const state_v& initState, const double& initTime = 0.0);
	void calc_f0 (signal_s& freqResp, const pitchTarget_s& qtaParams) const;
	void calc_state (state_v& currState, const double& currTime, const pitchTarget_s& qtaParams) const;

private:
	// private member functions
	void calc_filter_coeffs (coeff_v& filterCoeffs, const pitchTarget_s& qtaParams) const;

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
	void set_orig_f0 (const signal_s& origF0);
	void initialize_filter (const state_v& initState, const double& initTime = 0.0);
	void get_filter_state (state_v& currState, const double& currTime, const pitchTarget_s& qtaParams) const;
	void get_filter_response (signal_s& filteredF0, const pitchTarget_s& qtaParams) const;

	double operator() ( const la_col_vec& arg) const;
	double correlation_coeff (const pitchTarget_s& qtaParams) const;
	double root_mean_squared_error (const pitchTarget_s& qtaParams) const;

private:
	// private member functions
	double mean_squared_error (const pitchTarget_s& qtaParams) const;
	double maximum_norm_error (const pitchTarget_s& qtaParams) const;

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
	void read_praat_file(QtaErrorFunction& qtaError, bound_s& searchSpace, const std::string& configFile, const std::string& dataFile);
	void read_praat_file(QtaErrorFunction& qtaError, pitchTarget_s& optParams, const std::string& configFile, const std::string& dataFile);
	void write_praat_file(const QtaErrorFunction& qtaError, const pitchTarget_s& optParams, const std::string& outputFile) const;

private:
	// private member functions
	void read_config_file(QtaErrorFunction& qtaError, bound_s& searchSpace, const std::string& configFile);
	void read_data_file(QtaErrorFunction& qtaError, const std::string& dataFile) const;
	void calc_sample_times(time_v& sampleTimes, const double& begin, const double& end) const;

	// data members
	double m_sylEndTime;
	time_v m_outSampleTimes;
};

class Optimizer {
public:
	// constructors
	Optimizer() { srand (time(NULL)); }; // srand (0);

	// public member functions
	void optimize(pitchTarget_s& optParams, const QtaErrorFunction& qtaError, const bound_s& searchSpace, const unsigned& randIters = 10) const;

private:
	// private member functions
	double get_rand (const double& min, const double& max) const;
};



#endif /* SEARCH_H_ */
