#include <iostream>
#include <string>
#include <dlib/cmd_line_parser.h>
#include "types.h"
#include "tools.h"

int main(int argc, char* argv[])
{
	std::string inputFile, outputFile, modelFile;

	try
	{
		// ********** command line parsing **********
		dlib::command_line_parser parser;

		// command line options
		parser.add_option("h","Display this help message.");
		parser.set_group_name("Task Options");
		parser.add_option("t","Perform training on a set of samples.");
		parser.add_option("p","Perform test/prediction on a set of samples.");
		parser.add_option("c","Perform cross-validation on a set of samples.");
		parser.add_option("m","Perform model-selection on a set of samples.");
		parser.set_group_name("File Options");
		parser.add_option("in","Specify sample input files.",1);
		parser.add_option("out","Specify output file containing predicted targets.",1);
		parser.add_option("model","Specify algorithm model file (xml).",1);

		// parse command line
		parser.parse(argc,argv);

		// check command line options
		const char* one_time_opts[] = {"h", "t", "p", "c", "m", "in", "out", "model"};
		parser.check_one_time_options(one_time_opts);
		const char* incompatible[] = {"t", "p", "c", "m"};
		parser.check_incompatible_options(incompatible);
		const char* t_ops[] = {"t", "c", "m"};
		const char* t_sub_ops[] = {"in", "model"};
		const char* p_sub_ops[] = {"in", "out", "model"};
		parser.check_sub_options(t_ops, t_sub_ops);
		parser.check_sub_options("p", p_sub_ops);

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

		if (parser.option("model"))
		{
			modelFile = parser.option("model").argument();
		}
		else
		{
			std::cout << "Error in command line:\n   You must specify an algorithm model file.\n";
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
