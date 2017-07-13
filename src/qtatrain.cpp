#include <unistd.h> // getopt()
#include <sys/stat.h> // mkdir()
#include <iostream>
#include <string>
#include "types.h"
#include "training.h"

static void show_usage(std::string name)
{
    std::cerr << "Usage: " << name << " [OPTIONS]...\n"
              << "\nOptions:\n"
              << "\t-h, (help)\t\tShow this help message\n"
              << "\t-t STRING, (task)\t{training,prediction,cross-validation,model-selection}\n"
			  << "\t-a STRING, (algorithm)\tChoose Machine Learning algorithm\n"
			  << "\t-f FILE, (input)\tSpecify sample file\n"
			  << "\t-d DIR, (directory)\tSpecify parent directory for input/output data\n"
			  << "\nExamples:\n"
			  << "\t" << name << " -t training -f <training-file> -d <path-to-parent-dir> -a svr\n"
			  << "\t" << name << " -t prediction -f <test-file> -d <path-to-parent-dir> -a mlp\n"
			  << "\t" << name << " -t cross-validation -f <sample-file> -d <path-to-parent-dir> -a lrr\n"
			  << "\t" << name << " -t model-selection -f <sample-file> -d <path-to-parent-dir> -a rbf\n"
              << std::endl;
}

/***** command line processing; returns 1 for help *****/
static int parse_command_line(int argc, char* argv[], std::string &task, std::string &sampleFile, std::string &outputPath, std::string &algorithm)
{
	// command line arguments
	unsigned hFlag = 0;		// help flag
	char *tValue = NULL;	// task
	char *fValue = NULL;	// input file
	char *dValue = NULL;	// directory (input/output)
	char *aValue = NULL;	// machine learning algorithm
	int clArgument;

	// get command line arguments
	while ((clArgument = getopt(argc, argv, "ht:f:d:a:")) != -1)
	{
		switch (clArgument)
		{
		case 'h':
			hFlag = 1;
			break;
		case 't':
			tValue = optarg;
			break;
		case 'f':
			fValue = optarg;
			break;
		case 'd':
			dValue = optarg;
			break;
		case 'a':
			aValue = optarg;
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

	if (argc != 9)
	{
		throw dlib::error("[parse_command_line] Wrong number of command line arguments!");
	}

	if (tValue == NULL || fValue == NULL || aValue == NULL || dValue == NULL)
	{
		throw dlib::error("[parse_command_line] Missing command line option!");
	}

	// process command line arguments
	task = std::string(tValue);
	sampleFile = std::string(fValue);
	algorithm = std::string(aValue);
	outputPath = std::string(dValue);

	if ( !(algorithm == "mlp" || algorithm == "svr") )
	{
		throw dlib::error("[parse_command_line] Wrong algorithm specified: " + algorithm);
	}

	if ( !(task == "training" || task == "prediction" || task == "cross-validation" || task == "model-selection") )
	{
		throw dlib::error("[parse_command_line] Wrong task specified: " + task);
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
		std::cerr << "[parse_command_line] WARNING: Directory " << outputPath << "already exists! Overwriting data." << std::endl;
	}

	return 0;
}

int main(int argc, char* argv[])
{
	std::string task (""), sampleFile (""), outputPath (""), algorithm ("");

	try
	{
		if (parse_command_line (argc, argv, task, sampleFile, outputPath, algorithm))
		{
			show_usage(argv[0]);
			return 1;
		}

		/********** main script **********/
		// MlaTrainer regression (algorithm, outputPath);
		// regression.perform_task(task, sampleFile)

		/*********************************/

	}
	catch (std::exception& e)
	{
		std::cerr << "[main] Program was terminated because an exception was caught!\n" << e.what() << std::endl;
		return EXIT_FAILURE;
	}


	return 0;
}
