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
	// public member functions
	phon_feat_v get_features() { return m_features; };

protected:
	// private member functions
	void determine_features (std::string sampa);

	// data members
	phon_feat_v m_features;
};

class Consonant : public Phoneme {
public:
	// constructor
	Consonant (const std::string& sampa) { determine_features(sampa); }

private:
	// private member functions
	void determine_features (std::string sampa);

	// data members
	static const std::map<std::string, phon_feat_v> CONSONANTS;
};

class Vowel : public Phoneme {
public:
	// constructor
	Vowel (const std::string& sampa) { determine_features(sampa); }

private:
	// private member functions
	void determine_features (std::string sampa);

	// data members
	static const std::map<std::string, phon_feat_v> VOWELS;
};

class Syllable {
public:
	// constructor
	Syllable (const std::string& sampa, const pos_feat_v& posFeat, const acc_feat_v& accFeat);

	// public member functions
	syl_feat_s get_features() { return m_features; };
	feat_t get_number_phonemes() { return m_nOnset+m_nNucleus+m_nCoda; };

private:
	// private member functions
	std::vector<phon_feat_v> determine_phon_features (std::string sampa);

	// data members
	syl_feat_s m_features;
	feat_t m_nOnset, m_nCoda, m_nNucleus;
};

class Word {
public:
	// constructors
	Word (const std::string& sampa, const std::string& accents) { determine_feature_string(sampa, accents); };

	// public member functions
	std::vector<std::string> get_feature_string_vector () { return m_feature_string; };

private:
	// private member functions
	void determine_feature_string (std::string sampa, std::string accents);
	static void filter_accents (std::string& sampa);

	// data members
	std::vector<std::string> m_feature_string;
};

class DataAssembler {
public:
	// constructor
	DataAssembler (const std::string& sampaFile, const std::string& targetFile);

	// public member functions
	void write_to_file(const std::string& sampleFile);

private:
	// private member functions
	void read_input_files(const std::string& sampaFile, const std::string& targetFile);

	// data members
	std::map<std::string,Word> 						m_featMap;
	std::map<std::string,std::vector<std::string>> 	m_targetMap;
};


#endif /* SAMPLING_H_ */
