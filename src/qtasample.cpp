#include <iostream>
#include <string>
#include <dlib/cmd_line_parser.h>
#include "types.h"
#include "sampling.h"

int main(int argc, char* argv[])
{
	// command line variables
	std::string sampaFile, targetFile, outputFile;

	try
	{
		// ********** command line parsing **********
		dlib::command_line_parser parser;

		// command line options
		parser.add_option("h","Display this help message.");
		parser.set_group_name("File Options");
		parser.add_option("in","Specify sampa <arg1> and target <arg2> input files.",2);
		parser.add_option("out","Specify sample output file containing training data.",1);

		// parse command line
		parser.parse(argc,argv);

		// check command line options
		const char* one_time_opts[] = {"h", "in", "out"};
		parser.check_one_time_options(one_time_opts);

		// process command line options
		if (parser.option("h"))
		{
			std::cout << "Usage: qtasearch [options]\n";
			parser.print_options();
			return EXIT_SUCCESS;
		}

        if (parser.option("in"))
		{
        	sampaFile = parser.option("in").argument();
        	targetFile = parser.option("in").argument(1);
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

		// ********** main script **********


		// *********************************
	}
	catch (std::exception& e)
	{
		std::cerr << "[main] Program was terminated because an error occurred!\n" << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
