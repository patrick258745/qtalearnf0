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
typedef double 						target_t;
typedef double 						time_t;
typedef double 						frequency_t;
typedef std::vector<time_t> 		time_v;
typedef std::vector<frequency_t> 	frequency_v;

struct parameter_s
{
	target_t slope;
	target_t offset;
	target_t strength;
};

struct bounds_s
{
	parameter_s lower;
	parameter_s upper;
};

/********** qtadata types **********/
const unsigned int NUM_PHONEMES_ONSET		= 3;
const unsigned int NUM_PHONEMES_CODA		= 4;
const unsigned int NUM_PHONEMES				= NUM_PHONEMES_ONSET + NUM_PHONEMES_CODA + 1;

const unsigned int NUM_VOWEL_FEATURES		= 5;
const unsigned int NUM_CONSONANT_FEATURES	= 7;
const unsigned int NUM_PHONETIC_FEATURES	= NUM_CONSONANT_FEATURES*(NUM_PHONEMES_ONSET+NUM_PHONEMES_CODA) + 1*NUM_VOWEL_FEATURES;
const unsigned int NUM_ACCENT_FEATURES		= 3;
const unsigned int NUM_POSITION_FEATURES	= 8;
const unsigned int NUM_SYLLABLE_FEATURES	= NUM_PHONETIC_FEATURES + NUM_ACCENT_FEATURES + NUM_POSITION_FEATURES;

typedef signed int 				feature_t;
typedef std::vector<feature_t> 	phoneme_feature_v;
typedef std::vector<feature_t>	accent_feature_v;
typedef std::vector<feature_t>	position_feature_v;

struct syllable_feature_s
{
	std::vector<phoneme_feature_v> 	phonemes;
	accent_feature_v 				accents;
	position_feature_v 				positions;
};

typedef std::vector<syllable_feature_s>			word_feature_v;
typedef std::map<std::string, word_feature_v> 	feature_m;

typedef std::vector<target_t>			target_v;
typedef std::map<std::string, target_v>	target_m;

#endif /* TYPES_H_ */
