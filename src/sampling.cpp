#include <regex>
#include "dlib/string.h"
#include "sampling.h"

void Consonant::determine_features (std::string sampa)
{
	// collect information
	feat_t voicing (1);

	// compute unvoiced consonant modifier
	std::size_t position = sampa.find("_0");
	if (position != std::string::npos)
	{
		if ( (position != 0) && (sampa.length() <= 5) && (sampa[position+2] == '\0') )
		{
			sampa.erase(position,2);
			voicing = 0;
		}
		else
		{
			throw dlib::error("[Consonant] Invalid unvoiced consonant transcription found: " + sampa);
		}
	}

	// check validity
	if (sampa.length() > 2)
	{
		throw dlib::error("[Consonant] Invalid or unused {consonant,modifier} combination found: " + sampa);
	}

	// determine features
	try
	{
		m_features = CONSONANTS.at(sampa);
		m_features[0] = voicing;

	}
	catch (std::exception& e)
	{
		throw dlib::error("[Consonant] Invalid or unused SAMPA consonant symbol found: " + sampa);
	}
}

void Vowel::determine_features (std::string sampa)
{
	// collect information
	phon_feat_v diphthongVowel {0,0,0,0,0}, diphthongVowel2 {0,0,0,0,0};
	feat_t length (0), nasal (0);
	bool dipht1 (false), dipht2 (false);

	// compute long vowel modifier
	std::size_t position = sampa.find(":");
	if (position != std::string::npos)
	{
		if (position != 0)
		{
			sampa.erase(position,1);
			length++;
		}
		else
		{
			throw dlib::error("[Vowel] Invalid long vowel transcription found: " + sampa);
		}
	}

	// compute nasal vowel modifier
	position = sampa.find("~");
	if (position != std::string::npos)
	{
		if (position != 0)
		{
			sampa.erase(position,1);
			nasal++;
		}
		else
		{
			throw dlib::error("[Vowel] Invalid nasal vowel transcription found: " + sampa);
		}
	}

	// compute first diphthong modifier
	position = sampa.find("_^");
	if (position != std::string::npos)
	{
		if (position != 0)
		{
			sampa.erase(position,2);
			dipht1 = true;
			diphthongVowel = VOWELS.at(sampa.substr(0,1));
			sampa.erase(0,1);
		}
		else
		{
			throw dlib::error("[Vowel] Invalid first diphthong transcription found: " + sampa);
		}
	}

	// compute second diphthong modifier, e.g. Ingenieur
	position = sampa.find("_^");
	if (position != std::string::npos)
	{
		if (position != 0)
		{
			sampa.erase(position,2);
			dipht2 = true;
			diphthongVowel2 = VOWELS.at(sampa.substr(0,1));
			sampa.erase(0,1);
		}
		else
		{
			throw dlib::error("[Vowel] Invalid second diphthong transcription found: " + sampa);
		}
	}

	// compute syllabic consonant modifier
	position = sampa.find("=");
	if (position != std::string::npos)
	{
		if (position == 0 && sampa.length() < 2)
		{
			sampa = "=";
		}
		else
		{
			throw dlib::error("[Vowel] Invalid syllabic consonant transcription found: " + sampa);
		}
	}

	// check validity
	if (sampa.length() > 1)
	{
		throw dlib::error("[Vowel] Invalid or unused {vowel,modifier} combination found: " + sampa);
	}

	// determine features
	try
	{
		m_features = VOWELS.at(sampa);
		if (dipht1)
		{
			length++;
			// calculate mean for diphthongs -> rounding problem?
			if (dipht2)
			{
				length++;
				m_features[0] = (m_features[0] + diphthongVowel[0] + diphthongVowel2[0])/3.0;
				m_features[1] = (m_features[1] + diphthongVowel[1] + diphthongVowel2[1])/3.0;
				m_features[2] = (m_features[2] + diphthongVowel[2] + diphthongVowel2[2])/3.0;
			}
			else
			{
				m_features[0] = (m_features[0] + diphthongVowel[0])*0.5;
				m_features[1] = (m_features[1] + diphthongVowel[1])*0.5;
				m_features[2] = (m_features[2] + diphthongVowel[2])*0.5;
			}
		}

		// set length and nasal features
		m_features[3] = length;
		m_features[4] = nasal;
	}
	catch (std::exception& e)
	{
		throw dlib::error("[Vowel] Invalid or unused SAMPA vowel symbol found: " + sampa);
	}
}

Syllable::Syllable (const std::string& sampa, const pos_feat_v& posFeat, const acc_feat_v& accFeat)
{
	m_features.accents = accFeat;
	m_features.positions = posFeat;
	m_nOnset = 0; m_nCoda = 0;
	m_features.phonemes = determine_phon_features(sampa);
}

