#include "training.h"

// *************** SVRERROR ***************

double SvrCvError::operator() (const la_col_vec& logArg) const
{
	la_col_vec arg = exp(logArg);
	svr_params params ({arg(0), arg(1), arg(2)});
	dlib::matrix<double,1,2> tmp = dlib::cross_validate_regression_trainer(SupportVectorRegression::build_trainer(params), m_samples, m_targets, 5);

	return tmp(0);
}

// *************** SUPPORTVECTORREGRESSION ***************

grid_t SupportVectorRegression::get_grid(const la_col_vec& lowerBound, const la_col_vec& upperBound, const std::vector<unsigned>& numPerDim) const
{
	// get one dimension
	dlib::matrix<double> cSpace 		= dlib::logspace(log10(upperBound(0)), log10(lowerBound(0)), numPerDim[0]);
	dlib::matrix<double> gammaSpace 	= dlib::logspace(log10(upperBound(1)), log10(lowerBound(1)), numPerDim[1]);
	dlib::matrix<double> intensitySpace = dlib::logspace(log10(upperBound(2)), log10(lowerBound(2)), numPerDim[2]);

	// create grid points and return
	dlib::matrix<double> tmpGrid = dlib::cartesian_product(cSpace, gammaSpace);
	return dlib::cartesian_product(tmpGrid, intensitySpace);
}

svr_trainer_t SupportVectorRegression::build_trainer(const svr_params& params)
{
	// create a trainer
	svr_trainer_t trainer;

	// setup parameters for trainer
	trainer.set_c(params.C);
	trainer.set_kernel(svr_kernel_t(params.gamma));
	trainer.set_epsilon_insensitivity(params.intensity);

	// return trainer
	return trainer;
}

svr_model SupportVectorRegression::get_trained_model (const training_s& data) const
{
	// get trainer depending on parameters
	svr_trainer_t slopeTrainer = build_trainer({get_value("slope_regularization"), get_value("slope_gamma"), get_value("slope_intensity")});
	svr_trainer_t offsetTrainer = build_trainer({get_value("offset_regularization"), get_value("offset_gamma"), get_value("offset_intensity")});
	svr_trainer_t strengthTrainer = build_trainer({get_value("strength_regularization"), get_value("strength_gamma"), get_value("strength_intensity")});
	svr_trainer_t durationTrainer = build_trainer({get_value("duration_regularization"), get_value("duration_gamma"), get_value("duration_intensity")});

	// do the training
	svr_model model;
	model.slopePredictor = slopeTrainer.train(data.samples, m_data.slopes);
	model.offsetPredictor = offsetTrainer.train(data.samples, m_data.offsets);
	model.strengthPredictor = strengthTrainer.train(data.samples, m_data.strengths);
	model.durationPredictor = durationTrainer.train(data.samples, m_data.durations);

	// print results
	unsigned N (data.samples.size());
	std::cout << "\t[train] Number of support vectors (slope-model): " << model.slopePredictor.basis_vectors.size() << "/" << N << std::endl;
	std::cout << "\t[train] Number of support vectors (offset-model): " << model.offsetPredictor.basis_vectors.size() << "/" << N << std::endl;
	std::cout << "\t[train] Number of support vectors (strength-model): " << model.strengthPredictor.basis_vectors.size() << "/" << N << std::endl;
	std::cout << "\t[train] Number of support vectors (duration-model): " << model.durationPredictor.basis_vectors.size() << "/" << N << std::endl;

	return model;
}

void SupportVectorRegression::predict_targets(const svr_model& model, const training_s& data, const std::string& targetFile)
{
	// result container
	unsigned N (data.samples.size());
	target_v predictedTargets;

	// predict targets for test set
	for (unsigned n=0; n<N; ++n)
	{
		// store result
		pitch_target_s t;
		t.label = data.labels[n];
		t.m = model.slopePredictor(data.samples[n]);
		t.b = model.offsetPredictor(data.samples[n]);
		t.l = model.strengthPredictor(data.samples[n]);
		t.d = model.durationPredictor(data.samples[n]);
		predictedTargets.push_back(t);
	}

	// rescale predicted target values
	m_scaler.min_max_rescale(predictedTargets);

	// create output file and write results to it
	std::ofstream fout;
	fout.open (targetFile);
	fout << std::fixed << std::setprecision(6);
	fout << "name,slope,offset,strength,duration" << std::endl;
	for (pitch_target_s t : predictedTargets)
	{
		fout << t.label << "," << t.m << "," << t.b << "," << t.l << "," << t.d << std::endl;
	}
	fout.close();
}

