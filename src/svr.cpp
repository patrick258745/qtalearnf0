#include "training.h"

// *************** SVRERROR ***************

double SvrCvError::operator() (const la_col_vec& logArg) const
{
	la_col_vec arg = exp(logArg);
	svr_params params ({arg(0), arg(1), arg(2)});
	svr_trainer_t trainer = SupportVectorRegression::build_trainer(params);
	la_col_vec result = SupportVectorRegression::cross_validation(trainer, m_samples, m_targets);

	return result(0);
}

// *************** SUPPORTVECTORREGRESSION ***************

dlib::matrix<double> SupportVectorRegression::get_grid(const la_col_vec& lowerBound, const la_col_vec& upperBound, const unsigned& numPerDim)
{
	// get one dimension
	dlib::matrix<double> cSpace 		= dlib::logspace(log10(upperBound(0)), log10(lowerBound(0)), numPerDim);
	dlib::matrix<double> gammaSpace 	= dlib::logspace(log10(upperBound(1)), log10(lowerBound(1)), numPerDim);
	dlib::matrix<double> epsilonSpace 	= dlib::logspace(log10(upperBound(2)), log10(lowerBound(2)), 3);

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
	trainer.set_kernel(svr_kernel_t(params.gamma));
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
	lowerBound = 1e-5, 1e-5, 1e-4;
	upperBound = 1e5, 1e5, 1e-1;

	// store optimal parameters
	svr_params bestParams;
	double bestResult (1e6);

	// create grid for grid search
	dlib::matrix<double> grid = get_grid(lowerBound, upperBound, 15);

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

		// DEBUG message
		#ifdef DEBUG_MSG
        std::cout << "\t[model_selection] " << col << ": mse(" << params.C << "," << params.gamma << "," << params.epsilon << ")\t\t= " << result << std::endl;
		#endif
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
		// DEBUG message
		#ifdef DEBUG_MSG
		std::cout << "\t[model_selection] Grid search completed. Initialize BOBYQA ... start(" << dlib::trans(arg) << ")"  << std::endl;
		#endif

    	// optimization
    	dlib::find_min_bobyqa(SvrCvError (samples, targets), logArg, arg.size()*2+1, lowerBound, upperBound, min(upperBound-lowerBound)/10, 1e-2, 100);
    }
	catch (dlib::bobyqa_failure& err)
	{
		// DEBUG message
		#ifdef DEBUG_MSG
		std::cout << "\t[model_selection] WARNING: no convergence during optimization" << std::endl << err.info << std::endl;
		#endif
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
	dlib::serialize(m_params.at("Mmodel")) << train(slopeTrainer, m_data.samples, m_data.slopes);
	dlib::serialize(m_params.at("Bmodel")) << train(offsetTrainer, m_data.samples, m_data.offsets);
	dlib::serialize(m_params.at("Lmodel")) << train(strengthTrainer, m_data.samples, m_data.strengths);
	dlib::serialize(m_params.at("Dmodel")) << train(durationTrainer, m_data.samples, m_data.durations);
}

void SupportVectorRegression::cross_validation()
{
	// get trainer depending on parameters
	svr_trainer_t slopeTrainer = build_trainer({get_value("Mpenalty"), get_value("Mgamma"), get_value("Mepsilon")});
	svr_trainer_t offsetTrainer = build_trainer({get_value("Bpenalty"), get_value("Bgamma"), get_value("Bepsilon")});
	svr_trainer_t strengthTrainer = build_trainer({get_value("Lpenalty"), get_value("Lgamma"), get_value("Lepsilon")});
	svr_trainer_t durationTrainer = build_trainer({get_value("Dpenalty"), get_value("Dgamma"), get_value("Depsilon")});

	// do cross validation and print results
	std::cout << "slope:    " << dlib::trans(cross_validation(slopeTrainer, m_data.samples, m_data.slopes));
	std::cout << "offset:   " << dlib::trans(cross_validation(offsetTrainer, m_data.samples, m_data.offsets));
	std::cout << "strength: " << dlib::trans(cross_validation(strengthTrainer, m_data.samples, m_data.strengths));
	std::cout << "duration: " << dlib::trans(cross_validation(durationTrainer, m_data.samples, m_data.durations));
}