std::vector<phon_feat_v> Syllable::determine_phon_features (std::string sampa)
{
	// container
	std::vector<Phoneme> phones;
	std::vector<phon_feat_v> features;

	// filter '_' at syllable beginning e.g. "t._t"
	if (sampa[0] == '_')
	{
		sampa.erase(0,1);
	}

	// determine onset, nucleus and coda by regex
	std::regex vowels("[aAoeEiIOuUyY269@=](:|~|_\\^){0,3}[aAoeEiIOuUy269@=]?(:|~|_\\^){0,3}[aAoeEiIOuUy269@=]?(:|~|_\\^){0,3}");
	std::regex consonants("((\\?|pf|ps|ts|tS|pS|dZ|p|b|t|d|k|g|f|v|s|z|S|Z|T|D|C|x|h|m|n|N|l|j|R|r)(_0)?)?");

	// first: find nucleus, onset and coda
	std::vector<std::string> tokens;
	std::string nucleus, onset, coda;
	std::regex_search (sampa, nucleus, vowels);
	tokens = dlib::split(sampa,nucleus);
	std::size_t pos = sampa.find(nucleus);

	if (!tokens[0].empty())
	{
		if (!tokens[1].empty())
		{
			onset = tokens[0];
			coda = tokens[1];
		}
		else if (pos != 0)
		{
			coda = tokens[0];
		}
		else
		{
			onset = tokens[0];
		}
	}

	// DEBUG
	std::cout << onset << " " << nucleus << " " << coda << std::endl;

	// determine phonemes
	std::regex_iterator<std::string::iterator> rend;
	std::regex_iterator<std::string::iterator> rOnset ( onset.begin(), onset.end(), consonants );
	std::regex_iterator<std::string::iterator> rCoda ( onset.begin(), onset.end(), consonants );

	while (rOnset != rend)
	{
		m_nOnset++;
		phones.push_back(Consonant(rOnset->str()));
	}

	phones.push_back(Vowel(nucleus));

	while (rCoda != rend)
	{
		m_nCoda++;
		phones.push_back(Consonant(rCoda->str()));
	}

	// fill up with zeros
	for (unsigned o = m_nOnset; o < NUMPHONONSET; ++o)
	{
		auto it = phones.begin();
		phones.insert(it, Consonant(""));
	}

	for (unsigned c = m_nCoda; c < NUMPHONCODA; ++c)
	{
		phones.push_back(Consonant(""));
	}

	// words beginning with vowels become glottal onset
	if (m_nOnset == 0 && m_features.positions[1] == 1 && m_features.positions[3] == 1)
	{
		phones[2] = Consonant("?");
	}

	// update some features based on onset and coda count
	m_features.positions[4] = m_nOnset;
	m_features.positions[5] = m_nCoda;

	// check validity
	if (phones.size() != NUMPHON)
	{
		throw dlib::error("[Syllable] Invalid syllable structure detected: " + onset + " " + nucleus + " " + coda);
	}

	// return
	for (Phoneme p : phones)
	{
		features.push_back(p.get_features());
	}
	return features;
}

static void Word::filter_accents (std::string& sampa)
{
	// main accent
	auto positions = sampa.find('\"');
	if (positions!=std::string::npos)
	{
		sampa.erase(positions,1);
	}

	// other accent
	positions = sampa.find('%');
	while (positions!=std::string::npos)
	{
		sampa.erase(positions,1);
		positions = sampa.find('%');
	}

	// also filter backslash because there is no fricative j
	positions = sampa.find('\\');
	if (positions!=std::string::npos)
	{
		sampa.erase(positions,1);
	}
}

void Word::determine_feature_string (std::string sampa, std::string accents)
{
	// compute accents
	filter_accents(sampa);
	accents = "0" + accents + "0";
	std::vector<feat_t> globalAccents;
	for (char a : accents)
	{
		globalAccents.push_back(std::stod(std::string(a))); // ??? - '0'
	}

	// container
	std::vector<Syllable> syllables;

	// split words by vertical line
	unsigned cntWord (0), cntSylAbs (0);
	std::vector<std::string> wordTokens = dlib::split(sampa, "|");;

	// count and tokenize
	for (std::string w : wordTokens)
	{
		cntWord++;

		// split syllables by dot
		std::vector<std::string> syllableTokens = dlib::split(w,".");
		unsigned cntSylRel (syllableTokens.size());
		cntSylAbs += cntSylRel;

		// determine features
		unsigned cntSyl (0);
		for (std::string s : syllableTokens)
		{
			cntSyl++;
			acc_feat_v accFeat ({globalAccents[cntSyl-1], globalAccents[cntSyl], globalAccents[cntSyl+1]});
			pos_feat_v posFeat ({});
		}









		if (cntSylRel == 1) // words with only one syllable
		{
			m_syllable_vec[0].determine_accent_features(0, m_accentPattern[0], 0);
			m_syllable_vec[0].determine_position_features(word_str.size(),cntWord,syllable_str.size(),1,0,0);
		}
		else // longer words
		{
			// first syllable
			m_syllable_vec[0].determine_accent_features(0, m_accentPattern[0], m_accentPattern[1]);
			m_syllable_vec[0].determine_position_features(word_str.size(),cntWord,syllable_str.size(),1,0,m_syllable_vec[1].get_number_phonemes());

			// following syllables
			for (uint16_t i=1; i<m_numberSyllables-1; ++i)
			{
				m_syllable_vec[i].determine_accent_features(m_accentPattern[i-1], m_accentPattern[i], m_accentPattern[i+1]);
				m_syllable_vec[i].determine_position_features(word_str.size(),cntWord,syllable_str.size(),i+1,m_syllable_vec[i-1].get_number_phonemes(),m_syllable_vec[i+1].get_number_phonemes());
			}

			// last syllable
			m_syllable_vec[m_numberSyllables-1].determine_accent_features(m_accentPattern[m_numberSyllables-2], m_accentPattern[m_numberSyllables-1], 0);
			m_syllable_vec[m_numberSyllables-1].determine_position_features(word_str.size(),cntWord,syllable_str.size(),syllable_str.size(),m_syllable_vec[m_numberSyllables-2].get_number_phonemes(),0);
		}
	}

	// validity check
	if (cntSylAbs != m_numberSyllables)
	{
		throw Exception("ERROR: Accent pattern doesn't match SAMPA string!");
	}
}
