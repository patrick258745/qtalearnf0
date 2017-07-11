#include <unistd.h> // getopt()
#include <sys/stat.h> // mkdir()
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
              << "\t-s FILE, (sampa-file)\tSpecify file with SAMPA transcriptions\n"
			  << "\t-t FILE, (target-file)\tSpecify file with optimal qTA parameters\n"
			  << "\t-o DIR, (output)\tSpecify directory for output files\n"
			  << "\nExamples:\n"
			  << "\t" << name << " -s <sampa-file> -t <target-file> -d <path-to-output-dir>\n"
              << std::endl;
}

/***** command line processing; returns 1 for help *****/
static int parse_command_line(int argc, char* argv[], std::string &sampaFile, std::string &targetFile, std::string &outputPath)
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

	if (argc != 7)
	{
		throw util::CommandLineError("Wrong number of command line arguments!");
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

	return 0;
}

int main(int argc, char* argv[])
{
	// variables declaration
	std::string sampaFile (""), targetFile (""), outputPath ("");

	try
	{
		// parse command line arguments and get information
		if (parse_command_line (argc, argv, sampaFile, targetFile, outputPath))
		{
			show_usage(argv[0]);
			return 0;
		}

		/***** main script *****/

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
