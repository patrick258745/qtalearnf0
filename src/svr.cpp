#include "training.h"

// *************** SVRERROR ***************

double SvrCvError::operator() (const la_col_vec& logArg) const
{
	la_col_vec arg = exp(logArg);
	svr_params params ({arg(0), arg(1), arg(2)});
	dlib::matrix<double,1,2> tmp = dlib::cross_validate_regression_trainer(SupportVectorRegression::build_trainer(params), m_samples, m_targets, m_folds);
	return tmp(0);
}

// *************** SUPPORTVECTORREGRESSION ***************

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

	#ifdef DEBUG_MSG
	unsigned N (data.samples.size());
	std::cout << "[get_trained_model] trained SVR successfully" << std::endl;
	std::cout << "\tFollowing parameters used:" << std::endl;
	std::cout << "\tslope:\t\tC=" << get_value("slope_regularization") << "\t\tgamma=" << get_value("slope_gamma") << "\t\tintensity=" << get_value("slope_intensity") << "\t\t#SVs=" << model.slopePredictor.basis_vectors.size() << "/" << N << std::endl;
	std::cout << "\toffset:\t\tC=" << get_value("offset_regularization") << "\t\tgamma=" << get_value("offset_gamma") << "\t\tintensity=" << get_value("offset_intensity") << "\t\t#SVs=" << model.offsetPredictor.basis_vectors.size() << "/" << N << std::endl;
	std::cout << "\tstrength:\tC=" << get_value("strength_regularization") << "\t\tgamma=" << get_value("strength_gamma") << "\t\tintensity=" << get_value("strength_intensity") << "\t\t#SVs=" << model.strengthPredictor.basis_vectors.size() << "/" << N << std::endl;
	std::cout << "\tduration:\tC=" << get_value("duration_regularization") << "\t\tgamma=" << get_value("duration_gamma") << "\t\tintensity=" << get_value("duration_intensity") << "\t\t#SVs=" << model.durationPredictor.basis_vectors.size() << "/" << N << std::endl;
	#endif

	return model;
}

void SupportVectorRegression::predict_targets(const svr_model& model, training_s& data) const
{
	// predict targets for test set
	for (unsigned n=0; n<data.labels.size(); ++n)
	{
		data.slopes[n] = model.slopePredictor(data.samples[n]);
		data.offsets[n] = model.offsetPredictor(data.samples[n]);
		data.strengths[n] = model.strengthPredictor(data.samples[n]);
		data.durations[n] = model.durationPredictor(data.samples[n]);
	}
}

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

void SupportVectorRegression::save_parameters(const svr_params& slope, const svr_params& offset, const svr_params& strength, const svr_params& duration, algorithm_m& params) const
{
	params.at("slope_regularization") = std::to_string(slope.C);
	params.at("offset_regularization") = std::to_string(offset.C);
	params.at("strength_regularization") = std::to_string(strength.C);
	params.at("duration_regularization") = std::to_string(duration.C);

	params.at("slope_gamma") = std::to_string(slope.gamma);
	params.at("offset_gamma") = std::to_string(offset.gamma);
	params.at("strength_gamma") = std::to_string(strength.gamma);
	params.at("duration_gamma") = std::to_string(duration.gamma);

	params.at("slope_intensity") = std::to_string(slope.intensity);
	params.at("offset_intensity") = std::to_string(offset.intensity);
	params.at("strength_intensity") = std::to_string(strength.intensity);
	params.at("duration_intensity") = std::to_string(duration.intensity);
}

