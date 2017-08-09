#include "training.h"

// *************** MLPCVERROR ***************

double MlpCvError::operator() (const la_col_vec& arg) const
{
	mlp_params params ({(unsigned)arg(0), (unsigned)arg(1), arg(2), arg(3)});
	double mse = MultiLayerPerceptron::cross_validate_regression_network(params, m_data, 5);
	return mse;
}

// *************** MULTILAYERPERCEPTRON ***************

double MultiLayerPerceptron::measure_error(const la_col_vec& mse)
{
	return sum(squared(mse))/4;
	//return sum(squared(pow(mse,0.2)))/4;
}

mlp_params MultiLayerPerceptron::get_default_params() const
{
	// get parameters
	unsigned numFirstLayer = (unsigned)get_value("L1_number");
	unsigned numSecondLayer = (unsigned)get_value("L2_number");
	double alpha = get_value("alpha");
	double momentum = get_value("momentum");

	// create params struct
	mlp_params params;
	params.num1layer = numFirstLayer;
	params.num2layer = numSecondLayer;
	params.alpha = alpha;
	params.momentum = momentum;

	return params;
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

void MultiLayerPerceptron::train_network (mlp_kernel_t& network, const training_s& data)
{
	// iterate over all samples for training
	for (unsigned i=0; i<data.samples.size(); ++i)
	{
		// get network output and input
		la_col_vec output(4);
		output = data.slopes[i],data.offsets[i],data.strengths[i],data.durations[i];
		const sample_t& input = data.samples[i];

		// train the network
		network.train(input, output);
		//std::cout << "average change: " << network.get_average_change() << std::endl;
	}

	//std::cout << "[train] trained MLP successfully" << std::endl << std::endl;
}

double MultiLayerPerceptron::predict_targets (mlp_kernel_t& network, training_s& testData)
{
	// initialize
	la_col_vec sse(4);	// sum of squared errors
	sse = 0,0,0,0;
	unsigned N (testData.samples.size());

	// evaluate predictions
	for (unsigned i=0; i<N; ++i)
	{
		// prediction
		dlib::matrix<double> preTargets = network(testData.samples[i]);

		// sum squared errors
		sse(0) += loss(preTargets(0), testData.slopes[i]);
		sse(1) += loss(preTargets(1), testData.offsets[i]);
		sse(2) += loss(preTargets(2), testData.strengths[i]);
		sse(3) += loss(preTargets(3), testData.durations[i]);

		// store predicted values
		testData.slopes[i] = preTargets(0);
		testData.offsets[i] = preTargets(1);
		testData.strengths[i] = preTargets(2);
		testData.durations[i] = preTargets(3);
	}

	la_col_vec mse = sse/N;
	return measure_error(mse);
}

void MultiLayerPerceptron::save_to_file (training_s& data, const std::string& targetFile)
{
	// rescale predicted target values
	unsigned N (data.samples.size());
	m_scaler.min_max_rescale(data);

	// create output file and write results to it
	std::ofstream fout;
	fout.open (targetFile);
	fout << std::fixed << std::setprecision(6);
	fout << "name,slope,offset,strength,duration" << std::endl;
	for (unsigned i=0; i<N; ++i)
	{
		fout << data.labels[i] << "," << data.slopes[i] << "," << data.offsets[i] << "," << data.strengths[i] << "," << data.durations[i] << std::endl;
	}
	fout.close();
}

double MultiLayerPerceptron::cross_validate_regression_network(const mlp_params& params, const training_s& data, const unsigned& folds)
{
	// mean mse
	double mse (0.0);

	// fold borders
	dlib::matrix<double> borders = dlib::linspace(0, data.samples.size(), folds+1);

	for (unsigned i=0; i<folds; ++i)
	{
		// data for training
		training_s dataTraining (data), dataTest;

		// determine fold borders
		unsigned start = (unsigned)borders(i);
		unsigned end = (unsigned)borders(i+1);

		// get data for test
		for (unsigned j=start; j<end; ++j)
		{
			dataTest.samples.push_back(data.samples[j]);
			dataTest.slopes.push_back(data.slopes[j]);
			dataTest.offsets.push_back(data.offsets[j]);
			dataTest.strengths.push_back(data.strengths[j]);
			dataTest.durations.push_back(data.durations[j]);
		}

		// delete from training data
		dataTraining.samples.erase(dataTraining.samples.begin()+start, dataTraining.samples.begin()+end);
		dataTraining.slopes.erase(dataTraining.slopes.begin()+start, dataTraining.slopes.begin()+end);
		dataTraining.offsets.erase(dataTraining.offsets.begin()+start, dataTraining.offsets.begin()+end);
		dataTraining.strengths.erase(dataTraining.strengths.begin()+start, dataTraining.strengths.begin()+end);
		dataTraining.durations.erase(dataTraining.durations.begin()+start, dataTraining.durations.begin()+end);

		// train the network
		mlp_kernel_t net(NUMFEATSYL, params.num1layer, params.num2layer, 4, params.alpha, params.momentum);
		train_network(net, dataTraining);

		// measure predictive error
		mse += predict_targets(net, dataTest);
	}

	return mse/folds;
}

mlp_params MultiLayerPerceptron::select_model(const training_s& data)
{
	// define parameter search space {num1layer, num2layer, alpha}
	la_col_vec lowerBound(4), upperBound(4);
	std::vector<unsigned> dimensions;
	lowerBound = 0, 0, 1e-3, 01e-3;
	upperBound = 64, 64, 1e1, 1e1;
	dimensions = {15,10,10,10};

	// store optimal parameters
	mlp_params optParams;
	double minMSE (1e6);

	// create grid for grid search
	grid_t grid = get_grid(lowerBound, upperBound, dimensions);

	std::cout << "\tgrid size: " << grid.size() << std::endl;

	// loop over grid
	dlib::mutex mu;
	dlib::parallel_for(0, grid.nc(), [&](long col) //for(long col = 0; col < grid.nc(); ++col)
	{
        // do cross validation and then check if the results are the best
		mlp_params params ({(unsigned)grid(0, col),(unsigned)grid(1, col),grid(2, col),grid(3, col)});
    	double tmpMSE = cross_validate_regression_network(params, data, 5);

    	// save the best results
		dlib::auto_mutex lock(mu);
		if (tmpMSE < minMSE)
		{
			optParams = params;
			minMSE = tmpMSE;
		}

		// DEBUG message
		#ifdef DEBUG_MSG
		std::cout << "\t[select_model] STATUS: (" << col << "/" << grid.nc() << ") MSE=" << tmpMSE << "[" << minMSE << "] {" << params.num1layer << "," << params.num2layer << "," << params.alpha << "," << params.momentum << "}" << std::endl;
		#endif
    });

    // optimization with BOBYQA
	la_col_vec arg(4);
	arg = optParams.num1layer,optParams.num2layer,optParams.alpha,optParams.momentum;

    try
    {
    	// optimization
    	dlib::find_min_bobyqa(MlpCvError (data), arg, arg.size()*2+1, lowerBound, upperBound, min(upperBound-lowerBound)/10, 1e-2, 100);
    }
	catch (dlib::bobyqa_failure& err)
	{
		// DEBUG message
		#ifdef DEBUG_MSG
		std::cout << "\t[model_selection] WARNING: no convergence during optimization -> " << err.info << std::endl;
		#endif
	}

	// store results
	optParams.num1layer = arg(0);
	optParams.num2layer = arg(1);
	optParams.alpha = arg(2);
	optParams.momentum = arg(3);

    return optParams;
}

void MultiLayerPerceptron::train()
{
	// get network parameters and create network
	mlp_params params = get_default_params();
	mlp_kernel_t net(NUMFEATSYL, params.num1layer, params.num2layer, 4, params.alpha, params.momentum);

	// train the network
	train_network(net, m_data);

	// save the trained model to file
	dlib::serialize(m_params.at("model")) << net;

	std::cout << "[train] training finished successfully. Following Parameters used:" << std::endl;
	std::cout << "\tL1=" << params.num1layer << "\t\tL2=" << params.num2layer << "\t\talpha=" << params.alpha << "\t\tmomentum=" << params.momentum << std::endl;

}

void MultiLayerPerceptron::predict(double fraction = 0.8)
{
	// initialize
	unsigned N (m_data.samples.size());
	training_s trainingData, testData;
	get_separated_data(trainingData, testData, fraction);

	// get network parameters and create network
	mlp_params params = get_default_params();
	mlp_kernel_t net(NUMFEATSYL, params.num1layer, params.num2layer, 4, params.alpha, params.momentum);

	// train the network
	train_network(net, trainingData);

	std::cout << "[train] trained MLP successfully" << std::endl;
	std::cout << "\tL1=" << params.num1layer << "\t\tL2=" << params.num2layer << "\t\talpha=" << params.alpha << "\t\tmomentum=" << params.momentum << std::endl;

	// predict targets
	predict_targets(net, trainingData);
	save_to_file(trainingData, m_params.at("output_training"));
	predict_targets(net, testData);
	save_to_file(testData, m_params.at("output_test"));

	std::cout << "[predict] prediction finished successfully" << std::endl;
	std::cout << "\tpredicted training targets: " << trainingData.samples.size() << std::endl;
	std::cout << "\tpredicted test targets: " << testData.samples.size() << std::endl;
}

void MultiLayerPerceptron::cross_validation()
{
	mlp_params params = get_default_params();

	std::cout << "[cross_validation] cross-validation finished successfully" << std::endl;
	std::cout << "\tCV-error: " << cross_validate_regression_network(params, m_data, 5);
}

void MultiLayerPerceptron::model_selection()
{
	std::cout << "[model_selection] start model-selection..." << std::endl;

	// perform model selection
	mlp_params optParams = select_model(m_data);

	std::cout << "[model_selection] model-selection finished successfully" << std::endl;

	// print out results
	std::cout << "\tL1=" << optParams.num1layer << "\t\tL2=" << optParams.num2layer << "\t\talpha=" << optParams.alpha << "\t\tmomentum=" << optParams.momentum << std::endl;

	// store results
	m_params.at("L1_number") = std::to_string((unsigned)optParams.num1layer);
	m_params.at("L2_number") = std::to_string((unsigned)optParams.num2layer);
	m_params.at("alpha") = std::to_string(optParams.alpha);
	m_params.at("momentum") = std::to_string(optParams.momentum);
}
