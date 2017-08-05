#include <iostream>
#include <string>
#include <dlib/cmd_line_parser.h>
#include "types.h"
#include "tools.h"

int main(int argc, char* argv[])
{
	std::string inputFile, outputFile, plotDir, label;
	double syllableShift;

	try
	{
		// ********** command line parsing **********
		dlib::command_line_parser parser;

		// command line options
		parser.add_option("h","Display this help message.");
		parser.set_group_name("Task Options");
		parser.add_option("p","Generate plot file.");
		parser.add_option("s","Calculate some statistics.");
		parser.set_group_name("File Options");
		parser.add_option("in","Specify target input files.",1);
		parser.add_option("out","Specify output file (stat or png).",1);
		parser.add_option("dir","Specify plot file directory.",1);
		parser.add_option("label","Specify label of word to plot.",1);
		parser.set_group_name("Additional Plot Options");
		parser.add_option("shift","Specify syllable shift in ms.",1);

		// parse command line
		parser.parse(argc,argv);

		// check command line options
		const char* one_time_opts[] = {"h", "p", "s", "in", "out", "dir", "label"};
		parser.check_one_time_options(one_time_opts);
		const char* incompatible[] = {"p", "s"};
		parser.check_incompatible_options(incompatible);
		const char* required_sub_ops[] = {"dir", "out"};
		const char* t_ops[] = {"p", "s"};
		parser.check_sub_options(t_ops, required_sub_ops);
		parser.check_sub_option("p", "label");
		parser.check_sub_option("s", "in");

		// process command line options
		if (parser.option("h"))
		{
			std::cout << "Usage: qtasearch [options]\n";
			parser.print_options();
			return EXIT_SUCCESS;
		}

		if (parser.option("in"))
		{
			inputFile = parser.option("in").argument();
		}
		else if (parser.option("s"))
		{
			std::cout << "Error in command line:\n   You must specify a target file.\n";
			std::cout << "\nTry the -h option for more information." << std::endl;
			return EXIT_FAILURE;
		}

		if (parser.option("label"))
		{
			label = parser.option("label").argument();
		}
		else if (parser.option("p"))
		{
			std::cout << "Error in command line:\n   You must specify a label.\n";
			std::cout << "\nTry the -h option for more information." << std::endl;
			return EXIT_FAILURE;
		}

		if (parser.option("dir"))
		{
			plotDir = parser.option("dir").argument();
		}
		else
		{
			std::cout << "Error in command line:\n   You must specify a plot file directory.\n";
			std::cout << "\nTry the -h option for more information." << std::endl;
			return EXIT_FAILURE;
		}

		if (parser.option("out"))
		{
			outputFile = parser.option("out").argument();
		}
		else
		{
			std::cout << "Error in command line:\n   You must specify an output file.\n";
			std::cout << "\nTry the -h option for more information." << std::endl;
			return EXIT_FAILURE;
		}

		syllableShift = get_option(parser,"shift",0.0);

		// ********** main script **********
		if (parser.option("p"))
		{
			PlotFile F0plot (label, plotDir, syllableShift);
			F0plot.plot(outputFile);
		}
		else
		{
			Statistics stat;
			stat.print(inputFile, outputFile);
			stat.plot(inputFile, plotDir);
		}

		// *********************************
	}
	catch (std::exception& e)
	{
		std::cerr << "[main] Program was terminated because an error occurred!\n" << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
