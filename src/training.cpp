#include "training.h"

// *************** MLATRAINER ***************

MlaTrainer::MlaTrainer (const std::string& trainingFile, const std::string& algorithmFile)
{
	parse_xml(algorithmFile, *this);
	read_samples(trainingFile);
}

void MlaTrainer::perform_task (const dlib::command_line_parser& parser)
{
	std::string& alg (m_algorithmParams.at("type"));
	if (alg == "svr")
	{
		SupportVectorRegression svr(m_samples, m_targets, m_algorithmParams);
		svr.perform_task(parser);
	}
	else if (alg == "mlp")
	{
		MultiLayerPerceptron mlp(m_samples, m_targets, m_algorithmParams);
		mlp.perform_task(parser);
	}
	else
	{
		throw dlib::error("[MlaTrainer] wrong algorithm type specified: " + m_algorithmParams.at("type"));
	}
}

void MlaTrainer::start_element (const unsigned long line_number, const std::string& name,const dlib::attribute_list& atts)
{
	while (atts.move_next())
	{
		m_algorithmParams.insert(std::pair<std::string, std::string>(atts.element().key(), atts.element().value()));
	}
}

void MlaTrainer::read_samples (const std::string& sampleFile)
{
	// create a file-reading object for sample-file
	std::ifstream fin;
	fin.open(sampleFile); // open input file
	if (!fin.good())
	{
		throw dlib::error("[read_samples] sample file not found!");
	}

	// container
	std::string line;
	std::vector<std::string> tokens;

	// read sample file
	std::getline(fin, line); // ingnore first line (header)
	while (std::getline(fin, line))
	{
		sample_t currentSample;
		pitch_target_s currentTarget;
		tokens = dlib::split(line, ",");

		try
		{
			currentTarget.label = tokens[0];
			currentTarget.m = std::stod(tokens[1]);
			currentTarget.b = std::stod(tokens[2]);
			currentTarget.l = std::stod(tokens[3]);
			currentTarget.d = std::stod(tokens[4]);

			for (unsigned i=5; i<tokens.size(); ++i)
			{
				currentSample(i-5) = std::stod(tokens[i]);
			}
		}
		catch (std::invalid_argument& e)
		{
			throw dlib::error("[read_samples] invalid argument exceptions occurred while using std::stod!\n" + std::string(e.what()));
		}

		m_samples.push_back(currentSample);
		m_targets.push_back(currentTarget);
	}
}

// *************** DATASCALER ***************

scaler_s DataScaler::min_max_scale(std::vector<double>& data) const
{
	// running statistics
	dlib::running_stats<double> stat;
	for (double& d : data)
	{
		stat.add(d);
	}
	scaler_s scale;
	scale.min = stat.min();
	scale.max = stat.max();

	// min-max scaling
	for (double& d : data)
	{
		double range (stat.max()-stat.min());
		if (range > 0)
		{
			d = m_lower + (m_upper-m_lower) * ((d-scale.min)/range);
		}
		else // all features have same value
		{
			d = m_lower;
		}

	}

	return scale;
}

void DataScaler::min_max_rescale(std::vector<double>& data, const scaler_s& scale) const
{
	// min-max rescaling
	for (double& d : data)
	{
		d = ((d - m_lower)/(m_upper-m_lower))*(scale.max-scale.min) + scale.min;
	}
}

void DataScaler::min_max_scale(sample_v& samples)
{
	std::vector<dlib::running_stats<double>> stats (NUMFEATSYL);

	// get min and max values per feature (global)
	for (sample_t& s : samples)
	{
		for (unsigned i=0; i<NUMFEATSYL; ++i)
		{
			stats[i].add(s(i));
		}
	}

	// scale features
	for (sample_t& s : samples)
	{
		for (unsigned i=0; i<NUMFEATSYL; ++i)
		{
			double range (stats[i].max()-stats[i].min());
			if (range > 0)
			{
				s(i) = m_lower + (m_upper-m_lower) * ((s(i)-stats[i].min())/range);// scale value
			}
			else // all features have same value
			{
				s(i) = m_lower;
			}
		}
	}
}