void SupportVectorRegression::model_selection()
{
	// perform model selection
	la_col_vec optSlope = model_selection(m_data.samples, m_data.slopes);
	la_col_vec optOffset = model_selection(m_data.samples, m_data.offsets);
	la_col_vec optStrength = model_selection(m_data.samples, m_data.strengths);
	la_col_vec optDuration = model_selection(m_data.samples, m_data.durations);

	// print out results
	std::cout << "slope:\t\tC=" << optSlope(0) << "\t\tgamma=" << optSlope(1) << "\t\tepsilon=" << optSlope(2) << std::endl;
	std::cout << "offset:\t\tC=" << optOffset(0) << "\t\tgamma=" << optOffset(1) << "\t\tepsilon=" << optOffset(2) << std::endl;
	std::cout << "strength:\tC=" << optStrength(0) << "\t\tgamma=" << optStrength(1) << "\t\tepsilon=" << optStrength(2) << std::endl;
	std::cout << "duration:\tC=" << optDuration(0) << "\t\tgamma=" << optDuration(1) << "\t\tepsilon=" << optDuration(2) << std::endl;

	// store results
	m_params.at("Mpenalty") = std::to_string(optSlope(0));
	m_params.at("Bpenalty") = std::to_string(optOffset(0));
	m_params.at("Lpenalty") = std::to_string(optStrength(0));
	m_params.at("Dpenalty") = std::to_string(optDuration(0));

	m_params.at("Mgamma") = std::to_string(optSlope(1));
	m_params.at("Bgamma") = std::to_string(optOffset(1));
	m_params.at("Lgamma") = std::to_string(optStrength(1));
	m_params.at("Dgamma") = std::to_string(optDuration(1));

	m_params.at("Mepsilon") = std::to_string(optSlope(2));
	m_params.at("Bepsilon") = std::to_string(optOffset(2));
	m_params.at("Lepsilon") = std::to_string(optStrength(2));
	m_params.at("Depsilon") = std::to_string(optDuration(2));
}

void SupportVectorRegression::predict()
{
	// initialize
	unsigned N (m_data.samples.size());

	// result container
	target_v predictedTargets(N);

	// do prediction
	dlib::parallel_for(0,N, [&](long n)// for (unsigned n=0; n<N; ++n)
	{
		// prepare
		training_s dataTraining (m_data), dataTest;
		std::string label (m_targets[n].label);

		// add to test samples
		dataTest.samples.push_back(m_data.samples[n]);

		// get trainer depending on parameters
		svr_trainer_t slopeTrainer = build_trainer({get_value("Mpenalty"), get_value("Mgamma"), get_value("Mepsilon")});
		svr_trainer_t offsetTrainer = build_trainer({get_value("Bpenalty"), get_value("Bgamma"), get_value("Bepsilon")});
		svr_trainer_t strengthTrainer = build_trainer({get_value("Lpenalty"), get_value("Lgamma"), get_value("Lepsilon")});
		svr_trainer_t durationTrainer = build_trainer({get_value("Dpenalty"), get_value("Dgamma"), get_value("Depsilon")});

		// delete from training data
		dataTraining.samples.erase(dataTraining.samples.begin()+n);
		dataTraining.slopes.erase(dataTraining.slopes.begin()+n);
		dataTraining.offsets.erase(dataTraining.offsets.begin()+n);
		dataTraining.strengths.erase(dataTraining.strengths.begin()+n);
		dataTraining.durations.erase(dataTraining.durations.begin()+n);

		dataTest.slopes = predict(slopeTrainer, dataTraining.samples, dataTraining.slopes, dataTest.samples);
		dataTest.offsets = predict(offsetTrainer, dataTraining.samples, dataTraining.offsets, dataTest.samples);
		dataTest.strengths = predict(strengthTrainer, dataTraining.samples, dataTraining.strengths, dataTest.samples);
		dataTest.durations = predict(durationTrainer, dataTraining.samples, dataTraining.durations, dataTest.samples);

		// rescale predicted target values
		m_scaler.min_max_rescale(dataTest);

		// store result
		pitch_target_s t;
		t.label = label;
		t.m = dataTest.slopes[0];
		t.b = dataTest.offsets[0];
		t.l = dataTest.strengths[0];
		t.d = dataTest.durations[0];
		predictedTargets[n] = t;

		// DEBUG message
		#ifdef DEBUG_MSG
		std::cout << "\t[predict] (" << n << ") predicted: " << label << " " << t.m << " " << t.b << " " << t.l <<  std::endl;
		#endif
	});

	// create output file and write results to it
	std::ofstream fout;
	fout.open (m_params.at("output"));
	fout << std::fixed << std::setprecision(6);
	fout << "name,slope,offset,strength,duration" << std::endl;

	for (pitch_target_s t : predictedTargets)
	{
		fout << t.label << "," << t.m << "," << t.b << "," << t.l << "," << t.d << std::endl;
	}
	fout.close();
}
