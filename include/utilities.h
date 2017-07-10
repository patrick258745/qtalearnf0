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

class Exception : public std::exception {
public:
	Exception(const std::string& msg): m_message(msg) {}

	const char * what () const throw ()
	{
		return m_message.c_str();
	}

protected:
	std::string m_message;
};

}

#endif /* UTILITIES_H_ */
