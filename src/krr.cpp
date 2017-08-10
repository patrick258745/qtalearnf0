#include "training.h"

// *************** KRRERROR ***************



// *************** KERNELRIDGEREGRESSION ***************

krr_model KernelRidgeRegression::get_trained_model (const training_s& data) const
{
	const double gamma = 1.0/dlib::compute_mean_squared_distance(data.samples);

	// get trainer depending on parameters
	krr_trainer_t slopeTrainer; slopeTrainer.set_kernel(krr_kernel_t(gamma));
	krr_trainer_t offsetTrainer; offsetTrainer.set_kernel(krr_kernel_t(gamma));
	krr_trainer_t strengthTrainer; strengthTrainer.set_kernel(krr_kernel_t(gamma));
	krr_trainer_t durationTrainer; durationTrainer.set_kernel(krr_kernel_t(gamma));

	// do the training
	krr_model model;
	model.slopePredictor = slopeTrainer.train(data.samples, m_data.slopes);
	model.offsetPredictor = offsetTrainer.train(data.samples, m_data.offsets);
	model.strengthPredictor = strengthTrainer.train(data.samples, m_data.strengths);
	model.durationPredictor = durationTrainer.train(data.samples, m_data.durations);

	#ifdef DEBUG_MSG
	std::cout << "[get_trained_model] trained SVR successfully" << std::endl;
	std::cout << "\tFollowing parameters used:" << std::endl;
	std::cout << "\tall models:\t\tgamma=" << gamma << std::endl;
	#endif

	return model;
}

void KernelRidgeRegression::predict_targets(const krr_model& model, training_s& data) const
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

void KernelRidgeRegression::train()
{
	// train the model
	krr_model model = get_trained_model(m_data);

	// store model
	dlib::serialize(m_params.at("slope_model")) << model.slopePredictor;
	dlib::serialize(m_params.at("offset_model")) << model.offsetPredictor;
	dlib::serialize(m_params.at("strength_model")) << model.strengthPredictor;
	dlib::serialize(m_params.at("duration_model")) << model.durationPredictor;

	std::cout << "[train] KRR training finished successfully" << std::endl;
}

void KernelRidgeRegression::predict()
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
		krr_model model = get_trained_model(dataTraining);

		// predict targets
		predict_targets(model, dataTest);

		// store predicted targets
		push_back_targets(targetsPredicted, dataTest);
	}

	// rescale predicted target values
	m_scaler.min_max_rescale(targetsPredicted);

	// save target file
	save_target_file(targetsPredicted, m_params.at("output_file"));

	std::cout << "[predict] KRR prediction finished successfully" << std::endl;
}

