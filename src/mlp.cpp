#include "training.h"

// *************** MLPCVERROR ***************

double MlpCvError::operator() (const la_col_vec& logArg) const
{
	la_col_vec arg = exp(logArg);
	mlp_model_t net (NUMFEATSYL, (unsigned)arg(0), (unsigned)arg(1), 1, (unsigned)arg(2), (unsigned)arg(3));
	double mse = MultiLayerPerceptron::cross_validate_regression_network(net, m_samples, m_targets, m_folds);
	return mse;
}

// *************** MULTILAYERPERCEPTRON ***************

double MultiLayerPerceptron::cross_validate_regression_network(mlp_model_t& net, const sample_v& samples, const std::vector<double>& targets, const unsigned& folds)
{
	// result: cross-validation-error: mean of mse
	double mmse (0);

	// fold borders
	dlib::matrix<double> borders = dlib::linspace(0, samples.size(), folds+1);

	for (unsigned i=0; i<folds; ++i)
	{
		// determine fold borders
		unsigned start = (unsigned)borders(i);
		unsigned end = (unsigned)borders(i+1);

		// train the model
		net.reset();
		for (unsigned i=0; i<samples.size(); ++i)
		{
			if (!(start<=i && i<end))
			{
				net.train(samples[i], targets[i]);
			}
		}

		// predict targets
		double sse (0);
		for (unsigned i=start; i<end; ++i)
		{
			sse += loss(targets[i], net(samples[i]));
		}

		// store result
		mmse += (sse/(end-start+1));
	}

	return mmse/folds;
}

void MultiLayerPerceptron::get_trained_model (const training_s& data, mlp_model model) const
{
	// get trainer depending on parameters
	mlp_model_t netSlope (NUMFEATSYL, (unsigned)get_value("slope_number_L1"), (unsigned)get_value("slope_number_L2"), 1, get_value("slope_alpha"), get_value("slope_momentum"));
	mlp_model_t netOffset (NUMFEATSYL, (unsigned)get_value("offset_number_L1"), (unsigned)get_value("offset_number_L2"), 1, get_value("offset_alpha"), get_value("offset_momentum"));
	mlp_model_t netStrength (NUMFEATSYL, (unsigned)get_value("strength_number_L1"), (unsigned)get_value("strength_number_L2"), 1, get_value("strength_alpha"), get_value("strength_momentum"));
	mlp_model_t netDuration (NUMFEATSYL, (unsigned)get_value("duration_number_L1"), (unsigned)get_value("duration_number_L2"), 1, get_value("duration_alpha"), get_value("duration_momentum"));

	// do the training
	for (unsigned i=0; i<data.samples.size(); ++i)
	{
		netSlope.train(data.samples[i], data.slopes[i]);
		netOffset.train(data.samples[i], data.offsets[i]);
		netStrength.train(data.samples[i], data.strengths[i]);
		netDuration.train(data.samples[i], data.durations[i]);
	}

	// write the trained networks back
	model.slopePredictor.swap(netSlope);
	model.offsetPredictor.swap(netOffset);
	model.strengthPredictor.swap(netStrength);
	model.durationPredictor.swap(netDuration);

	#ifdef DEBUG_MSG
	std::cout << "[get_trained_model] trained MLP successfully" << std::endl;
	std::cout << "\tFollowing parameters used:" << std::endl;
	std::cout << "\tslope:\t\tL1=" << get_value("slope_number_L1") << "\t\tL2=" << get_value("slope_number_L2") << "\t\talpha=" << get_value("slope_alpha") << "\t\tmomentum=" << get_value("slope_momentum") << std::endl;
	std::cout << "\toffset:\t\tL1=" << get_value("offset_number_L1") << "\t\tL2=" << get_value("offset_number_L2") << "\t\talpha=" << get_value("offset_alpha") << "\t\tmomentum=" << get_value("offset_momentum") << std::endl;
	std::cout << "\tstrength:\tL1=" << get_value("strength_number_L1") << "\t\tL2=" << get_value("strength_number_L2") << "\t\talpha=" << get_value("strength_alpha") << "\t\tmomentum=" << get_value("strength_momentum") << std::endl;
	std::cout << "\tduration:\tL1=" << get_value("duration_number_L1") << "\t\tL2=" << get_value("duration_number_L2") << "\t\talpha=" << get_value("duration_alpha") << "\t\tmomentum=" << get_value("duration_momentum") << std::endl;
	#endif
}

