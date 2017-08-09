#include "training.h"

// *************** LINEARRIDGEREGRESSION ***************

lrr_model LinearRidgeRegression::get_trained_model (const training_s& data) const
{
	// get trainer depending on parameters
	lrr_trainer_t slopeTrainer;
	lrr_trainer_t offsetTrainer;
	lrr_trainer_t strengthTrainer;
	lrr_trainer_t durationTrainer;

	// do the training
	lrr_model model;
	model.slopePredictor = slopeTrainer.train(data.samples, m_data.slopes);
	model.offsetPredictor = offsetTrainer.train(data.samples, m_data.offsets);
	model.strengthPredictor = strengthTrainer.train(data.samples, m_data.strengths);
	model.durationPredictor = durationTrainer.train(data.samples, m_data.durations);

	std::cout << "[train] trained LRR successfully" << std::endl;

	return model;
}

void LinearRidgeRegression::predict_targets(const lrr_model& model, const training_s& data, const std::string& targetFile)
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

void LinearRidgeRegression::train()
{
	// train the model
	lrr_model model = get_trained_model(m_data);

	// store model
	dlib::serialize(m_params.at("slope_model")) << model.slopePredictor;
	dlib::serialize(m_params.at("offset_model")) << model.offsetPredictor;
	dlib::serialize(m_params.at("strength_model")) << model.strengthPredictor;
	dlib::serialize(m_params.at("duration_model")) << model.durationPredictor;

	std::cout << "[train] training finished successfully" << std::endl;
}

void LinearRidgeRegression::predict(double fraction)
{
	// initialize
	training_s trainingData, testData;
	get_separated_data(trainingData, testData, fraction);

	// train the model
	lrr_model model = get_trained_model(trainingData);

	// predict targets
	predict_targets(model, trainingData, m_params.at("output_training"));
	predict_targets(model, testData, m_params.at("output_test"));

	std::cout << "[predict] prediction finished successfully" << std::endl;
	std::cout << "\tpredicted training targets: " << trainingData.samples.size() << std::endl;
	std::cout << "\tpredicted test targets: " << testData.samples.size() << std::endl;
}

void LinearRidgeRegression::cross_validation()
{
	// get trainer depending on parameters
	lrr_trainer_t slopeTrainer;
	lrr_trainer_t offsetTrainer;
	lrr_trainer_t strengthTrainer;
	lrr_trainer_t durationTrainer;

	std::cout << "[cross_validation] cross-validation finished successfully" << std::endl;

	// do cross validation and print results
	std::cout << "\tCV-error (slope):    " << dlib::trans(cross_validate_regression_trainer(slopeTrainer, m_data.samples, m_data.slopes,10));
	std::cout << "\tCV-error (offset):   " << dlib::trans(cross_validate_regression_trainer(offsetTrainer, m_data.samples, m_data.offsets,10));
	std::cout << "\tCV-error (strength): " << dlib::trans(cross_validate_regression_trainer(strengthTrainer, m_data.samples, m_data.strengths,10));
	std::cout << "\tCV-error (duration): " << dlib::trans(cross_validate_regression_trainer(durationTrainer, m_data.samples, m_data.durations,10));
}
