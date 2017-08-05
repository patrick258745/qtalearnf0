#include "training.h"

// *************** MLPCVERROR ***************

double MlpCvError::operator() (const la_col_vec& arg) const
{
	mlp_params params ({(unsigned)arg(0), (unsigned)arg(1), arg(2), 0.8});
	la_col_vec mse = MultiLayerPerceptron::cross_validation(params, m_data, 10);

	return MultiLayerPerceptron::measure_error(mse);
}

// *************** MULTILAYERPERCEPTRON ***************

double MultiLayerPerceptron::measure_error(const la_col_vec& mse)
{
	std::cout << trans(mse);
	return sum(squared(mse))/4;
}

mlp_params MultiLayerPerceptron::get_default_params() const
{
	// get parameters
	unsigned numFirstLayer = (unsigned)get_value("num1layer");
	unsigned numSecondLayer = (unsigned)get_value("num2layer");
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

dlib::matrix<double> MultiLayerPerceptron::get_grid(const la_col_vec& lowerBound, const la_col_vec& upperBound, const unsigned& numPerDim)
{
	// get one dimension
	dlib::matrix<double> num1Space 	= dlib::linspace(upperBound(0), lowerBound(0), numPerDim);
	dlib::matrix<double> num2Space 	= dlib::linspace(upperBound(1), lowerBound(1), numPerDim);
	dlib::matrix<double> alphaSpace = dlib::logspace(log10(upperBound(2)), log10(lowerBound(2)), numPerDim);

	// create grid points and return
	dlib::matrix<double> tmpGrid = dlib::cartesian_product(num1Space, num2Space);
	return dlib::cartesian_product(tmpGrid, alphaSpace);
}

void MultiLayerPerceptron::train (mlp_kernel_t& network, const training_s& data)
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
	}
}

la_col_vec MultiLayerPerceptron::predict (mlp_kernel_t& network, training_s& testData)
{
	// result container
	std::vector<la_col_vec> predictedTargets;
	la_col_vec sse(4);	// sum of squared errors
	sse = 0,0,0,0;

	// evaluate predictions
	for (unsigned i=0; i<testData.samples.size(); ++i)
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

	return sse;
}

la_col_vec MultiLayerPerceptron::cross_validation(mlp_params params, const training_s& data, unsigned folds)
{
	// results
	la_col_vec sse(4);
	sse = 0,0,0,0;

	// fold borders
	dlib::matrix<double> borders = dlib::linspace(0, data.samples.size(), folds+1);

	for (unsigned i=0; i<folds; ++i)
	{
		// data for training
		training_s dataTraining (data), dataTest;

		// determine borders
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
		train(net, dataTraining);
		sse += predict(net, dataTest);
	}

	la_col_vec mse = sse/data.samples.size();

	return mse;
}

la_col_vec MultiLayerPerceptron::model_selection(const training_s& data)
{
	// define parameter search space {num1layer, num2layer, alpha}
	la_col_vec lowerBound(3), upperBound(3);
	lowerBound = 2, 0, 1e-2;
	upperBound = 64, 8, 1e0;

	// store optimal parameters
	mlp_params bestParams;
	double bestResult (1e6);

	// create grid for grid search
	dlib::matrix<double> grid = get_grid(lowerBound, upperBound, 10);

	// loop over grid
	dlib::mutex mu;
	dlib::parallel_for(0, grid.nc(), [&](long col) //for(long col = 0; col < grid.nc(); ++col)
	{
        // do cross validation and then check if the results are the best
		mlp_params params ({(unsigned)grid(0, col),(unsigned)grid(1, col),grid(2, col),0.8});
    	la_col_vec mseAll = cross_validation(params, data, 10);
        double result = measure_error(mseAll);

        // save the best results
        dlib::auto_mutex lock(mu);
        if (result < bestResult)
        {
        	bestParams = params;
            bestResult = result;
        }

		// DEBUG message
		#ifdef DEBUG_MSG
        std::cout << "\t[model_selection] " << col << ": mse(" << params.num1layer << "," << params.num2layer << "," << params.alpha << ")\t\t= " << result << std::endl;
		#endif
    });

    // optimization with BOBYQA
	la_col_vec arg(3);
	arg = bestParams.num1layer,bestParams.num2layer,bestParams.alpha;

    try
    {
		// DEBUG message
		#ifdef DEBUG_MSG
		std::cout << "\t[model_selection] Grid search completed. Initialize BOBYQA ... start(" << dlib::trans(arg) << ") mse = " << bestResult  << std::endl;
		#endif

    	// optimization
    	dlib::find_min_bobyqa(MlpCvError (data), arg, arg.size()*2+1, lowerBound, upperBound, min(upperBound-lowerBound)/10, 1e-2, 100);
    }
	catch (dlib::bobyqa_failure& err)
	{
		// DEBUG message
		#ifdef DEBUG_MSG
		std::cout << "\t[model_selection] WARNING: no convergence during optimization" << std::endl << err.info << std::endl;
		#endif
	}

    return arg;
}

void MultiLayerPerceptron::train()
{
	// get network parameters and create network
	mlp_params params = get_default_params();
	mlp_kernel_t net(NUMFEATSYL, params.num1layer, params.num2layer, 4, params.alpha, params.momentum);

	// train the network
	train(net, m_data);

	// save the trained model to file
	dlib::serialize(m_params.at("model")) << net;
}

void MultiLayerPerceptron::predict()
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
		dataTest.slopes.push_back(m_data.slopes[n]);
		dataTest.offsets.push_back(m_data.offsets[n]);
		dataTest.strengths.push_back(m_data.strengths[n]);
		dataTest.durations.push_back(m_data.durations[n]);

		// delete from training data
		dataTraining.samples.erase(dataTraining.samples.begin()+n);
		dataTraining.slopes.erase(dataTraining.slopes.begin()+n);
		dataTraining.offsets.erase(dataTraining.offsets.begin()+n);
		dataTraining.strengths.erase(dataTraining.strengths.begin()+n);
		dataTraining.durations.erase(dataTraining.durations.begin()+n);

		// get network parameters and create network
		mlp_params params = get_default_params();
		mlp_kernel_t net(NUMFEATSYL, params.num1layer, params.num2layer, 4, params.alpha, params.momentum);

		// train the network
		train(net, dataTraining);

		// prediction
		la_col_vec sse = predict(net, dataTest);

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
		std::cout << "\t[predict] (" << n << ") predicted: " << label << " " << t.m << " " << t.b << " " << t.l << "\tmse: " << dlib::trans(sse)/1;
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

void MultiLayerPerceptron::cross_validation()
{
	mlp_params params = get_default_params();
	std::cout << trans(cross_validation(params, m_data, 10));
}

void MultiLayerPerceptron::model_selection()
{
	// perform model selection
	la_col_vec optParams = model_selection(m_data);

	// print out results
	std::cout << "L1=" << optParams(0) << "\t\tL2=" << optParams(1) << "\t\talpha=" << optParams(2) << std::endl;

	// store results
	m_params.at("num1layer") = std::to_string((unsigned)optParams(0));
	m_params.at("num2layer") = std::to_string((unsigned)optParams(1));
	m_params.at("alpha") = std::to_string(optParams(2));
}