svr_params SupportVectorRegression::select_model(const sample_v& samples, const std::vector<double>& targets,  const grid_t& grid, const la_col_vec& lowerBound, const la_col_vec& upperBound) const
{
	// store optimal parameters
	svr_params optParams; double minMSE (1e6);

	// optimization via grid search
	dlib::mutex mu;
	dlib::parallel_for(0, grid.nc(), [&](long col) //for(long col = 0; col < grid.nc(); ++col)
	{
        // do cross validation and then check if the results are the best
		svr_params params ({grid(0, col),grid(1, col),grid(2, col)});
    	dlib::matrix<double,1,2> tmp = dlib::cross_validate_regression_trainer(build_trainer(params), samples, targets, m_folds);
        double tmpMSE = tmp(0);

        // save the best results
        dlib::auto_mutex lock(mu);
        if (tmpMSE < minMSE)
        {
        	optParams = params;
        	minMSE = tmpMSE;

    		#ifdef DEBUG_MSG
    		std::cout << "\t[select_model] STATUS: (" << col << "/" << grid.nc() << ") MSE=" << minMSE << " {" << params.C << "," << params.gamma << "," << params.intensity << "}" << std::endl;
    		#endif
        }
    });

    // optimization via BOBYQA
	la_col_vec arg(3);
	arg = optParams.C,optParams.gamma,optParams.intensity;
	la_col_vec logArg = log(arg), logLowerBound = log(lowerBound), logUpperBound = log(upperBound); // log scale
    dlib::find_min_bobyqa(SvrCvError (samples, targets, m_folds), logArg, arg.size()*2+1, logLowerBound, logUpperBound, min(logUpperBound-logLowerBound)/10, 1e-2, 100);

	// return  results
	arg = exp(logArg); optParams.C = arg(0); optParams.gamma = arg(1); optParams.intensity = arg(2);
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

	std::cout << "[train] SVR training finished successfully" << std::endl;
}

void SupportVectorRegression::predict()
{
	// result container
	target_v targetsPredicted;

	// fold borders
	dlib::matrix<double> borders = dlib::linspace(0, m_data.labels.size(), m_folds+1);

	for (unsigned i=0; i<m_folds; ++i)
	{
		// data for training
		training_s dataTraining, dataTest;

		// determine fold borders
		unsigned start = (unsigned)borders(i);
		unsigned end = (unsigned)borders(i+1);

		// split corpus data into test and training data
		split_data(m_data, dataTraining, dataTest, start, end);

		// train the model
		svr_model model = get_trained_model(dataTraining);

		// predict targets
		predict_targets(model, dataTest);

		// store predicted targets
		push_back_targets(targetsPredicted, dataTest);
	}

	// rescale predicted target values
	m_scaler.min_max_rescale(targetsPredicted);

	// save target file
	save_target_file(targetsPredicted, m_params.at("output_file"));

	std::cout << "[predict] SVR prediction finished successfully" << std::endl;
}

void SupportVectorRegression::model_selection()
{
	// define parameter search space {C,gamma,intensity}
	la_col_vec lowerBound(3), upperBound(3);
	std::vector<unsigned> dimensions;
	lowerBound = 1e-3, 1e-3, 1e-3;
	upperBound = 1e1, 1e1, 1e1;
	dimensions = {10,10,10};

	// calculate grid for grid search
	grid_t grid = get_grid(lowerBound, upperBound, dimensions);

	// print information
	std::cout << "[model_selection] start model-selection..." << std::endl;
	std::cout << "\tgrid size: " << grid.nc() << std::endl;

	// perform model selection
	svr_params optSlope = select_model(m_data.samples, m_data.slopes, grid, lowerBound, upperBound);
	std::cout << "\tslope:\t\tC=" << optSlope.C << "\t\tgamma=" << optSlope.gamma << "\t\tintensity=" << optSlope.intensity << std::endl;
	svr_params optOffset = select_model(m_data.samples, m_data.offsets, grid, lowerBound, upperBound);
	std::cout << "\toffset:\t\tC=" << optOffset.C << "\t\tgamma=" << optOffset.gamma << "\t\tintensity=" << optOffset.intensity << std::endl;
	svr_params optStrength = select_model(m_data.samples, m_data.strengths, grid, lowerBound, upperBound);
	std::cout << "\tstrength:\tC=" << optStrength.C << "\t\tgamma=" << optStrength.gamma << "\t\tintensity=" << optStrength.intensity << std::endl;
	svr_params optDuration = select_model(m_data.samples, m_data.durations, grid, lowerBound, upperBound);
	std::cout << "\tduration:\tC=" << optDuration.C << "\t\tgamma=" << optDuration.gamma << "\t\tintensity=" << optDuration.intensity << std::endl;

	// store results
	save_parameters(optSlope, optOffset, optStrength, optDuration, m_params);

	std::cout << "[model_selection] SVR model-selection finished successfully" << std::endl;
}
