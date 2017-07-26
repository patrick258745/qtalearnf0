#include <regex>
#include <sstream>
#include "dlib/string.h"
#include "sampling.h"

void Consonant::determine_features (std::string sampa)
{
	// collect information
	bool unvoiced (false);

	// compute unvoiced consonant modifier
	std::size_t position = sampa.find("_0");
	if (position != std::string::npos)
	{
		if ( (position != 0) && (sampa.length() <= 5) && (sampa[position+2] == '\0') )
		{
			sampa.erase(position,2);
			unvoiced = true;
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
		if (unvoiced)
		{
			m_features[0] = 0;
		}
	}
	catch (std::exception& e)
	{
		throw dlib::error("[Consonant] Invalid or unused SAMPA consonant symbol found: " + sampa);
	}
}

const std::map<std::string, phon_feat_v> Consonant::CONSONANTS{
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

const std::map<std::string, phon_feat_v> Vowel::VOWELS{
		{"" , { 0, 0, 0, 0, 0} },
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

Syllable::Syllable (const std::string& sampa, const pos_feat_v& posFeat, const acc_feat_v& accFeat)
{
	m_features.accents = accFeat;
	m_features.positions = posFeat;
	m_nOnset = 0; m_nCoda = 0; m_nNucleus = 1;
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

	// check for empty syllable
	if (sampa.empty())
	{
		sampa = "@";
		m_nNucleus = 0;
	}

	// determine onset, nucleus and coda by regex
	std::regex vowels("[aAoeEiIOuUyY269@=](:|~|_\\^){0,3}[aAoeEiIOuUy269@=]?(:|~|_\\^){0,3}[aAoeEiIOuUy269@=]?(:|~|_\\^){0,3}");
	std::regex consonants("((\\?|pf|ps|ts|tS|pS|dZ|p|b|t|d|k|g|f|v|s|z|S|Z|T|D|C|x|h|m|n|N|l|j|R|r)(_0)?)?");

	// first: find nucleus, onset and coda
	std::vector<std::string> tokens;
	std::string nucleus, onset, coda;
	std::smatch m;
	std::regex_search (sampa, m, vowels);
	nucleus = m[0];
	tokens = dlib::split(sampa,nucleus);
	std::size_t pos = sampa.find(nucleus);

	if (tokens.size() > 0)
	{
		if (tokens.size() > 1)
		{
			onset = tokens[0];
			coda = tokens[1];
		}
		else if (pos == 0)
		{
			coda = tokens[0];
		}
		else
		{
			onset = tokens[0];
		}
	}

	// DEBUG
	// std::cout << onset << " " << nucleus << " " << coda << std::endl;

	// determine phonemes
	std::regex_iterator<std::string::iterator> rend;
	std::regex_iterator<std::string::iterator> rOnset ( onset.begin(), onset.end(), consonants );
	std::regex_iterator<std::string::iterator> rCoda ( coda.begin(), coda.end(), consonants );

	while (rOnset != rend && !rOnset->str().empty())
	{
		m_nOnset++;
		phones.push_back(Consonant(rOnset->str()));
		rOnset++;
	}

	phones.push_back(Vowel(nucleus));

	while (rCoda != rend && !rCoda->str().empty())
	{
		m_nCoda++;
		phones.push_back(Consonant(rCoda->str()));
		rCoda++;
	}

	// fill up with zeros
	unsigned tmpCnt (m_nOnset);
	while (tmpCnt != NUMPHONONSET)
	{
		tmpCnt++;
		auto it = phones.begin();
		phones.insert(it, Consonant(""));
	}

	tmpCnt = m_nCoda;
	while (tmpCnt != NUMPHONCODA)
	{
		tmpCnt++;
		phones.push_back(Consonant(""));
	}

	// words beginning with vowels become glottal onset
	if (m_nOnset == 0 && m_features.positions[1] == 1 && m_features.positions[3] == 1)
	{
		phones[2] = Consonant("?");
	}

	// update some features based on onset and coda count
	m_features.positions.push_back(m_nOnset);
	m_features.positions.push_back(m_nCoda);

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

void Word::filter_accents (std::string& sampa)
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
	std::vector<int> globalAccents;
	for (char a : accents)
	{
		globalAccents.push_back(a - '0'); // ??? - '0'
	}

	// container
	std::vector<Syllable> syllables, tmp;

	// split words by vertical line
	unsigned cntWord (0), cntSylAbs (0);
	std::vector<std::string> wordTokens = dlib::split(sampa, "|");;

	// count and tokenize
	for (std::string w : wordTokens)
	{
		cntWord++;

		// split syllables by dot
		std::vector<std::string> syllableTokens = dlib::split(w,".");
		cntSylAbs += syllableTokens.size();

		// determine tmp for number phonemes of syllable and also next and prev
		tmp.push_back(Syllable("",{0,0,0},{0,0,0,0,0,0,0,0}));
		for (std::string s : syllableTokens)
		{
			tmp.push_back(Syllable(s,{0,0,0},{0,0,0,0,0,0,0,0}));
		}
		tmp.push_back(Syllable("",{0,0,0},{0,0,0,0,0,0,0,0}));

		// determine syllables
		for (unsigned i=0; i< syllableTokens.size(); ++i)
		{
			acc_feat_v accFeat ({(feat_t)globalAccents[i], (feat_t)globalAccents[i+1], (feat_t)globalAccents[i+2]});
			pos_feat_v posFeat ({(feat_t)wordTokens.size(), (feat_t)cntWord, (feat_t)syllableTokens.size(), (feat_t)i+1, (feat_t)tmp[i].get_number_phonemes(), (feat_t)tmp[i+2].get_number_phonemes()}); // numPhonprev, next
			Syllable syl (syllableTokens[i],posFeat,accFeat);
			syllables.push_back(syl);

			// store features to string stream
			std::ostringstream sout;
			sout << std::setprecision(1);
			for (phon_feat_v pv : syl.get_features().phonemes)
			{
				for (feat_t f : pv)
				{
					sout << f << ",";
				}
			}
			for (feat_t f : syl.get_features().accents)
			{
				sout << f << ",";
			}
			for (feat_t f : syl.get_features().positions)
			{
				sout << f << ",";
			}
			m_feature_string.push_back(sout.str());
		}
	}
}

DataAssembler::DataAssembler (const std::string& sampaFile, const std::string& targetFile)
{
	read_input_files(sampaFile, targetFile);
}

void DataAssembler::read_input_files(const std::string& sampaFile, const std::string& targetFile)
{
	// create a file-reading object for sampa-file
	std::ifstream finS;
	finS.open(sampaFile); // open input file
	if (!finS.good())
	{
		throw dlib::error("[read_input_files] sampa file not found!");
	}

	// create a file-reading object for target-file
	std::ifstream finT;
	finT.open(targetFile); // open input file
	if (!finT.good())
	{
		throw dlib::error("[read_input_files] target file not found!");
	}

	// container
	std::string line;
	std::vector<std::string> tokens;

	// read sampa file
	std::getline(finS, line); // ingnore first line (header)
	while (std::getline(finS, line))
	{
		tokens = dlib::split(line, ",");
		m_featMap.insert(std::pair<std::string,Word>(tokens[0], Word(tokens[1], tokens[2])));
	}

	// read targets to map
	std::string label;
	std::vector<std::string> targets;
	std::getline(finT, line); // ingnore first line (header)
	while (std::getline(finT, line))
	{
		tokens = dlib::split(line, ",");
		if (label != tokens[0])
		{
			if (!label.empty())
			{
				// store
				m_targetMap.insert(std::pair<std::string,std::vector<std::string>>(label,targets));
				targets.clear();
			}

			// new word
			label = tokens[0];
			std::ostringstream starget;
			starget << tokens[1] << "," << tokens[2] << "," << tokens[3] << "," << tokens[4];
			targets.push_back(starget.str());
		}
		else
		{
			std::ostringstream starget;
			starget << tokens[1] << "," << tokens[2] << "," << tokens[3] << "," << tokens[4];
			targets.push_back(starget.str());
		}
	}
	// store last
	m_targetMap.insert(std::pair<std::string,std::vector<std::string>>(label,targets));

	// check number of words
	if (m_targetMap.size() == 0 || m_featMap.size() == 0)
	{
		throw dlib::error("[read_input_files] There is an empty or wrong formated input file!");
	}
}

void DataAssembler::write_to_file(const std::string& sampleFile)
{
	// create output file and write results to it
	std::ofstream fout;
	fout.open (sampleFile);

	// write header to outputfile
	fout << "word,m,b,l,d,S1,N1,P1,F1,G1,L1,O1,S2,N2,P2,F2,G2,L2,O2,S3,N3,P3,F3,G3,L3,O3,ZP,ZH,LR,LL,NL,S4,N4,P4,F4,G4,L4,O4,S5,N5,P5,F5,G5,L5,O5,S6,N6,P6,F6,G6,L6,O6,S7,N7,P7,F7,G7,L7,O7,AA,AV,AF,WA,WN,AS,NS,PV,PF,PO,PC" << std::endl;

	// write data to output file
	for (auto i : m_featMap)
	{
		if (m_targetMap.find(i.first) != m_targetMap.end())
		{
			std::vector<std::string> t (m_targetMap.at(i.first));
			std::vector<std::string> f (i.second.get_feature_string_vector());

			// check number of equal syllables
			if (f.size() != t.size())
			{
				throw dlib::error("[write_to_file] Different number os syllables in sampa and target file!");
			}

			for (unsigned j=0; j<t.size(); ++j)
			{
				fout << i.first << "," << t[j] << "," << f[j].substr(0,f[j].size()-1) << std::endl;
			}

		}
	}

	fout.close();
}
