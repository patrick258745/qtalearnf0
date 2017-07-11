/*
 * utilities.h
 *
 *  Created on: 10 July 2017
 *      Author: Patrick Schmager
 */

#ifndef UTILITIES_H_
#define UTILITIES_H_

#include <iostream>
#include <exception>
#include "types.h"

namespace util {

/********** exceptions **********/

class ExitOnError : public std::exception {
public:
	ExitOnError(const std::string& msg): m_message(msg) {}

	const char * what () const throw ()
	{
		return m_message.c_str();
	}

protected:
	std::string m_message;
};

class CommandLineError : public std::exception {
public:
	CommandLineError(const std::string& msg): m_message(msg) {}

	const char * what () const throw ()
	{
		return m_message.c_str();
	}

protected:
	std::string m_message;
};

/********** math operations **********/
double binomial (const unsigned &n, const unsigned &k);
double factorial (unsigned n);

}

#endif /* UTILITIES_H_ */