void MultiLayerPerceptron::predict_targets(const mlp_model& model, training_s& data) const
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

grid_t MultiLayerPerceptron::get_grid(const la_col_vec& lowerBound, const la_col_vec& upperBound, const std::vector<unsigned>& numPerDim) const
{
	// get one dimension
	dlib::matrix<double> num1Space 	= dlib::linspace(upperBound(0), lowerBound(0), numPerDim[0]);
	dlib::matrix<double> num2Space 	= dlib::linspace(upperBound(1), lowerBound(1), numPerDim[1]);
	dlib::matrix<double> alphaSpace = dlib::logspace(log10(upperBound(2)), log10(lowerBound(2)), numPerDim[2]);
	dlib::matrix<double> momentumSpace = dlib::logspace(log10(upperBound(3)), log10(lowerBound(3)), numPerDim[3]);

	// create grid points and return
	dlib::matrix<double> tmpGrid = dlib::cartesian_product(num1Space, num2Space);
	tmpGrid = dlib::cartesian_product(tmpGrid, alphaSpace);
	return dlib::cartesian_product(tmpGrid, momentumSpace);
}

void MultiLayerPerceptron::save_parameters(const mlp_params& slope, const mlp_params& offset, const mlp_params& strength, const mlp_params& duration, algorithm_m& params) const
{
	params.at("slope_number_L1") = std::to_string(slope.num1layer);
	params.at("offset_number_L1") = std::to_string(offset.num1layer);
	params.at("strength_number_L1") = std::to_string(strength.num1layer);
	params.at("duration_number_L1") = std::to_string(duration.num1layer);

	params.at("slope_number_L2") = std::to_string(slope.num2layer);
	params.at("offset_number_L2") = std::to_string(offset.num2layer);
	params.at("strength_number_L2") = std::to_string(strength.num2layer);
	params.at("duration_number_L2") = std::to_string(duration.num2layer);

	params.at("slope_alpha") = std::to_string(slope.alpha);
	params.at("offset_alpha") = std::to_string(offset.alpha);
	params.at("strength_alpha") = std::to_string(strength.alpha);
	params.at("duration_alpha") = std::to_string(duration.alpha);

	params.at("slope_momentum") = std::to_string(slope.momentum);
	params.at("offset_momentum") = std::to_string(offset.momentum);
	params.at("strength_momentum") = std::to_string(strength.momentum);
	params.at("duration_momentum") = std::to_string(duration.momentum);
}

mlp_params MultiLayerPerceptron::select_model(const sample_v& samples, const std::vector<double>& targets,  const grid_t& grid, const la_col_vec& lowerBound, const la_col_vec& upperBound) const
{
	// store optimal parameters
	mlp_params optParams; double minMSE (1e6);

	// optimization via grid search
	dlib::mutex mu;
	dlib::parallel_for(0, grid.nc(), [&](long col) //for(long col = 0; col < grid.nc(); ++col)
	{
        // do cross validation and then check if the results are the best
		mlp_params params ({(unsigned)grid(0, col),(unsigned)grid(1, col),grid(2, col),grid(3, col)});
		mlp_model_t net (NUMFEATSYL, params.num1layer, params.num2layer, 1, params.alpha, params.momentum);
    	double tmp = cross_validate_regression_network(net, samples, targets, m_folds);
        double tmpMSE = tmp;

        // save the best results
        dlib::auto_mutex lock(mu);
        if (tmpMSE < minMSE)
        {
        	optParams = params;
        	minMSE = tmpMSE;

    		#ifdef DEBUG_MSG
    		std::cout << "\t[select_model] STATUS: (" << col << "/" << grid.nc() << ") MSE=" << minMSE << " {" << params.num1layer << "," << params.num2layer << "," << params.alpha << "," << params.momentum << "}" << std::endl;
    		#endif
        }
    });

    // optimization via BOBYQA
	la_col_vec arg(4);
	arg = optParams.num1layer,optParams.num2layer,optParams.alpha,optParams.momentum;
	la_col_vec logArg = log(arg), logLowerBound = log(lowerBound), logUpperBound = log(upperBound); // log scale
    dlib::find_min_bobyqa(MlpCvError (samples, targets, m_folds), logArg, arg.size()*2+1, logLowerBound, logUpperBound, min(logUpperBound-logLowerBound)/10, 1e-2, 100);

	// return  results
	arg = exp(logArg); optParams.num1layer = arg(0); optParams.num2layer = arg(1); optParams.alpha = arg(2); optParams.momentum = arg(3);
    return optParams;
}

