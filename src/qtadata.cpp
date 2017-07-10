#include <unistd.h> // getopt()
#include <sys/stat.h> // mkdir()
#include <stdlib.h>	// exit()
#include <iostream>
#include <string>

/***** print out usage information *****/
static void show_usage(std::string name)
{
    std::cerr << "Usage: " << name << " [OPTIONS]...\n"
              << "\nOptions:\n"
              << "\t-h, (help)\t\tShow this help message\n"
              << "\t-s FILE, (sampa-file)\tSpecify file with SAMPA transcriptions\n"
			  << "\t-t FILE, (target-file)\tSpecify file with optimal qTA parameters\n"
			  << "\t-o DIR, (output)\tSpecify directory for output files\n"
			  << "\nExamples:\n"
			  << "\t" << name << " -s <sampa-file> -t <target-file> -d <path-to-output-dir>\n"
              << std::endl;
}

/***** command line processing *****/
static void parse_command_line(int argc, char* argv[], std::string &sampaFile, std::string &targetFile, std::string &outputPath)
{
	/***** command line parsing *****/
	unsigned hFlag = 0;		// help flag
	char *fValue = NULL;	// sampa file (input)
	char *qValue = NULL;	// target file (input)
	char *oValue = NULL;	// output directory
	int clArgument;

	// get command line arguments
	while ((clArgument = getopt(argc, argv, "hs:t:o:")) != -1)
	{
		switch (clArgument)
		{
		case 'h':
			hFlag = 1;
			break;
		case 'f':
			fValue = optarg;
			break;
		case 'q':
			qValue = optarg;
			break;
		case 'o':
			oValue = optarg;
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

	if (argc != 7)
	{
		std::cerr << "Wrong number of command line arguments!" << std::endl;
		show_usage(argv[0]);
		exit (EXIT_FAILURE);
	}

	// process command line arguments
	sampaFile = std::string(fValue);
	targetFile = std::string(qValue);
	outputPath = std::string(oValue);
	if (outputPath.back() != '/')
	{
		outputPath += "/";
	}

	// create specified directories if neccessary
	mkdir(outputPath.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
}

int main(int argc, char* argv[])
{
	// variables declaration
	std::string sampaFile (""), targetFile (""), outputPath ("");

	// parse command line arguments and get information
	parse_command_line (argc, argv, sampaFile, targetFile, outputPath);

	return 0;
}
