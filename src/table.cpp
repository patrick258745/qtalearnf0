#include "types.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <dlib/string.h>

int main(int argc, char* argv[])
{
	// setup
	std::string line;
	std::vector<std::string> tokens;
	std::vector<std::string> results;

	// ********** results file **********
	std::ifstream fin;
	fin.open(std::string(argv[1])); // open input file
	if (!fin.good())
	{
		throw dlib::error("[table] input file not found! " + std::string(argv[1]));
	}

	// read file
	while (std::getline(fin, line))
	{
		tokens = dlib::split(line, ",");
		std::string name (tokens[0]);
		double initf0 = std::stod(tokens[1]);
		unsigned int ptr (4);
		while (tokens.size()>ptr)
		{
			double m = std::stod(tokens[ptr+0]);
			double b = std::stod(tokens[ptr+1]);
			double d = std::stod(tokens[ptr+3]);
			//std::cout << m << "," << b << "," << d << "," << initf0 << std::endl;
			//std::cout << std::to_string(initf0 + b - m*d) << std::endl;
			b = initf0 + b - m*d;
			results.push_back(name+","+tokens[ptr+0]+","+std::to_string(b)+","+tokens[ptr+2]+","+tokens[ptr+3]+"\n");
			std::cout << results.back();
			ptr += 6;
		}
	}

	// store result
	std::ofstream outfile;
	outfile.open(std::string(argv[2]), std::ios_base::app);
	for (std::string s : results)
	{
		outfile << s;
	}

	return 0;
}

/*
 	// setup
	std::string line;
	std::vector<std::string> tokens;
	std::vector<std::string> results (11,"-");

	// ********** results file **********
	std::ifstream fin;
	fin.open(std::string(argv[1])); // open input file
	if (!fin.good())
	{
		throw dlib::error("[table] input file not found! " + std::string(argv[1]));
	}

	// read file
	while (std::getline(fin, line))
	{
		if (line.find(std::string(argv[3])+":") != std::string::npos)
		{
			tokens = dlib::split(line, ": ");
			results[0] = tokens[1];
		}
		else if (line.find("qta.") != std::string::npos)
		{
			std::getline(fin, line);
			tokens = dlib::split(line, "\t");
			results[1] = tokens[0];
			results[2] = tokens[1];
		}
		else if (line.find("lrr.") != std::string::npos)
		{
			std::getline(fin, line);
			tokens = dlib::split(line, "\t");
			results[3] = tokens[0];
			results[4] = tokens[1];
		}
		else if (line.find("krr.") != std::string::npos)
		{
			std::getline(fin, line);
			tokens = dlib::split(line, "\t");
			results[5] = tokens[0];
			results[6] = tokens[1];
		}
		else if (line.find("svr.") != std::string::npos)
		{
			std::getline(fin, line);
			tokens = dlib::split(line, "\t");
			results[7] = tokens[0];
			results[8] = tokens[1];
		}
		else if (line.find("mlp.") != std::string::npos)
		{
			std::getline(fin, line);
			tokens = dlib::split(line, "\t");
			results[9] = tokens[0];
			results[10] = tokens[1];
		}
	}

	// store result
	std::ofstream outfile;
	outfile.open(std::string(argv[2]), std::ios_base::app);
	outfile << results[0] << ","
			<< results[1] << ","
			<< results[2] << ","
			<< results[3] << ","
			<< results[4] << ","
			<< results[5] << ","
			<< results[6] << ","
			<< results[7] << ","
			<< results[8] << ","
			<< results[9] << ","
			<< results[10] << ","
			<< std::endl;

	return 0;
 */
