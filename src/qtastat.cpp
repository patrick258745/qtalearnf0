#include <unistd.h> // getopt()
#include <sys/stat.h> // mkidir()
#include <iostream>
#include <string>
#include "types.h"
#include "statistics.h"
static void show_usage(std::string name)
{
    std::cerr << "Usage: " << name << " [OPTIONS]...\n"
              << "\nOptions:\n"
              << "\t-h, (help)\t\tShow this help message\n"
              << "\t-a STRING, (algorithm)\tCreate plots for algorithm\n"
			  << "\t-d DIR, (data)\t\tSpecify parent directory for target file\n"
			  << "\t-i DIR, (input)\t\tPath to data files for plot\n"
			  << "\t-o DIR, (output)\tPath to parent dir of image files\n"
			  << "\t-s VAL, (shift)\tSpecify syllable shift\n"
			  << "\nExamples:\n"
			  << "\t" << name << " -a qta -i <input-dir> -o <output-dir> -f <target-file>\n"
			  << "\t" << name << " -a svr -i <input-dir> -o <output-dir> -f <target-file> -s 50\n"
              << std::endl;
}

/***** command line processing; returns 1 for help *****/
static int parse_command_line(int argc, char* argv[], std::string &algorithm, std::string &dataPath, std::string &inputPath, std::string &outputPath, double &syllableShift)
{
	// command line arguments
	unsigned hFlag = 0;		// search flag
	char *aValue = NULL;	// qualifier
	char *dValue = NULL;	// target file (input)
	char *iValue = NULL;	// plot data directory (input)
	char *oValue = NULL;	// image data directory (output)
	char *sValue = NULL;	// syllable shift
	int clArgument;

	// get command line arguments
	while ((clArgument = getopt(argc, argv, "ha:i:o:d:s:")) != -1)
	{
		switch (clArgument)
		{
		case 'h':
			hFlag = 1;
			break;
		case 'a':
			aValue = optarg;
			break;
		case 'd':
			dValue = optarg;
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
			throw dlib::error("[parse_command_line] Wrong option specifier!");
		default:
			throw dlib::error("[parse_command_line] Wrong usage of command line options!");
		}
	}

	// check command line arguments
	if (hFlag)
	{
		return 1;
	}

	if (argc != 9 && argc != 11)
	{
		throw dlib::error("[parse_command_line] Wrong number of command line arguments!");
	}

	if (aValue == NULL || dValue == NULL || iValue == NULL || oValue == NULL)
	{
		throw dlib::error("[parse_command_line] Missing command line option!");
	}

	// process command line arguments
	algorithm = std::string(aValue);
	dataPath = std::string(dValue);
	inputPath = std::string(iValue);
	outputPath = std::string(oValue);

	if ( !(algorithm == "qta" || algorithm == "mlp" || algorithm == "svr") )
	{
		throw dlib::error("[parse_command_line] Wrong algorithm specified: " + algorithm);
	}

	if (inputPath.back() != '/')
	{
		inputPath += "/";
	}
	if (outputPath.back() != '/')
	{
		outputPath += "/";
	}
	if (dataPath.back() != '/')
	{
		dataPath += "/";
	}

	// create specified directories if neccessary
	int status;
	status = mkdir(outputPath.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	outputPath += (algorithm + "/");
	status = mkdir(outputPath.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	if (status == -1)
	{
		std::cerr << "[parse_command_line] WARNING: Directory " << outputPath << "already exists! Overwriting data." << std::endl;
	}

	// convert string to double
	try
	{
		if (sValue != NULL)
		{
			syllableShift = std::stod(std::string(sValue));
		}
		else
		{
			syllableShift = 0.0;
		}
	}
	catch (std::invalid_argument &e)
	{
		throw dlib::error("[parse_command_line] Error while converting " + std::string(sValue) + " to a number!\n" + e.what());
	}

	return 0;
}

int main(int argc, char* argv[])
{
	double syllableShift;
	std::string algorithm (""), dataPath (""), inputPath (""), outputPath ("");

	try
	{
		if (parse_command_line (argc, argv, algorithm, dataPath, inputPath, outputPath, syllableShift))
		{
			show_usage(argv[0]);
			return 1;
		}

		/********** main script **********/
		// Evaluater results (algorithm, dataPath, syllableShift);
		// results.generate_plots(inputPath, outputPath);
		// results.generate_plots(inputPath);

		/*********************************/

	}
	catch (std::exception& e)
	{
		std::cerr << "[main] Program was terminated because an exception was caught!\n" << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return 0;
}
