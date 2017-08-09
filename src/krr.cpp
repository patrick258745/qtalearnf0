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

	std::cout << "[train] trained KRR successfully" << std::endl;

	return model;
}

void KernelRidgeRegression::predict_targets(const krr_model& model, const training_s& data, const std::string& targetFile)
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

void KernelRidgeRegression::train()
{
	// train the model
	krr_model model = get_trained_model(m_data);

	// store model
	dlib::serialize(m_params.at("slope_model")) << model.slopePredictor;
	dlib::serialize(m_params.at("offset_model")) << model.offsetPredictor;
	dlib::serialize(m_params.at("strength_model")) << model.strengthPredictor;
	dlib::serialize(m_params.at("duration_model")) << model.durationPredictor;

	std::cout << "[train] training finished successfully. Following parameter used:" << std::endl;
	std::cout << "\tgamma=" << 1.0/dlib::compute_mean_squared_distance(m_data.samples) << std::endl;
}

void KernelRidgeRegression::predict(double fraction)
{
	// initialize
	training_s trainingData, testData;
	get_separated_data(trainingData, testData, fraction);

	// train the model
	krr_model model = get_trained_model(trainingData);

	std::cout << "[train] training finished successfully. Following parameter used:" << std::endl;
	std::cout << "\tgamma=" << 1.0/dlib::compute_mean_squared_distance(m_data.samples) << std::endl;

	// predict targets
	predict_targets(model, trainingData, m_params.at("output_training"));
	predict_targets(model, testData, m_params.at("output_test"));

	std::cout << "[predict] prediction finished successfully" << std::endl;
	std::cout << "\tpredicted training targets: " << trainingData.samples.size() << std::endl;
	std::cout << "\tpredicted test targets: " << testData.samples.size() << std::endl;
}

void KernelRidgeRegression::cross_validation()
{
	const double gamma = 1.0/dlib::compute_mean_squared_distance(m_data.samples);

	// get trainer depending on parameters
	krr_trainer_t slopeTrainer; slopeTrainer.set_kernel(krr_kernel_t(gamma));
	krr_trainer_t offsetTrainer; offsetTrainer.set_kernel(krr_kernel_t(gamma));
	krr_trainer_t strengthTrainer; strengthTrainer.set_kernel(krr_kernel_t(gamma));
	krr_trainer_t durationTrainer; durationTrainer.set_kernel(krr_kernel_t(gamma));

	std::cout << "[cross_validation] cross-validation finished successfully" << std::endl;

	// do cross validation and print results
	std::cout << "\tCV-error (slope):    " << dlib::trans(cross_validate_regression_trainer(slopeTrainer, m_data.samples, m_data.slopes,10));
	std::cout << "\tCV-error (offset):   " << dlib::trans(cross_validate_regression_trainer(offsetTrainer, m_data.samples, m_data.offsets,10));
	std::cout << "\tCV-error (strength): " << dlib::trans(cross_validate_regression_trainer(strengthTrainer, m_data.samples, m_data.strengths,10));
	std::cout << "\tCV-error (duration): " << dlib::trans(cross_validate_regression_trainer(durationTrainer, m_data.samples, m_data.durations,10));
}
