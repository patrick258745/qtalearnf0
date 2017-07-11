#include <unistd.h> // getopt()
#include <sys/stat.h> // mkidir()
#include <iostream>
#include <string>
#include <utilities.h>
#include <types.h>

static void show_usage(std::string name)
{
    std::cerr << "Usage: " << name << " [OPTIONS]...\n"
              << "\nOptions:\n"
              << "\t-h, (help)\t\tShow this help message\n"
              << "\t-a STRING, (algorithm)\tCreate plots for algorithm\n"
			  << "\t-f FILE, (file)\t\tSpecify file with qta targets\n"
			  << "\t-i DIR, (input)\t\tPath to data files for plot\n"
			  << "\t-o DIR, (output)\tPath to parent dir of image files\n"
			  << "\nExamples:\n"
			  << "\t" << name << " -q qta -i <input-dir> -o <output-dir>\n"
			  << "\t" << name << " -q svr -i <input-dir> -o <output-dir> -s 50\n"
              << std::endl;
}

/***** command line processing; returns 1 for help *****/
static int parse_command_line(int argc, char* argv[], std::string &algorithm, std::string &targetFile, std::string &inputPath, std::string &outputPath, double &syllableShift)
{
	unsigned hFlag = 0;		// search flag
	char *aValue = NULL;	// qualifier
	char *fValue = NULL;	// target file (input)
	char *iValue = NULL;	// plot data directory (input)
	char *oValue = NULL;	// image data directory (output)
	char *sValue = NULL;	// syllable shift
	int clArgument;

	// get command line arguments
	while ((clArgument = getopt(argc, argv, "ha:i:o:f:")) != -1)
	{
		switch (clArgument)
		{
		case 'h':
			hFlag = 1;
			break;
		case 'a':
			aValue = optarg;
			break;
		case 'f':
			fValue = optarg;
			break;
		case 'i':
			iValue = optarg;
			break;
		case 'o':
			oValue = optarg;
			break;
		case 's':
			sValue = optarg;
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

	if (argc != 7 || argc != 9)
	{
		throw util::CommandLineError("Wrong number of command line arguments!");
	}

	// process command line arguments
	algorithm = std::string(aValue);
	targetFile = std::string(fValue);
	inputPath = std::string(iValue);
	outputPath = std::string(oValue);

	if ( !(algorithm == "qta" || algorithm == "mlp" || algorithm == "svr") )
	{
		throw util::CommandLineError("Wrong algorithm specified: " + algorithm);
	}

	if (inputPath.back() != '/')
	{
		inputPath += "/";
	}
	if (outputPath.back() != '/')
	{
		outputPath += "/";
	}

	// create specified directories if neccessary
	int status;
	status = mkdir(outputPath.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	outputPath += (algorithm + "/");
	status = mkdir(outputPath.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	if (status == -1)
	{
		throw util::ExitOnError("Error while creating directory: " + outputPath);
	}

	// convert string to double
	try
	{
		syllableShift = std::stod(sValue);
	}
	catch (std::invalid_argument &e)
	{
		throw util::ExitOnError("Error while converting " + std::string(sValue) + " to a number!\n" + e.what());
	}

	return 0;
}

int main(int argc, char* argv[])
{
	double syllableShift;
	std::string algorithm (""), targetFile (""), inputPath (""), outputPath ("");

	try
	{
		if (parse_command_line (argc, argv, algorithm, targetFile, inputPath, outputPath, syllableShift))
		{
			show_usage(argv[0]);
			return 1;
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
