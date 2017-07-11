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

/********** qta2f0 types **********/
typedef dlib::matrix<double,0,1> col_vec;

typedef col_vec	time_v;
typedef col_vec	state_v;
typedef col_vec freq_v;
typedef col_vec	coeff_v;
typedef col_vec	param_v;

struct bound_s
{
	param_v lower;
	param_v upper;
};

struct signal_s
{
	time_v sampleTimes;
	freq_v sampleValues;
};

/********** qtadata types **********/
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

typedef signed int 			feat_t;
typedef double				target_t;
typedef std::vector<feat_t> phon_feat_v;
typedef std::vector<feat_t>	acc_feat_v;
typedef std::vector<feat_t>	pos_feat_v;

struct syl_feat_s
{
	std::vector<phon_feat_v> 	phonemes;
	acc_feat_v 					accents;
	pos_feat_v 					positions;
};

typedef std::vector<syl_feat_s>				word_feat_v;
typedef std::map<std::string, word_feat_v> 	feat_m;

typedef std::vector<target_t>			target_v;
typedef std::map<std::string, target_v>	target_m;

#endif /* TYPES_H_ */
