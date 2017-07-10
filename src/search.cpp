#include <stdlib.h>	// exit()
#include <iostream>
#include <dlib/statistics.h>
#include "search.h"

CdlpFilter::CdlpFilter (const unsigned& filterOrder, const state_v& initState, const time_t& initTime = 0.0)
	: m_filterOrder(filterOrder), m_initState(initState), m_initTime(initTime)
{
	if (m_filterOrder != m_initState.size())
	{
		std::cerr << "Error: Initial state has size " << m_initState.size() << " but filter order is " << m_filterOrder << "!"  << std::endl;
		exit (EXIT_FAILURE);
	}
}

QtaErrorFunction::QtaErrorFunction (const unsigned& filterOrder, const state_v& initialState, const time_t& sampleTimes, const freq_v& origF0)
: m_lowPassFilter(filterOrder, initialState), m_sampleTimes(sampleTimes), m_origF0(origF0)
{
	if (m_sampleTimes.size() != m_origF0.size())
	{
		std::cerr << "Error: Original f0 samples doesn't match its sample times: " << m_origF0.size() << " != " << m_sampleTimes.size() << std::endl;
		exit (EXIT_FAILURE);
	}
}