void DataScaler::min_max_scale(training_s& trainingData)
{
	min_max_scale(trainingData.samples);
	m_slopeScale = min_max_scale(trainingData.slopes);
	m_offsetScale = min_max_scale(trainingData.offsets);
	m_strengthScale = min_max_scale(trainingData.strengths);
	m_durationScale = min_max_scale(trainingData.durations);
}

void DataScaler::min_max_rescale(training_s& trainingData)
{
	min_max_rescale(trainingData.slopes, m_slopeScale);
	min_max_rescale(trainingData.offsets, m_offsetScale);
	min_max_rescale(trainingData.strengths, m_strengthScale);
	min_max_rescale(trainingData.durations, m_durationScale);
}

// *************** MLALGORITHM ***************

MlAlgorithm::MlAlgorithm (const sample_v& samples, const target_v& targets, const algorithm_m& params) : m_scaler(FEATLOW,FEATUP)
{
	// get data
	m_params = params;
	m_targets = targets;
	m_data.samples = samples;

	// randomize data
	randomize_data(m_targets, m_data.samples);

	// save data to training format
	for (pitch_target_s t : targets)
	{
		m_data.slopes.push_back(t.m);
		m_data.offsets.push_back(t.b);
		m_data.strengths.push_back(t.l);
		m_data.durations.push_back(t.d);
	}

	// normalize training data
	//m_scaler.min_max_scale(m_data.samples);
	m_scaler.min_max_scale(m_data);
}

double MlAlgorithm::get_value(const std::string& param) const
{
	double value (0.0);
	try
	{
		value = std::stod(m_params.at(param));
	}
	catch (std::out_of_range& e)
	{
		throw dlib::error("[get_value] the following parameter was not defined: " + param + "\n" + std::string(e.what()));
	}
	catch (std::invalid_argument& e)
	{
		throw dlib::error("[get_value] invalid argument exceptions occurred while using std::stod!\n" + std::string(e.what()));
	}

	return value;
}

void MlAlgorithm::randomize_data (target_v& targets, sample_v& samples)
{
	// initialize random seed
	srand (time(NULL));

	unsigned N (samples.size());
	for (unsigned i=0; i<N; ++i)
	{
		unsigned randPos (rand()%(N-1)+1);
		std::string label (targets[randPos].label);

		// get start position of word
		unsigned start (randPos);
		while (targets[start-1].label == label)
		{
			if (start-1 < 0)
				break;
			--start;
		}

		// get end position of word
		unsigned end (randPos);
		while (targets[end+1].label == label)
		{
			if (end+1 >= N)
				break;
			++end;
		}

		// shift random word to end
		for (unsigned pos=start; pos<=end; ++pos)
		{
			targets.push_back(targets[pos]);
			samples.push_back(samples[pos]);
		}

		// delete random word
		targets.erase(targets.begin()+start, targets.begin()+end+1);
		samples.erase(samples.begin()+start, samples.begin()+end+1);
	}
}

// loss = squared distance
double MlAlgorithm::loss (const double& x, const double& y)
{
	return (x-y)*(x-y);
}

void MlAlgorithm::perform_task(const dlib::command_line_parser& parser)
{
	if (parser.option("t"))
	{
		train();
	}
	else if (parser.option("p"))
	{
		predict();
	}
	else if (parser.option("c"))
	{
		cross_validation();
	}
	else if (parser.option("m"))
	{
		model_selection();
		write_algorithm_file(parser.option("alg").argument());
	}
	else
	{
		throw dlib::error("[MlAlgorithm] Error in command line:\n   Wrong task specifier!");
	}
}

void MlAlgorithm::write_algorithm_file(const std::string& algFile) const
{
	// create output file and write results to it
	std::ofstream fout;
	fout.open (algFile);
	fout << "<algorithm" << std::endl;

	for (auto p : m_params)
	{
		fout << "\t" << p.first << "=\"" << p.second << "\"" << std::endl;
	}

	fout << "></algorithm>";
	fout.close();
}

