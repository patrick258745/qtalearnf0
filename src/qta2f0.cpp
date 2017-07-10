#include <unistd.h> // getopt()
#include <stdlib.h>	// exit()
#include <iostream>
#include <string>

/***** print out usage information *****/
static void show_usage(std::string name)
{
    std::cerr << "Usage: " << name << " [OPTIONS]...\n"
              << "\nOptions:\n"
              << "\t-h, (help)\t\tShow this help message\n"
              << "\t-t, (task)\t{search, predict}\n"
			  << "\t-d DIR, (directory)\tSpecify directory for input/output files\n"
			  << "\nExamples:\n"
			  << "\t" << name << " -t search -d <path-to-corpus>\n"
			  << "\t" << name << " -t predict -d <path-to-corpus>\n"
              << std::endl;
}

/***** command line processing *****/
static void parse_command_line(int argc, char* argv[], std::string &task, std::string &corpusPath)
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
			show_usage(argv[0]);
			exit (EXIT_FAILURE);
		default:
			abort();
		}
	}

	// evaluate command line arguments
	if (hFlag)
	{
		show_usage(argv[0]);
		exit (EXIT_SUCCESS);
	}

	if (argc != 5)
	{
		std::cerr << "Wrong number of command line arguments!" << std::endl;
		show_usage(argv[0]);
		exit (EXIT_FAILURE);
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
		std::cerr << "Wrong task specified: " << task << std::endl;
		show_usage(argv[0]);
		exit (EXIT_FAILURE);
	}
}

int main(int argc, char* argv[])
{
	// variables declaration
	std::string task (""), corpusPath ("");

	// parse command line arguments and get information
	parse_command_line (argc, argv, task, corpusPath);

	if (task == "search")
	{
		;
	}
	else if (task == "predict")
	{
		;
	}
	else
	{
		std::cerr << "Error while processing command line arguments!" << std::endl;
		show_usage(argv[0]);
		return -1;
	}

	return 0;
}