void MultiLayerPerceptron::train()
{
	// create and train the model
	mlp_model_t net1(1,1), net2(1,1), net3(1,1), net4(1,1);
	mlp_model model ({net1, net2, net3, net4});
	get_trained_model(m_data, model);

	// store model
	dlib::serialize(m_params.at("slope_model")) << model.slopePredictor;
	dlib::serialize(m_params.at("offset_model")) << model.offsetPredictor;
	dlib::serialize(m_params.at("strength_model")) << model.strengthPredictor;
	dlib::serialize(m_params.at("duration_model")) << model.durationPredictor;

	std::cout << "[train] MLP training finished successfully" << std::endl;
}

void MultiLayerPerceptron::predict()
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

		// create and train the model
		mlp_model_t net1(1,1), net2(1,1), net3(1,1), net4(1,1);
		mlp_model model ({net1, net2, net3, net4});
		get_trained_model(m_data, model);

		// predict targets
		predict_targets(model, dataTest);

		// store predicted targets
		push_back_targets(targetsPredicted, dataTest);
	}

	// rescale predicted target values
	m_scaler.min_max_rescale(targetsPredicted);

	// save target file
	save_target_file(targetsPredicted, m_params.at("output_file"));

	std::cout << "[predict] MLP prediction finished successfully" << std::endl;
}

void MultiLayerPerceptron::model_selection()
{
	// define parameter search space {num1layer, num2layer, alpha}
	la_col_vec lowerBound(4), upperBound(4);
	std::vector<unsigned> dimensions;
	lowerBound = 1e0, 1e0, 1e-3, 1e-3;
	upperBound = 2e2, 2e2, 1e1, 1e1;
	dimensions = {15,15,10,10};

	// calculate grid for grid search
	grid_t grid = get_grid(lowerBound, upperBound, dimensions);

	// print information
	std::cout << "[model_selection] start model-selection..." << std::endl;
	std::cout << "\tgrid size: " << grid.nc() << std::endl;

	// perform model selection
	mlp_params optSlope = select_model(m_data.samples, m_data.slopes, grid, lowerBound, upperBound);
	std::cout << "\tslope:\t\tL1=" << optSlope.num1layer << "\t\tL2=" << optSlope.num2layer << "\t\talpha=" << optSlope.alpha << "\t\tmomentum=" << optSlope.momentum << std::endl;
	mlp_params optOffset = select_model(m_data.samples, m_data.offsets, grid, lowerBound, upperBound);
	std::cout << "\toffset:\t\tL1=" << optOffset.num1layer << "\t\tL2=" << optOffset.num2layer << "\t\talpha=" << optOffset.alpha << "\t\tmomentum=" << optOffset.momentum << std::endl;
	mlp_params optStrength = select_model(m_data.samples, m_data.strengths, grid, lowerBound, upperBound);
	std::cout << "\tstrength:\tL1=" << optStrength.num1layer << "\t\tL2=" << optStrength.num2layer << "\t\talpha=" << optStrength.alpha << "\t\tmomentum=" << optStrength.momentum << std::endl;
	mlp_params optDuration = select_model(m_data.samples, m_data.durations, grid, lowerBound, upperBound);
	std::cout << "\tduration:\tL1=" << optDuration.num1layer << "\t\tL2=" << optDuration.num2layer << "\t\talpha=" << optDuration.alpha << "\t\tmomentum=" << optDuration.momentum << std::endl;

	// store results
	save_parameters(optSlope, optOffset, optStrength, optDuration, m_params);

	std::cout << "[model_selection] SVR model-selection finished successfully" << std::endl;
}
