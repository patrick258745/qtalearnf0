/*
 * sample.h
 *
 *  Created on: 10 July 2017
 *      Author: Patrick Schmager
 */

#ifndef SAMPLING_H_
#define SAMPLING_H_

#include <map>
#include "types.h"

class Phoneme {
public:
	// constructors
	Phoneme (const std::string& sampa) { determine_features(sampa); }

	// public member functions
	phon_feat_v get_features() { return m_features; };

private:
	// private member functions
	void determine_features (std::string sampa);

	// data members
	phon_feat_v m_features;
};

class Consonant : Phoneme {
private:
	// private member functions
	void determine_features (std::string sampa);

	// data members
	static const std::map<std::string, phon_feat_v> CONSONANTS{
			{""	  , {0, 0, 0, 0, 0, 0, 0} },
			{"p"  , {0, 0, 1, 0, 0, 0, 0} },
			{"b"  , {1, 0, 1, 0, 0, 0, 0} },
			{"t"  , {0, 0, 1, 0, 0, 0, 1} },
			{"d"  , {1, 0, 1, 0, 0, 0, 1} },
			{"k"  , {0, 0, 1, 0, 0, 0, 2} },
			{"g"  , {1, 0, 1, 0, 0, 0, 2} },
			{"?"  , {1, 0, 1, 0, 0, 0, 3} },
			{"f"  , {0, 0, 0, 1, 0, 0, 0} },
			{"v"  , {1, 0, 0, 1, 0, 0, 0} },
			{"s"  , {0, 0, 0, 1, 0, 0, 1} },
			{"z"  , {1, 0, 0, 1, 0, 0, 1} },
			{"S"  , {0, 0, 0, 1, 0, 0, 1} },
			{"Z"  , {1, 0, 0, 1, 0, 0, 1} },
			{"T"  , {0, 0, 0, 1, 0, 0, 1} },
			{"D"  , {1, 0, 0, 1, 0, 0, 1} },
			{"C"  , {0, 0, 0, 1, 0, 0, 2} },
			{"j"  , {1, 0, 0, 1, 0, 0, 2} },
			{"x"  , {0, 0, 0, 1, 0, 0, 2} },
			{"h"  , {0, 0, 0, 1, 0, 0, 3} },
			{"m"  , {1, 1, 0, 0, 0, 0, 0} },
			{"n"  , {1, 1, 0, 0, 0, 0, 1} },
			{"N"  , {1, 1, 0, 0, 0, 0, 2} },
			{"l"  , {1, 0, 0, 0, 0, 1, 1} },
			{"R"  , {1, 0, 0, 1, 0, 0, 2} },
			{"r"  , {1, 0, 0, 1, 0, 0, 2} },
			{"pf" , {0, 0, 1, 1, 0, 0, 0} },
			{"ps" , {0, 0, 1, 1, 0, 0, 1} },
			{"ts" , {0, 0, 1, 1, 0, 0, 1} },
			{"tS" , {0, 0, 1, 1, 0, 0, 1} },
			{"pS" , {0, 0, 1, 1, 0, 0, 1} },
			{"dZ" , {1, 0, 1, 1, 0, 0, 1} },
	};
};

class Vowel : Phoneme {
private:
	// private member functions
	void determine_features (std::string sampa);

	// data members
	static const std::map<std::string, phon_feat_v> Vowel::VOWELS{
			{"i" , {-2, 3, 0, 0, 0} },
			{"I" , {-1, 2, 0, 0, 0} },
			{"e" , {-2, 1, 0, 0, 0} },
			{"E" , {-2,-1, 0, 0, 0} },
			{"y" , {-2, 3, 1, 0, 0} },
			{"Y" , {-1, 2, 1, 0, 0} },
			{"2" , {-2, 1, 1, 0, 0} },
			{"9" , {-2,-1, 1, 0, 0} },
			{"@" , { 0, 0, 0, 0, 0} },
			{"6" , { 0,-2, 0, 0, 0} },
			{"a" , { 0,-3, 0, 0, 0} },
			{"A" , { 2,-3, 0, 0, 0} },
			{"u" , { 2, 3, 1, 0, 0} },
			{"U" , { 1, 2, 1, 0, 0} },
			{"o" , { 2, 1, 1, 0, 0} },
			{"O" , { 2,-1, 1, 0, 0} },
			{"=" , { 0, 0, 0, 0, 0} }
	};
};

class Syllable {
public:
	// constructor
	Syllable (const std::string& sampa, const pos_feat_v& posFeat, const acc_feat_v& accFeat);

	// public member functions
	syl_feat_s get_features() { return m_features; };

private:
	// private member functions
	std::vector<phon_feat_v> determine_phon_features (std::string sampa);

	// data members
	syl_feat_s m_features;
	feat_t m_nOnset, m_nCoda;
};

class Word {
public:
	// constructors
	Word (const std::string& sampa, const std::string& accents) { determine_feature_string(sampa, accents); };

	// public member functions
	std::string get_feature_string () { return m_feature_string; };

private:
	// private member functions
	void determine_feature_string (std::string sampa, std::string accents);
	static void filter_accents (std::string& sampa);

	// data members
	std::string m_feature_string;
};

class SampaConverter {
public:
	// public member functions

private:
	// private member functions


	// data members

};

class DataAssembler {
public:
	// public member functions

private:
	// private member functions


	// data members

};


#endif /* SAMPLING_H_ */
