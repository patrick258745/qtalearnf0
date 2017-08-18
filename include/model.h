/*
 * targets.h
 *
 *  Created on: 10 July 2017
 *      Author: Patrick Schmager
 */

#ifndef MODEL_H_
#define MODEL_H_

#include <cstdlib>
#include <fstream>
#include <time.h>
#include "types.h"

class CdlpFilter {
public:
	// constructors
	CdlpFilter () : m_filterOrder(0) {};
	CdlpFilter (const state_v& initState, const time_v& intervalBounds);

	// public member functions
	void initialize (const state_v& initState, const time_v& intervalBounds);
	void calc_f0 (signal_s& freqResp, const target_v& qtaVector) const;

private:
	// private member functions
	void calc_filter_coeffs (coeff_v& filterCoeffs, const pitch_target_s& qtaParams, const state_v& startState) const;
	void calc_state (state_v& currState, const double& currTime, const double& startTime, const pitch_target_s& qtaParams) const;

	// data members
	unsigned	m_filterOrder;
	state_v		m_initState;
	time_v		m_intervalBounds;
};

class QtaErrorFunction {
public:
	// constructors
	QtaErrorFunction () : m_penalty(0) {};
	QtaErrorFunction (const signal_s& origF0, const state_v& initState, const time_v& intervalBounds);

	// public member functions
	void set_orig_f0 (const signal_s& origF0);
	void initialize_filter (const state_v& initState, const time_v& intervalBounds);
	void get_filter_response (signal_s& filteredF0, const target_v& qtaVector) const;

	double operator() ( const la_col_vec& arg) const;
	double correlation_coeff (const target_v& qtaVector) const;
	double mean_absolute_error (const target_v& qtaVector) const;
	double root_mean_squared_error (const target_v& qtaVector) const;

	double cost_function (const target_v& qtaVector) const;
	double penalty_function (const target_v& qtaVector) const;

	double		m_penalty;

private:
	// private member functions
	double mean_squared_error (const target_v& qtaVector) const;
	double maximum_norm_error (const target_v& qtaVector) const;
	double max_velocity (const target_v& qtaVector) const;

	// data members
	CdlpFilter	m_lowPassFilter;
	signal_s	m_origF0;
};

class PraatFileIo {
public:
	// public member functions
	void read_praat_file(QtaErrorFunction& qtaError, std::vector<bound_s>& searchSpace, const std::string& inputFile);
	void read_praat_file(QtaErrorFunction& qtaError, target_v& optParams, const std::string& inputFile);
	void write_praat_file(const QtaErrorFunction& qtaError, const target_v& optParams, const std::string& outputFile) const;

private:
	// private member functions
	void read_input(QtaErrorFunction& qtaError, std::vector<bound_s>& qtaParams, std::ifstream& fin);
	void calc_sample_times(time_v& sampleTimes, const double& begin, const double& end) const;

	// data members
	time_v m_outSampleTimes;
};

class Optimizer {
public:
	// constructors
	Optimizer() { srand (time(NULL)); }; // srand (0);

	// public member functions
	void optimize(target_v& optParams, QtaErrorFunction& qtaError, const std::vector<bound_s>& searchSpace, const unsigned& randIters = 10) const;

private:
	// private member functions
	double get_rand (const double& min, const double& max) const;
};



#endif /* MODEL_H_ */
