#include <dlib/string.h>
#include "tools.h"

PlotFile::PlotFile (const std::string& label, const std::string& directory, const double& shift)
{
	m_data.label = label;
	m_data.shift = shift;
	m_data.directory = directory;
	if (m_data.directory.back() != '/')
	{
		m_data.directory += "/";
	}
}

void PlotFile::read_plot_data ()
{
	// setup
	std::string line;
	std::vector<std::string> tokens;

	// ********** TextGrid file **********
	// create a file-reading object for TextGrid file (syllable bounds)
	std::string fileName = m_data.directory + m_data.label + ".TextGrid";
	std::ifstream finBound;
	finBound.open(fileName); // open input file
	if (!finBound.good())
	{
		throw dlib::error("[read_plot_data] TextGrid file not found! " + fileName);
	}

	// ignore first 13 lines
	for (unsigned i=0; i<13; ++i)
		std::getline(finBound, line);

	// get number of intervals
	std::getline(finBound, line);
	tokens = dlib::split(line, "=");
	unsigned numberIntervals = std::stoi(tokens[1]);

	try
	{
		// get first syllable bound
		std::getline(finBound, line); // ingnore line "intervals [1]:"
		std::getline(finBound, line); // ingnore line "xmin = 0 "
		std::getline(finBound, line);
		tokens = dlib::split(line, "=");
		m_data.bound.push_back(std::stod(tokens[1]));

		// get further syllable bounds
		for (unsigned i=1; i<numberIntervals-2; ++i)
		{
			std::getline(finBound, line); // ingnore line "intervals [1]:"
			std::getline(finBound, line); // ingnore line "xmin = 0 "
			std::getline(finBound, line);
			std::getline(finBound, line);
			tokens = dlib::split(line, "=");
			m_data.bound.push_back(std::stod(tokens[1])-(m_data.shift/1000));
		}

		// get last syllable bounds
		std::getline(finBound, line); // ingnore line "intervals [1]:"
		std::getline(finBound, line); // ingnore line "xmin = 0 "
		std::getline(finBound, line);
		std::getline(finBound, line);
		tokens = dlib::split(line, "=");
		m_data.bound.push_back(std::stod(tokens[1]));
	}
	catch (std::invalid_argument& e)
	{
		throw dlib::error("[read_config_file] invalid argument (bounds) exceptions occurred while using std::stod!\n" + std::string(e.what()));
	}

	// ********** target file **********
	// create a file-reading object for TextGrid file (syllable bounds)
	fileName = m_data.directory + m_data.label + ".targets";
	std::ifstream finTarget;
	finTarget.open(fileName); // open target file
	if (!finTarget.good())
	{
		throw dlib::error("[read_plot_data] target file not found! " + fileName);
	}

	std::getline(finTarget, line); // ignore first line, header
	unsigned numberTargets (0);
	m_data.rmse = 0.0;
	while (std::getline(finTarget, line))
	{
		++numberTargets;
		tokens = dlib::split(line, "\t");
		try
		{
			m_data.slope.push_back(std::stod(tokens[1]));
			m_data.offset.push_back(std::stod(tokens[2]));
			m_data.rmse += std::stod(tokens[5]);
		}
		catch (std::invalid_argument& e)
		{
			throw dlib::error("[read_config_file] invalid argument (targets) exceptions occurred while using std::stod!\n" + std::string(e.what()));
		}

	}

	if (numberIntervals-2 != numberTargets)
	{
		//throw dlib::error("[read_plot_data] Different number of intervals and targets! " + std::to_string(numberIntervals-2) + " != " + std::to_string(numberTargets));
	}
	m_data.rmse /= (double)numberTargets;
}

void PlotFile::generate_plot_file ()
{
	// setup
	std::ofstream m_file;
	m_file.open(m_data.directory + m_data.label + ".plot");

	// write image file information to plot file
	m_file << "##### output file options #####" << std::endl;
	m_file << "set terminal pngcairo size 1536,512 enhanced font 'Verdana,14'" << std::endl;
	m_file << "set output '" << m_data.label <<".png'" << std::endl;

	// write general plot settings to plot file
	m_file << std::endl << "##### general settings #####" << std::endl;
	m_file << "set datafile missing '0'" << std::endl;
	m_file << "stats '" << m_data.label <<".qtaf0' using 2:3 nooutput" << std::endl;
	m_file << "set xrange [" << m_data.bound[0] << ":" << m_data.bound[m_data.bound.size()-1] <<"]" << std::endl;
	m_file << "set yrange [STATS_min_y-2:STATS_max_y+2]" << std::endl;
	m_file << "set title 'F0 - " << m_data.label <<" (rmse=" << m_data.rmse <<")'" << std::endl;
	m_file << "set xlabel 'Time [sec]'" << std::endl;
	m_file << "set ylabel 'Frequency [st]'" << std::endl;

	//  write syllable bound information to plot file
	m_file << std::endl << "##### plot syllable bounds #####" << std::endl;
	for (unsigned int i=0; i<m_data.bound.size(); ++i)
	{
		m_file << "set arrow from " << std::to_string(m_data.bound[i]) << ",STATS_min_y-2 to " << std::to_string(m_data.bound[i]) << ",STATS_max_y+2 nohead dt 3 lt -1" << std::endl;
	}

	// write target information
	m_file << std::endl << "###### plot targets #####" << std::endl;
	for (unsigned int i=0; i<m_data.slope.size(); ++i)
	{
		double t1 = m_data.bound[i];
		double t2 = m_data.bound[i+1];

		double f1 = m_data.offset[i];
		double f2 = m_data.offset[i] + m_data.slope[i]*(m_data.bound[i+1] - m_data.bound[i]);
		m_file << "set arrow from " << std::to_string(t1) << "," << std::to_string(f1) << " to " << std::to_string(t2) << "," << std::to_string(f2) << " nohead dt 4" << std::endl;
	}

	// write f0 curve information
	m_file << std::endl << "###### plot curves from data points #####" << std::endl;
	m_file << "plot '" << m_data.label <<".origf0' using 2:3 title 'Original F0' with points lt 7 lc '#0000FF' lw 1,\\" << std::endl;
	m_file << " '" 	 << m_data.label <<".qtaf0' using 2:3 title 'Resynthesized F0' with linespoints lt -1 lc black lw 2" << std::endl;

	m_file.close();
}

void PlotFile::plot ()
{
	// prepare plot
	read_plot_data ();
	generate_plot_file ();

	// change directory and call gnuplot
	std::string command = "cd " + m_data.directory + "; gnuplot " + m_data.label + ".plot";
	const int dir_err = system(command.c_str());
	if (-1 == dir_err)
	{
		throw dlib::error("[plot] couldn't execute gnuplot command: " + command);
	}
}
