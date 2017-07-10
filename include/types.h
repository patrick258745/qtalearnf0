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

/********** qta2f0 types **********/
typedef double 					target_t;
typedef double 					time_t;
typedef double 					freq_t;
typedef double 					state_t;
typedef double 					coeff_t;
typedef std::vector<time_t>		time_v;
typedef std::vector<state_t>	state_v;
typedef std::vector<freq_t> 	freq_v;
typedef std::vector<coeff_t>	coeff_v;

struct param_s
{
	target_t slope;
	target_t offset;
	target_t strength;
};

struct bound_s
{
	param_s lower;
	param_s upper;
};

/********** qtadata types **********/
const unsigned int NUMPHONONSET	= 3;
const unsigned int NUMPHONCODA	= 4;
const unsigned int NUMPHON		= NUMPHONONSET + NUMPHONCODA + 1;

const unsigned int NUMFEATVOW	= 5;
const unsigned int NUMFEATCON	= 7;
const unsigned int NUMFEATPHON	= NUMFEATCON*(NUMPHONONSET+NUMPHONCODA) + 1*NUMFEATVOW;
const unsigned int NUMFEATACC	= 3;
const unsigned int NUMFEATPOS	= 8;
const unsigned int NUMFEATSYL	= NUMFEATPHON + NUMFEATACC + NUMFEATPOS;

typedef signed int 				feat_t;
typedef std::vector<feat_t> 	phon_feat_v;
typedef std::vector<feat_t>		acc_feat_v;
typedef std::vector<feat_t>		pos_feat_v;

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
