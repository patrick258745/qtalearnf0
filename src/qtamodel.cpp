#include <iostream>
#include <string>
#include <dlib/cmd_line_parser.h>
#include "types.h"
#include "model.h"

int main(int argc, char* argv[])
{
	// command line variables
	std::string inputFile, outputFile;

	try
	{
		/********** command line parsing **********/
		dlib::command_line_parser parser;

		// command line options
		parser.add_option("h","Display this help message.");
		parser.set_group_name("Task Options");
		parser.add_option("s","Search for optimal qTA parameters.");
		parser.add_option("r","Read optimal qTA parameters from input file.");
		parser.set_group_name("File Options");
		parser.add_option("in","Specify praat input file.",1);
		parser.add_option("out","Specify praat output file.",1);

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
        	inputFile = parser.option("in").argument();
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
			praatFiles.read_praat_file(qtaError, searchSpace, inputFile);
			paramSearch.optimize(optParams, qtaError, searchSpace);
		}
		else
		{
			praatFiles.read_praat_file(qtaError, optParams, inputFile);
		}

		// save results to praat file
		praatFiles.write_praat_file(qtaError, optParams, outputFile);

		/*********************************/
	}
	catch (std::exception& e)
	{
		std::cerr << "[main] Program was terminated because an error occurred!\n" << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
