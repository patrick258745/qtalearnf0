/*
 * tools.h
 *
 *  Created on: 10 July 2017
 *      Author: Patrick Schmager
 */

#ifndef TOOLS_H_
#define TOOLS_H_

#include "types.h"

class PlotFile {
public:
	// Constructors
	PlotFile (const std::string& label, const std::string& directory, const double& shift = 0.0);

	// public member functions
	void plot ();

private:
	// private member functions
	void read_plot_data ();
	void generate_plot_file ();

	// data members
	plot_data_s m_data;
};

#endif /* TOOLS_H_ */
