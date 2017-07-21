/*
 * types.h
 *
 *  Created on: 10 July 2017
 *      Author: Patrick Schmager
 */

#ifndef TYPES_H_
#define TYPES_H_

#include <map>
#include <vector>
#include <string>
#include <dlib/matrix.h>
#include <dlib/error.h>

/********** qtamodel types **********/
typedef dlib::matrix<double,0,1> la_col_vec;	// linear algebra column vector
typedef std::vector<double> real_vec;			// vector for real numbers

typedef real_vec	time_v;		// sample times
typedef real_vec	state_v;	// filter state; derivatives
typedef real_vec	coeff_v;	// filter coefficients
typedef la_col_vec 	freq_v;		// frequency values

struct pitchTarget_s
{
	double m;	// slope
	double b;	// offset
	double l;	// strength
};
typedef std::vector<pitchTarget_s> target_v;	// vector of pitch targets

struct bound_s
{
	pitchTarget_s lower;
	pitchTarget_s upper;
};

struct signal_s
{
	time_v sampleTimes;
	freq_v sampleValues;
};

/********** qtasampling types **********/
typedef const unsigned int glob_const;

glob_const NUMPHONONSET	= 3;
glob_const NUMPHONCODA	= 4;
glob_const NUMPHON		= NUMPHONONSET + NUMPHONCODA + 1;

glob_const NUMFEATVOW	= 5;
glob_const NUMFEATCON	= 7;
glob_const NUMFEATPHON	= NUMFEATCON*(NUMPHONONSET+NUMPHONCODA) + 1*NUMFEATVOW;
glob_const NUMFEATACC	= 3;
glob_const NUMFEATPOS	= 8;
glob_const NUMFEATSYL	= NUMFEATPHON + NUMFEATACC + NUMFEATPOS;

typedef double	 			feat_t;
typedef double				ptarget_t;
typedef std::vector<feat_t> phon_feat_v;
typedef std::vector<feat_t>	acc_feat_v;
typedef std::vector<feat_t>	pos_feat_v;

struct syl_feat_s
{
	std::vector<phon_feat_v> 	phonemes;
	acc_feat_v 					accents;
	pos_feat_v 					positions;
};

/********** qtatraining types **********/
struct qtaTarget_s
{
	pitchTarget_s params;
	double d;	// duration
	double r;	// root-mean-squared-error
	double c;	// correlation-coefficient
};

/********** qtatools types **********/
struct plot_data_s
{
	std::string 		label;
	std::string 		directory;
	double 				shift;
	double				rmse;
	std::vector<double> bound;
	std::vector<double> slope;
	std::vector<double> offset;
};

#endif /* TYPES_H_ */
