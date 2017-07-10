#include <unistd.h> // getopt()
#include <sys/stat.h> // mkdir()
#include <stdlib.h>	// exit()
#include <iostream>
#include <string>

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

/***** command line processing *****/
static void parse_command_line(int argc, char* argv[], std::string &task, std::string &sampleFile, std::string &outputPath, std::string &algorithm)
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

	if (argc != 9)
	{
		std::cerr << "Wrong number of command line arguments!" << std::endl;
		show_usage(argv[0]);
		exit (EXIT_FAILURE);
	}

	if (aValue == NULL)
	{
		std::cerr << "No algorithm specified!" << std::endl;
		show_usage(argv[0]);
		exit (EXIT_FAILURE);
	}

	// process command line arguments
	task = std::string(tValue);
	sampleFile = std::string(fValue);
	algorithm = std::string(aValue);
	outputPath = std::string(dValue);

	if ( !(algorithm == "mlp" || algorithm == "svr") )
	{
		std::cerr << "Wrong algorithm specified: " << algorithm << std::endl;
		show_usage(argv[0]);
		exit (EXIT_FAILURE);
	}

	if ( !(task == "training" || task == "prediction" || task == "cross-validation" || task == "model-selection") )
	{
		std::cerr << "Wrong task specified: " << task << std::endl;
		show_usage(argv[0]);
		exit (EXIT_FAILURE);
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
		std::cerr << "Error while creating directory: " << outputPath << std::endl;
		exit (EXIT_FAILURE);
	}
}

int main(int argc, char* argv[])
{
	std::string task (""), sampleFile (""), outputPath (""), algorithm ("");
	parse_command_line (argc, argv, task, sampleFile, outputPath, algorithm);


	return 0;
}
