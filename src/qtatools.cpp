#include <iostream>
#include <string>
#include <dlib/cmd_line_parser.h>
#include "types.h"
#include "tools.h"

int main(int argc, char* argv[])
{
	std::string inputFile, plotDir, label;

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
		parser.add_option("dir","Specify plot file directory.",1);
		parser.add_option("label","Specify label of word to plot.",1);

		// parse command line
		parser.parse(argc,argv);

		// check command line options
		const char* one_time_opts[] = {"h", "p", "s", "in", "dir", "label"};
		parser.check_one_time_options(one_time_opts);
		const char* incompatible[] = {"p", "s"};
		parser.check_incompatible_options(incompatible);
		parser.check_sub_option("p", "dir");
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

		if (parser.option("dir"))
		{
			plotDir = parser.option("dir").argument();
		}

		if (parser.option("label"))
		{
			plotDir = parser.option("label").argument();
		}
		else
		{
			std::cout << "Error in command line:\n   You must specify a label.\n";
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
