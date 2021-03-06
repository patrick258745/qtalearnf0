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
	if (alg == "lrr")
	{
		LinearRidgeRegression lrr(m_samples, m_targets, m_algorithmParams);
		lrr.perform_task(parser);
	}
	else if (alg == "krr")
	{
		KernelRidgeRegression krr(m_samples, m_targets, m_algorithmParams);
		krr.perform_task(parser);
	}
	else if (alg == "svr")
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

void DataScaler::min_max_rescale(double& data, const scaler_s& scale) const
{
	data = ((data - m_lower)/(m_upper-m_lower))*(scale.max-scale.min) + scale.min;
}

void DataScaler::min_max_rescale(target_v& data)
{
	for (pitch_target_s& t : data)
	{
		min_max_rescale(t.m, m_slopeScale);
		min_max_rescale(t.b, m_offsetScale);
		min_max_rescale(t.l, m_strengthScale);
		min_max_rescale(t.d, m_durationScale);
	}
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
	for (pitch_target_s t : m_targets)
	{
		m_data.labels.push_back(t.label);
		m_data.slopes.push_back(t.m);
		m_data.offsets.push_back(t.b);
		m_data.strengths.push_back(t.l);
		m_data.durations.push_back(t.d);
	}

	// normalize training data
	m_scaler.min_max_scale(m_data);
}

void MlAlgorithm::push_back_targets(target_v& targetsPredicted, training_s& dataTest)
{
	// convert training_s into target_v
	for (unsigned n=0; n<dataTest.labels.size(); ++n)
	{
		pitch_target_s t;
		t.label = dataTest.labels[n];
		t.m = dataTest.slopes[n];
		t.b = dataTest.offsets[n];
		t.l = dataTest.strengths[n];
		t.d = dataTest.durations[n];
		targetsPredicted.push_back(t);
	}
}

void MlAlgorithm::save_target_file(const target_v& targetsPredicted, const std::string& targetFile)
{
	// create output file and write results to it
	std::ofstream fout;
	fout.open (targetFile);
	fout << std::fixed << std::setprecision(6);
	fout << "name,slope,offset,strength,duration" << std::endl;
	for (pitch_target_s t : targetsPredicted)
	{
		fout << t.label << "," << t.m << "," << t.b << "," << t.l << "," << t.d << std::endl;
	}
	fout.close();
}

void MlAlgorithm::split_data(const training_s& data, training_s& dataTraining, training_s& dataTest, const unsigned& start, const unsigned& end)
{
	// initialize
	dataTraining = data;
	dataTest.samples.clear();
	dataTest.slopes.clear();
	dataTest.strengths.clear();
	dataTest.offsets.clear();
	dataTest.durations.clear();
	dataTest.labels.clear();

	// get data for test
	dataTest.labels.insert (dataTest.labels.begin(),dataTraining.labels.begin()+start, dataTraining.labels.begin()+end);
	dataTest.samples.insert (dataTest.samples.begin(),dataTraining.samples.begin()+start, dataTraining.samples.begin()+end);
	dataTest.slopes.insert (dataTest.slopes.begin(),dataTraining.slopes.begin()+start, dataTraining.slopes.begin()+end);
	dataTest.offsets.insert (dataTest.offsets.begin(),dataTraining.offsets.begin()+start, dataTraining.offsets.begin()+end);
	dataTest.strengths.insert (dataTest.strengths.begin(),dataTraining.strengths.begin()+start, dataTraining.strengths.begin()+end);
	dataTest.durations.insert (dataTest.durations.begin(),dataTraining.durations.begin()+start, dataTraining.durations.begin()+end);

	// delete from training data
	dataTraining.labels.erase(dataTraining.labels.begin()+start, dataTraining.labels.begin()+end);
	dataTraining.samples.erase(dataTraining.samples.begin()+start, dataTraining.samples.begin()+end);
	dataTraining.slopes.erase(dataTraining.slopes.begin()+start, dataTraining.slopes.begin()+end);
	dataTraining.offsets.erase(dataTraining.offsets.begin()+start, dataTraining.offsets.begin()+end);
	dataTraining.strengths.erase(dataTraining.strengths.begin()+start, dataTraining.strengths.begin()+end);
	dataTraining.durations.erase(dataTraining.durations.begin()+start, dataTraining.durations.begin()+end);
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
	for (unsigned i=0; i<10*N; ++i)
	{
		int randPos (rand()%(N-1)+1);
		std::string label (targets[randPos].label);

		// get start position of word
		int start (randPos);
		while (targets[start-1].label == label)
		{
			--start;
			if (start-1 <= 0)
				break;
		}

		// get end position of word
		int end (randPos);
		while (targets[end+1].label == label)
		{
			++end;
			if (end+1 >= N)
				break;
		}

		// shift random word to end
		for (int pos=start; pos<=end; ++pos)
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
	m_folds = get_option(parser,"K",5);

	if (parser.option("t"))
	{
		train();
	}
	else if (parser.option("p"))
	{
		predict();
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

