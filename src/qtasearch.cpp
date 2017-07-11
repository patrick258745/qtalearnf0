#include <unistd.h> // getopt()
#include <iostream>
#include <string>
#include <utilities.h>
#include <types.h>

/***** print out usage information *****/
static void show_usage(std::string name)
{
    std::cerr << "Usage: " << name << " [OPTIONS]...\n"
              << "\nOptions:\n"
              << "\t-h, (help)\t\tShow this help message\n"
              << "\t-t, (task)\t\t{search, predict}\n"
			  << "\t-d DIR, (directory)\tSpecify directory for input/output files\n"
			  << "\nExamples:\n"
			  << "\t" << name << " -t search -d <path-to-corpus>\n"
			  << "\t" << name << " -t predict -d <path-to-corpus>\n"
              << std::endl;
}

/***** command line processing; returns 1 for help *****/
static int parse_command_line(int argc, char* argv[], std::string &task, std::string &corpusPath)
{
	unsigned hFlag = 0;		// help flag
	char *tValue = NULL;	// task specifier
	char *dValue = NULL;	// corpus directory (input)
	int clArgument;

	// get command line arguments
	while ((clArgument = getopt(argc, argv, "ht:d:")) != -1)
	{
		switch (clArgument)
		{
		case 'h':
			hFlag = 1;
			break;
		case 'd':
			dValue = optarg;
			break;
		case 't':
			tValue = optarg;
			break;
		case '?':
			throw util::CommandLineError("Wrong option specifier!");
		default:
			throw util::CommandLineError("Unknown error occurred in getopt()!");
		}
	}

	// evaluate command line arguments
	if (hFlag)
	{
		return 1;
	}

	if (argc != 5)
	{
		throw util::CommandLineError("Wrong number of command line arguments!");
	}

	// process command line arguments
	task = std::string(tValue);
	corpusPath = std::string(dValue);
	if (corpusPath.back() != '/')
	{
		corpusPath += "/";
	}

	if ( !(task == "search" || task == "predict") )
	{
		throw util::CommandLineError("Wrong task specified! " + task);
	}

	return 0;
}

int main(int argc, char* argv[])
{
	// variables declaration
	std::string task (""), corpusPath ("");

	try
	{
		// parse command line arguments and get information
		if (parse_command_line (argc, argv, task, corpusPath))
		{
			show_usage(argv[0]);
			return 0;
		}

		/***** main script *****/
		if (task == "search")
		{
			;
		}
		else if (task == "predict")
		{
			;
		}
		/***********************/
	}
	catch (util::CommandLineError& err)
	{
		std::cerr << "Error while processing command line arguments!\n"  << err.what() << std::endl;
		show_usage(argv[0]);
		return 1;
	}
	catch (util::ExitOnError& err)
	{
		std::cerr << "Program was terminated because an error occurred!\n" << err.what() << std::endl;
		return 1;
	}
	catch (std::exception& e)
	{
		std::cerr << "Program was terminated because an unhandled exception occurred!\n" << e.what() << std::endl;
		std::terminate();
	}

	return 0;
}