svr_params SupportVectorRegression::select_model(const sample_v& samples, const std::vector<double>& targets) const
{
	// define parameter search space {C,gamma,intensity}
	la_col_vec lowerBound(3), upperBound(3);
	std::vector<unsigned> dimensions;
	lowerBound = 1e-3, 1e-5, 1e-5;
	upperBound = 1e3, 1e1, 1e0;
	dimensions = {12,12,7};
	grid_t grid = get_grid(lowerBound, upperBound, dimensions);

	// store optimal parameters
	svr_params optParams;
	double minMSE (1e6);

	// loop over grid
	dlib::mutex mu;
	dlib::parallel_for(0, grid.nc(), [&](long col) //for(long col = 0; col < grid.nc(); ++col)
	{
        // do cross validation and then check if the results are the best
		svr_params params ({grid(0, col),grid(1, col),grid(2, col)});
    	dlib::matrix<double,1,2> tmp = dlib::cross_validate_regression_trainer(build_trainer(params), samples, targets, 5);
        double tmpMSE = tmp(0);

        // save the best results
        dlib::auto_mutex lock(mu);
        if (tmpMSE < minMSE)
        {
        	optParams = params;
        	minMSE = tmpMSE;
        }

		// DEBUG message
		#ifdef DEBUG_MSG
		std::cout << "\t[select_model] STATUS: (" << col << "/" << grid.nc() << ") MSE=" << tmpMSE << "[" << minMSE << "] {" << params.C << "," << params.gamma << "," << params.intensity << "}" << std::endl;
		#endif
    });

    // optimization with BOBYQA
	la_col_vec arg(3);
	arg = optParams.C,optParams.gamma,optParams.intensity;

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
		#ifdef DEBUG_MSG
		std::cout << "\t[model_selection] WARNING: no convergence during optimization -> " << err.info << std::endl;
		#endif
	}

	// store results
	arg = exp(logArg);
	optParams.C = arg(0);
	optParams.gamma = arg(1);
	optParams.intensity = arg(2);

    return optParams;
}

void SupportVectorRegression::train()
{
	// train the model
	svr_model model = get_trained_model(m_data);

	// store model
	dlib::serialize(m_params.at("slope_model")) << model.slopePredictor;
	dlib::serialize(m_params.at("offset_model")) << model.offsetPredictor;
	dlib::serialize(m_params.at("strength_model")) << model.strengthPredictor;
	dlib::serialize(m_params.at("duration_model")) << model.durationPredictor;
}

void SupportVectorRegression::predict()
{
	// initialize
	double fraction (0.75);
	training_s trainingData, testData;
	get_separated_data(trainingData, testData, fraction);

	// train the model
	svr_model model = get_trained_model(m_data);

	// predict targets
	predict_targets(model, trainingData, m_params.at("output_training"));
	predict_targets(model, testData, m_params.at("output_test"));
}

void SupportVectorRegression::cross_validation()
{
	// get trainer depending on parameters
	svr_trainer_t slopeTrainer = build_trainer({get_value("slope_regularization"), get_value("slope_gamma"), get_value("slope_intensity")});
	svr_trainer_t offsetTrainer = build_trainer({get_value("offset_regularization"), get_value("offset_gamma"), get_value("offset_intensity")});
	svr_trainer_t strengthTrainer = build_trainer({get_value("strength_regularization"), get_value("strength_gamma"), get_value("strength_intensity")});
	svr_trainer_t durationTrainer = build_trainer({get_value("duration_regularization"), get_value("duration_gamma"), get_value("duration_intensity")});

	// do cross validation and print results
	std::cout << "slope:    " << dlib::trans(cross_validate_regression_trainer(slopeTrainer, m_data.samples, m_data.slopes,5));
	std::cout << "offset:   " << dlib::trans(cross_validate_regression_trainer(offsetTrainer, m_data.samples, m_data.offsets,5));
	std::cout << "strength: " << dlib::trans(cross_validate_regression_trainer(strengthTrainer, m_data.samples, m_data.strengths,5));
	std::cout << "duration: " << dlib::trans(cross_validate_regression_trainer(durationTrainer, m_data.samples, m_data.durations,5));
}

void SupportVectorRegression::model_selection()
{
	// perform model selection
	svr_params optSlope = select_model(m_data.samples, m_data.slopes);
	svr_params optOffset = select_model(m_data.samples, m_data.offsets);
	svr_params optStrength = select_model(m_data.samples, m_data.strengths);
	svr_params optDuration = select_model(m_data.samples, m_data.durations);

	// print out results
	std::cout << "slope:\t\tC=" << optSlope.C << "\t\tgamma=" << optSlope.gamma << "\t\tintensity=" << optSlope.intensity << std::endl;
	std::cout << "offset:\t\tC=" << optOffset.C << "\t\tgamma=" << optOffset.gamma << "\t\tintensity=" << optOffset.intensity << std::endl;
	std::cout << "strength:\tC=" << optStrength.C << "\t\tgamma=" << optStrength.gamma << "\t\tintensity=" << optStrength.intensity << std::endl;
	std::cout << "duration:\tC=" << optDuration.C << "\t\tgamma=" << optDuration.gamma << "\t\tintensity=" << optDuration.intensity << std::endl;

	// store results
	m_params.at("slope_regularization") = std::to_string(optSlope.C);
	m_params.at("offset_regularization") = std::to_string(optOffset.C);
	m_params.at("strength_regularization") = std::to_string(optStrength.C);
	m_params.at("duration_regularization") = std::to_string(optDuration.C);

	m_params.at("slope_gamma") = std::to_string(optSlope.gamma);
	m_params.at("offset_gamma") = std::to_string(optOffset.gamma);
	m_params.at("strength_gamma") = std::to_string(optStrength.gamma);
	m_params.at("duration_gamma") = std::to_string(optDuration.gamma);

	m_params.at("slope_intensity") = std::to_string(optSlope.intensity);
	m_params.at("offset_intensity") = std::to_string(optOffset.intensity);
	m_params.at("strength_intensity") = std::to_string(optStrength.intensity);
	m_params.at("duration_intensity") = std::to_string(optDuration.intensity);
}
