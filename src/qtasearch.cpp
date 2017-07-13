#include <iostream>
#include <string>
#include <dlib/misc_api.h>
#include <dlib/cmd_line_parser.h>
#include "types.h"
#include "search.h"

int main(int argc, char* argv[])
{
	// variables declaration
	std::string configFile, dataFile, outputFile;

	try
	{
		/********** command line parsing **********/
		dlib::command_line_parser parser;

		// command line options
		parser.add_option("h","Display this help message.");
		parser.add_option("s","Search for optimal parameters.");
		parser.add_option("r","Read optimal parameters from config file.");
		parser.add_option("in","Specify config and data input files.",2);
		parser.add_option("out","Specify output file.",1);

		// parse command line
		parser.parse(argc,argv);

		// check command line options
		const char* one_time_opts[] = {"h", "s", "r", "in", "out"};
		parser.check_one_time_options(one_time_opts);
		parser.check_incompatible_options("s", "r");

		// process command line options
		if (parser.option("h"))
		{
			std::cout << "Usage: qtasearch [options]\n";
			parser.print_options();
			return EXIT_SUCCESS;
		}

        if (!parser.option("s") && !parser.option("r"))
        {
            std::cout << "Error in command line:\n   You must specify either the s option or the r option.\n";
            std::cout << "\nTry the -h option for more information." << std::endl;
            return EXIT_FAILURE;
        }

        if (parser.option("in"))
		{
        	configFile = parser.option("in").argument();
        	dataFile = parser.option("in").argument(1);
		}
		else
		{
			std::cout << "Error in command line:\n   You must specify an input file.\n";
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

		/********** main script **********/
		pitchTarget_s optParams;
		bound_s searchSpace;
		PraatFileIo praatFiles;
		QtaErrorFunction qtaError;
		Optimizer paramSearch;

		// get optimal qta parameters
		if (parser.option("s"))
		{
			praatFiles.read_praat_file(qtaError, searchSpace, configFile, dataFile);
			paramSearch.optimize(optParams, qtaError, searchSpace);
		}
		else
		{
			praatFiles.read_praat_file(qtaError, optParams, configFile, dataFile);
		}

		// save results to praat file
		praatFiles.write_praat_file(qtaError, optParams, outputFile);

		/*********************************/
	}
	catch (std::exception& e)
	{
		std::cerr << "[main] Program was terminated because an exception was caught!\n" << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
