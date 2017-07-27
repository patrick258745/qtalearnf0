#include "training.h"
#include <dlib/threads.h>
#include <dlib/optimization.h>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

MlaTrainer::MlaTrainer (const std::string& trainingFile, const std::string& algorithmFile)
{
	parse_xml(algorithmFile, *this);
	read_samples(trainingFile);
	scale_samples(0,1);
}

void MlaTrainer::perform_task (const dlib::command_line_parser& parser)
{
	if (!parser.option("p"))
	{
		randomize_data();
	}

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
		qtaTarget_s currentTarget;
		tokens = dlib::split(line, ",");

		try
		{
			currentTarget.label = tokens[0];
			currentTarget.m = std::stod(tokens[1]);
			currentTarget.b = std::stod(tokens[2]);
			currentTarget.l = std::stod(tokens[3]);
			currentTarget.d = std::stod(tokens[4]);
			currentTarget.r = 0.0;
			currentTarget.c = 0.0;

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

void MlaTrainer::scale_samples (const double& lower, const double& upper)
{
	std::vector<double> minVal (NUMFEATSYL, 1e3);
	std::vector<double> maxVal (NUMFEATSYL, -1e3);

	// get min and max values per feature (global)
	for (sample_t s : m_samples)
	{
		for (unsigned i=0; i<NUMFEATSYL; ++i)
		{
			minVal[i] = std::min(minVal[i], s(i));
			maxVal[i] = std::max(maxVal[i], s(i));
		}
	}

	// scale features
	for (sample_t& s : m_samples)
	{
		for (unsigned i=0; i<NUMFEATSYL; ++i)
		{
			// scaling
			double diff = maxVal[i] - minVal[i];
			if (diff == 0)
			{
				s(i) = lower;
			}
			else if (diff > 0)
			{
				s(i) = lower + (upper-lower) * (s(i)-minVal[i])/(diff);// scale value
			}
			else
			{
				throw dlib::error("[read_samples] Wrong scaling! max<min!");
			}
		}
	}
}

void MlaTrainer::normalize_samples ()
{
    dlib::vector_normalizer<sample_t> normalizer;
    // Let the normalizer learn the mean and standard deviation of the samples.
    normalizer.train(m_samples);
    // now normalize each sample
    for (unsigned long i = 0; i < m_samples.size(); ++i)
        m_samples[i] = normalizer(m_samples[i]);
}

void MlaTrainer::randomize_data ()
{
	// initialize random seed
	srand (time(NULL));

	unsigned N (m_samples.size());
	for (unsigned i=0; i<N; ++i)
	{
		unsigned randPos (rand()%(N-1)+1);
		std::string label (m_targets[randPos].label);

		// get start position of word
		unsigned start (randPos);
		while ((start-1) >= 0 && m_targets[start-1].label == label)
		{
			--start;
		}

		// get end position of word
		unsigned end (randPos);
		while ((end+1) < N && m_targets[end+1].label == label)
		{
			++end;
		}

		// shift random word to end
		for (unsigned pos=start; pos<=end; ++pos)
		{
			m_targets.push_back(m_targets[pos]);
			m_samples.push_back(m_samples[pos]);
		}

		// delete random word
		for (unsigned pos=end; pos>=start; --pos)
		{
			m_targets.erase(m_targets.begin()+pos);
			m_samples.erase(m_samples.begin()+pos);
		}
	}
}

MlAlgorithm::MlAlgorithm (const sample_v& samples, const qta_target_v& targets, const algorithm_m& params)
{
	m_targets_orig = targets;
	m_samples = samples;
	m_params = params;

	for (qtaTarget_s t : targets)
	{
		m_targets.slopes.push_back(t.m);
		m_targets.offsets.push_back(t.b);
		m_targets.strengths.push_back(t.l);
		m_targets.durations.push_back(t.d);
	}
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
	}
	else
	{
		throw dlib::error("[MlAlgorithm] Error in command line:\n   Wrong task specifier!");
	}
}

double MlAlgorithm::get_value(std::string param)
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

double SvrCvError::operator() (const la_col_vec& logArg) const
{
	la_col_vec arg = exp(logArg);
	svr_params params ({arg(0), arg(1), arg(2)});
	svr_trainer_t trainer = SupportVectorRegression::build_trainer(params);
	la_col_vec result = SupportVectorRegression::cross_validation(trainer, m_samples, m_targets);

	return result(0);
}

dlib::matrix<double> SupportVectorRegression::get_grid(const la_col_vec& lowerBound, const la_col_vec& upperBound, const unsigned& numPerDim)
{
	// get one dimension
	dlib::matrix<double> cSpace 		= dlib::logspace(log10(upperBound(0)), log10(lowerBound(0)), numPerDim);
	dlib::matrix<double> gammaSpace 	= dlib::logspace(log10(upperBound(1)), log10(lowerBound(1)), numPerDim);
	dlib::matrix<double> epsilonSpace 	= dlib::logspace(log10(upperBound(2)), log10(lowerBound(2)), numPerDim);

	// create grid points and return
	dlib::matrix<double> tmpGrid = dlib::cartesian_product(cSpace, gammaSpace);
	return dlib::cartesian_product(tmpGrid, epsilonSpace);
}

svr_trainer_t SupportVectorRegression::build_trainer(const svr_params& params)
{
	// create a trainer
	svr_trainer_t trainer;

	// setup parameters for trainer
	trainer.set_c(params.C);
	trainer.set_kernel(kernel_t(params.gamma));
	trainer.set_epsilon_insensitivity(params.epsilon);

	// return trainer
	return trainer;
}

svr_model_t SupportVectorRegression::train(const svr_trainer_t& trainer, const sample_v& samples, const std::vector<double>& targets)
{
	return trainer.train(samples, targets);
}

la_col_vec SupportVectorRegression::cross_validation(const svr_trainer_t& trainer, const sample_v& samples, const std::vector<double>& targets)
{
	dlib::matrix<double,1,2> result = dlib::cross_validate_regression_trainer(trainer, samples, targets, 10);
	return dlib::trans(result);
}

la_col_vec SupportVectorRegression::model_selection(const sample_v& samples, const std::vector<double>& targets)
{
	// define parameter search space {C,gamma,epsilon}
	la_col_vec lowerBound(3), upperBound(3);
	lowerBound = 1e-2, 1e-5, 1e-4;
	upperBound = 1e3, 1e1, 1e0;

	// store optimal parameters
	svr_params bestParams;
	double bestResult (1e6);

	// create grid for grid search
	dlib::matrix<double> grid = get_grid(lowerBound, upperBound, 5);

	// loop over grid
	dlib::mutex mu;
	dlib::parallel_for(0, grid.nc(), [&](long col) //for(long col = 0; col < grid.nc(); ++col)
	{
        // do cross validation and then check if the results are the best
		svr_params params ({grid(0, col),grid(1, col),grid(2, col)});
    	svr_trainer_t trainer = SupportVectorRegression::build_trainer(params);
    	la_col_vec tmp = SupportVectorRegression::cross_validation(trainer, samples, targets);
        double result = tmp(0);

        // save the best results
        dlib::auto_mutex lock(mu);
        if (result < bestResult)
        {
        	bestParams = params;
            bestResult = result;
        }
    	//DEBUG
    	std::cout << col << ": cverror(" << params.C << "," << params.gamma << "," << params.epsilon << " = " << result << std::endl;
    }
	);

    // optimization with BOBYQA
	la_col_vec arg(3);
	arg = bestParams.C,bestParams.gamma,bestParams.epsilon;

	// log scale
    lowerBound = log(lowerBound);
    upperBound = log(upperBound);
    la_col_vec logArg = log(arg);

    try
    {
    	// optimization
    	dlib::find_min_bobyqa(SvrCvError (samples, targets), logArg, arg.size()*2+1, lowerBound, upperBound, min(upperBound-lowerBound)/10, 1e-2, 100);
    }
	catch (dlib::bobyqa_failure& err)
	{
		// DEBUG message
		// std::cerr << "WARNING: no convergence during optimization in iteration: " << it << std::endl << err.info << std::endl;
	}

    return exp(logArg);
}

std::vector<double> SupportVectorRegression::predict(const svr_trainer_t& trainer, const sample_v& samplesTrain, const std::vector<double>& targetsTrain, const sample_v& samplesTest)
{
	std::vector<double> predictedTargets;

	svr_model_t regFunction = train(trainer, samplesTrain, targetsTrain);
	for (sample_t s : samplesTest)
	{
		predictedTargets.push_back(regFunction(s));
	}

	return predictedTargets;
}

void SupportVectorRegression::train()
{
	// get trainer depending on parameters
	svr_trainer_t slopeTrainer = build_trainer({get_value("Mpenalty"), get_value("Mgamma"), get_value("Mepsilon")});
	svr_trainer_t offsetTrainer = build_trainer({get_value("Bpenalty"), get_value("Bgamma"), get_value("Bepsilon")});
	svr_trainer_t strengthTrainer = build_trainer({get_value("Lpenalty"), get_value("Lgamma"), get_value("Lepsilon")});
	svr_trainer_t durationTrainer = build_trainer({get_value("Dpenalty"), get_value("Dgamma"), get_value("Depsilon")});

	// do the training and save the models to files
	dlib::serialize(m_params.at("Mmodel")) << train(slopeTrainer, m_samples, m_targets.slopes);
	dlib::serialize(m_params.at("Bmodel")) << train(offsetTrainer, m_samples, m_targets.offsets);
	dlib::serialize(m_params.at("Lmodel")) << train(strengthTrainer, m_samples, m_targets.strengths);
	dlib::serialize(m_params.at("Dmodel")) << train(durationTrainer, m_samples, m_targets.durations);
}

void SupportVectorRegression::cross_validation()
{
	// get trainer depending on parameters
	svr_trainer_t slopeTrainer = build_trainer({get_value("Mpenalty"), get_value("Mgamma"), get_value("Mepsilon")});
	svr_trainer_t offsetTrainer = build_trainer({get_value("Bpenalty"), get_value("Bgamma"), get_value("Bepsilon")});
	svr_trainer_t strengthTrainer = build_trainer({get_value("Lpenalty"), get_value("Lgamma"), get_value("Lepsilon")});
	svr_trainer_t durationTrainer = build_trainer({get_value("Dpenalty"), get_value("Dgamma"), get_value("Depsilon")});

	// do cross validation and print results
	std::cout << "slope:    " << dlib::trans(cross_validation(slopeTrainer, m_samples, m_targets.slopes));
	std::cout << "offset:   " << dlib::trans(cross_validation(offsetTrainer, m_samples, m_targets.offsets));
	std::cout << "strength: " << dlib::trans(cross_validation(strengthTrainer, m_samples, m_targets.strengths));
	std::cout << "duration: " << dlib::trans(cross_validation(durationTrainer, m_samples, m_targets.durations));
}

void SupportVectorRegression::model_selection()
{
	// perform model selection
	la_col_vec optSlope = model_selection(m_samples, m_targets.slopes);
	la_col_vec optOffset = model_selection(m_samples, m_targets.offsets);
	la_col_vec optStrength = model_selection(m_samples, m_targets.strengths);
	la_col_vec optDuration = model_selection(m_samples, m_targets.durations);

	// print out results
	std::cout << "slope:\t\tC=" << optSlope(0) << "\tgamma=" << optSlope(1) << "\tepsilon=" << optSlope(2) << std::endl;
	std::cout << "offset:\t\tC=" << optOffset(0) << "\tgamma=" << optOffset(1) << "\tepsilon=" << optOffset(2) << std::endl;
	std::cout << "strength:\tC=" << optStrength(0) << "\tgamma=" << optStrength(1) << "\tepsilon=" << optStrength(2) << std::endl;
	std::cout << "duration:\tC=" << optDuration(0) << "\tgamma=" << optDuration(1) << "\tepsilon=" << optDuration(2) << std::endl;
}

void SupportVectorRegression::predict()
{
	// initialize
	unsigned N (m_samples.size());

	// result container
	qta_target_v predictedTargets;

	// get trainer depending on parameters
	svr_trainer_t slopeTrainer = build_trainer({get_value("Mpenalty"), get_value("Mgamma"), get_value("Mepsilon")});
	svr_trainer_t offsetTrainer = build_trainer({get_value("Bpenalty"), get_value("Bgamma"), get_value("Bepsilon")});
	svr_trainer_t strengthTrainer = build_trainer({get_value("Lpenalty"), get_value("Lgamma"), get_value("Lepsilon")});
	svr_trainer_t durationTrainer = build_trainer({get_value("Dpenalty"), get_value("Dgamma"), get_value("Depsilon")});

	// create output file and write results to it
	std::ofstream fout;
	fout.open (m_params.at("output"));
	fout << std::fixed << std::setprecision(6);
	fout << "name,slope,offset,strength,duration,rmse,corr" << std::endl;

	// do prediction
	for (unsigned n=0; n<N; ++n)
	{
		sample_v samplesTest;
		std::string label (m_targets_orig[n].label);

		// add to test data
		samplesTest.push_back(m_samples[n]);

		// delete from training data
		sample_v samplesTraining (m_samples);
		samplesTraining.erase(samplesTraining.begin()+n);

		training_target_s targetsTraining (m_targets);
		targetsTraining.slopes.erase(targetsTraining.slopes.begin()+n);
		targetsTraining.offsets.erase(targetsTraining.offsets.begin()+n);
		targetsTraining.strengths.erase(targetsTraining.strengths.begin()+n);
		targetsTraining.durations.erase(targetsTraining.durations.begin()+n);

		training_target_s tmp;
		tmp.slopes = predict(slopeTrainer, samplesTraining, targetsTraining.slopes, samplesTest);
		tmp.offsets = predict(offsetTrainer, samplesTraining, targetsTraining.offsets, samplesTest);
		tmp.strengths = predict(strengthTrainer, samplesTraining, targetsTraining.strengths, samplesTest);
		tmp.durations = predict(durationTrainer, samplesTraining, targetsTraining.durations, samplesTest);

		// store result
		qtaTarget_s t = {label, tmp.slopes[0], tmp.offsets[0], tmp.strengths[0], tmp.durations[0], 0, 0};
		fout << label << "," << tmp.slopes[0] << "," << tmp.offsets[0] << "," << tmp.strengths[0] << "," << tmp.durations[0] << "," << 0.0 << "," << 0.0 << std::endl;
	}

}
