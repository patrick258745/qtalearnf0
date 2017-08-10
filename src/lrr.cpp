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

	return model;
}

void LinearRidgeRegression::predict_targets(const lrr_model& model, training_s& data) const
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

void LinearRidgeRegression::train()
{
	// train the model
	lrr_model model = get_trained_model(m_data);

	// store model
	dlib::serialize(m_params.at("slope_model")) << model.slopePredictor;
	dlib::serialize(m_params.at("offset_model")) << model.offsetPredictor;
	dlib::serialize(m_params.at("strength_model")) << model.strengthPredictor;
	dlib::serialize(m_params.at("duration_model")) << model.durationPredictor;

	std::cout << "[train] LRR training finished successfully" << std::endl;
}

void LinearRidgeRegression::predict()
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
		lrr_model model = get_trained_model(dataTraining);

		// predict targets
		predict_targets(model, dataTest);

		// store predicted targets
		push_back_targets(targetsPredicted, dataTest);
	}

	// rescale predicted target values
	m_scaler.min_max_rescale(targetsPredicted);

	// save target file
	save_target_file(targetsPredicted, m_params.at("output_file"));

	std::cout << "[predict] LRR prediction finished successfully" << std::endl;
}
