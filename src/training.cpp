#include "training.h"
#include <dlib/optimization.h>

MlaTrainer::MlaTrainer (const std::string& trainingFile, const std::string& algorithmFile)
{
	parse_xml(algorithmFile, *this);
	read_samples(trainingFile);
	scale_samples(0,1);
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

    // randomize samples
    //randomize_samples(m_samples, m_targets);
}

MlAlgorithm::MlAlgorithm (const sample_v& samples, const qta_target_v& targets, const algorithm_m& params)
{
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
	return {0};//dlib::cross_validate_regression_trainer(trainer, samples, targets, 10);
}


la_col_vec SupportVectorRegression::model_selection(const sample_v& samples, const std::vector<double>& targets)
{
	// define parameter search space {C,gamma,epsilon}
	la_col_vec lowerBound(3,1), upperBound(3,1);
	lowerBound = 1e-5, 1e-15, 1e-5;
	upperBound = 1e15, 1e3, 1e5;

	// create grid for grid search
	dlib::matrix<double> grid = get_grid(lowerBound, upperBound, 5);

    // loop over grid
	double best_result (1e4);
	la_col_vec best_params;

    for (long col = 0; col < grid.nc(); ++col)
    {
        // do cross validation and then check if the results are the best
    	la_col_vec params;
    	params = grid(0, col),grid(1, col),grid(2, col);
        SvrCvError cvError (samples, targets);
        double result = cvError(params);

        // save the best results
        if (result < best_result)
        {
        	best_params = params;
            best_result = result;
        }
    }

    // optimization with BOBYQA
    lowerBound = log(lowerBound);
    upperBound = log(upperBound);
    best_params = log(best_params);
    double fmin = dlib::find_min_bobyqa(SvrCvError(samples, targets), best_params, best_params.size()*2+1, lowerBound, upperBound, min(lowerBound-upperBound)/10, 1e-3, 1000);

    return exp(best_params);
}

double SvrCvError::operator() (const la_col_vec& arg) const
{
	svr_params params ({arg(0), arg(1), arg(2)});
	svr_trainer_t trainer = SupportVectorRegression::build_trainer(params);
	la_col_vec result = SupportVectorRegression::cross_validation(trainer, m_samples, m_targets);
	return result(0);
}

dlib::matrix<double> SupportVectorRegression::get_grid(const la_col_vec& lowerBound, const la_col_vec& upperBound, const unsigned& numPerDim)
{
	// get one dimension
	la_col_vec cSpace 		= dlib::logspace(log10(upperBound(0)), log10(lowerBound(0)), numPerDim);
	la_col_vec gammaSpace 	= dlib::logspace(log10(upperBound(1)), log10(lowerBound(1)), numPerDim);
	la_col_vec epsilonSpace = dlib::logspace(log10(upperBound(2)), log10(lowerBound(2)), numPerDim);

	// create grid points and return
	dlib::matrix<double> tmpGrid = dlib::cartesian_product(cSpace, gammaSpace);
	return dlib::cartesian_product(tmpGrid, epsilonSpace);

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
	std::cout << "slope:    " << cross_validation(slopeTrainer, m_samples, m_targets.slopes);
	std::cout << "offset:   " << cross_validation(offsetTrainer, m_samples, m_targets.offsets);
	std::cout << "strength: " << cross_validation(strengthTrainer, m_samples, m_targets.strengths);
	std::cout << "duration: " << cross_validation(durationTrainer, m_samples, m_targets.durations);
}

void SupportVectorRegression::model_selection()
{
	// perform model selection
	la_col_vec optSlope = model_selection(m_samples, m_targets.slopes);
	la_col_vec optOffset = model_selection(m_samples, m_targets.offsets);
	la_col_vec optStrength = model_selection(m_samples, m_targets.strengths);
	la_col_vec optDuration = model_selection(m_samples, m_targets.durations);

	// print out results
	std::cout << "slope:\tC=" << optSlope(0) << " gamma=" << optSlope(1) << " epsilon=" << optSlope(2) << std::endl;
	std::cout << "offset:\tC=" << optOffset(0) << " gamma=" << optOffset(1) << " epsilon=" << optOffset(2) << std::endl;
	std::cout << "strength:\tC=" << optStrength(0) << " gamma=" << optStrength(1) << " epsilon=" << optStrength(2) << std::endl;
	std::cout << "duration:\tC=" << optDuration(0) << " gamma=" << optDuration(1) << " epsilon=" << optDuration(2) << std::endl;
}

void SupportVectorRegression::predict()
{

}
