#include <unistd.h> // getopt()
#include <sys/stat.h> // mkdir()
#include <iostream>
#include <string>
#include <utilities.h>
#include <types.h>

static void show_usage(std::string name)
{
    std::cerr << "Usage: " << name << " [OPTIONS]...\n"
              << "\nOptions:\n"
              << "\t-h, (help)\t\tShow this help message\n"
              << "\t-t STRING, (task)\t{training,prediction,cross-validation,model-selection}\n"
			  << "\t-a STRING, (algorithm)\tChoose Machine Learning algorithm\n"
			  << "\t-i FILE, (input)\tSpecify sample file\n"
			  << "\t-d DIR, (directory)\tSpecify parent directory for input/output data\n"
			  << "\nExamples:\n"
			  << "\t" << name << " -t training -i <training-file> -d <path-to-parent-dir> -a svr\n"
			  << "\t" << name << " -t prediction -i <test-file> -d <path-to-parent-dir> -a mlp\n"
			  << "\t" << name << " -t cross-validation-i <sample-file> -d <path-to-parent-dir> -a lrr\n"
			  << "\t" << name << " -t model-selection -i <sample-file> -d <path-to-parent-dir> -a rbf\n"
              << std::endl;
}

/***** command line processing; returns 1 for help *****/
static int parse_command_line(int argc, char* argv[], std::string &task, std::string &sampleFile, std::string &outputPath, std::string &algorithm)
{
	unsigned hFlag = 0;		// help flag
	char *tValue = NULL;	// task
	char *fValue = NULL;	// input file
	char *dValue = NULL;	// directory (input/output)
	char *aValue = NULL;	// machine learning algorithm
	int clArgument;

	// get command line arguments
	while ((clArgument = getopt(argc, argv, "ht:i:d:a:")) != -1)
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

	if (argc != 9)
	{
		throw util::CommandLineError("Wrong number of command line arguments!");
	}

	// process command line arguments
	task = std::string(tValue);
	sampleFile = std::string(fValue);
	algorithm = std::string(aValue);
	outputPath = std::string(dValue);

	if ( !(algorithm == "mlp" || algorithm == "svr") )
	{
		throw util::CommandLineError("Wrong algorithm specified: " + algorithm);
	}

	if ( !(task == "training" || task == "prediction" || task == "cross-validation" || task == "model-selection") )
	{
		throw util::CommandLineError("Wrong task specified: " + task);
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
