#include <stdlib.h>	// exit()
#include <iostream>
#include <exception>
#include <dlib/statistics.h>
#include "search.h"

CdlpFilter::CdlpFilter (const unsigned& filterOrder, const state_v& initState, const time_t& initTime = 0.0)
	: m_filterOrder(filterOrder), m_initState(initState), m_initTime(initTime)
{
	if (m_filterOrder != m_initState.size())
	{
		throw util::Exception("Initial state size doesn't match filter order: " + m_filterOrder + " != " + m_initState.size());
	}
}

QtaErrorFunction::QtaErrorFunction (const unsigned& filterOrder, const state_v& initialState, const time_t& sampleTimes, const freq_v& origF0)
: m_lowPassFilter(filterOrder, initialState), m_sampleTimes(sampleTimes), m_origF0(origF0)
{
	if (m_sampleTimes.size() != m_origF0.size())
	{
		throw util::Exception("Original f0 samples doesn't match its sample times: " + m_origF0.size() + " != " << m_sampleTimes.size());
	}
}
