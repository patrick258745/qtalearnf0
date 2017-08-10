/*
 * training.h
 *
 *  Created on: 10 July 2017
 *      Author: Patrick Schmager
 */

#ifndef TRAINING_H_
#define TRAINING_H_

#include <map>
#include <dlib/xml_parser.h>
#include <dlib/cmd_line_parser.h>
#include <dlib/threads.h>
#include <dlib/optimization.h>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include "types.h"

// ********** MLATRAINER **********

class MlaTrainer : public dlib::document_handler{
public:
	// constructors
	MlaTrainer (const std::string& trainingFile, const std::string& algorithmFile);
	void perform_task (const dlib::command_line_parser& parser);

private:
	// private member functions
	void read_samples (const std::string& sampleFile);

	// data members
	sample_v 	m_samples;
	target_v	m_targets;
	algorithm_m m_algorithmParams;

	// document_handler functions
    virtual void start_document () {};
    virtual void end_document () {};
    virtual void start_element (const unsigned long line_number, const std::string& name, const dlib::attribute_list& atts);
    virtual void end_element (const unsigned long line_number, const std::string& name) {};
    virtual void characters (const std::string& data) {};
    virtual void processing_instruction (const unsigned long line_number, const std::string& target, const std::string& data) {};
};

// ********** DATASCALER **********

class DataScaler {
public:
	// constructors
	DataScaler(const double lower, const double upper) : m_lower(lower), m_upper(upper) {};

	// public members
	void min_max_scale(training_s& trainingData);
	void min_max_rescale(target_v& data);

private:
	// private member functions
	void min_max_scale(sample_v& samples);
	scaler_s min_max_scale(std::vector<double>& data) const;
	void min_max_rescale(double& data, const scaler_s& scale) const;

	// data members
	double m_lower;
	double m_upper;
	scaler_s m_slopeScale;
	scaler_s m_offsetScale;
	scaler_s m_strengthScale;
	scaler_s m_durationScale;
};

// ********** MLALGORITHM **********

class MlAlgorithm {
public:
	// public member functions
	MlAlgorithm (const sample_v& samples, const target_v& targets, const algorithm_m& params);
	virtual ~MlAlgorithm() {};
	void perform_task (const dlib::command_line_parser& parser);

protected:
	// virtual members
	virtual void train()=0;
	virtual void predict()=0;
	virtual void model_selection()=0;

	// access parameters from algorithm file
	void write_algorithm_file(const std::string& algFile) const;
	double get_value(const std::string& param) const;

	// static helper functions
	static double loss (const double& x, const double& y);
	static void randomize_data(target_v& targets, sample_v& samples);
	static void split_data(const training_s& data, training_s& dataTraining, training_s& dataTest, const unsigned& start, const unsigned& end);

	// save predicted target file
	static void push_back_targets(target_v& targets, training_s& data);
	static void save_target_file(const target_v& targets, const std::string& targetFile);

	training_s	m_data;
	algorithm_m m_params;
	target_v	m_targets;
	DataScaler 	m_scaler;
	unsigned 	m_folds;
};

// ********** LINEARRIDGEREGRESSION **********

class LinearRidgeRegression : public MlAlgorithm {
public:
	// constructor, destructor
	LinearRidgeRegression (const sample_v& samples, const target_v& targets, const algorithm_m& params) : MlAlgorithm(samples,targets,params) {};
	~LinearRidgeRegression() {};

	// public member functions
	void train 				() override;
	void predict 			() override;
	void model_selection 	() override{};

private:
	lrr_model get_trained_model (const training_s& data) const;
	void predict_targets(const lrr_model& model, training_s& data) const;

};

// ********** KERNELRIDGEREGRESSION **********

class KernelRidgeRegression : public MlAlgorithm {
public:
	// constructor, destructor
	KernelRidgeRegression (const sample_v& samples, const target_v& targets, const algorithm_m& params) : MlAlgorithm(samples,targets,params) {};
	~KernelRidgeRegression() {};

	// public member functions
	void train 				() override;
	void predict 			() override;
	void model_selection 	() override{};

private:
	krr_model get_trained_model (const training_s& data) const;
	void predict_targets(const krr_model& model, training_s& data) const;

};

// ********** SUPPORTVECTORREGRESSION **********

class SupportVectorRegression : public MlAlgorithm {
public:
	// constructor, destructor
	SupportVectorRegression (const sample_v& samples, const target_v& targets, const algorithm_m& params) : MlAlgorithm(samples,targets,params) {};
	~SupportVectorRegression() {};

	// public member functions
	void train () override;
	void predict () override;
	void model_selection () override;

	// helper
	static svr_trainer_t build_trainer(const svr_params& params);

private:
	svr_model get_trained_model (const training_s& data) const;
	void predict_targets(const svr_model& model, training_s& data) const;
	grid_t get_grid(const la_col_vec& lowerBound, const la_col_vec& upperBound, const std::vector<unsigned>& numPerDim) const;
	void save_parameters(const svr_params& slope, const svr_params& offset, const svr_params& strength, const svr_params& duration, algorithm_m& params) const;
	svr_params select_model (const sample_v& samples, const std::vector<double>& targets,  const grid_t& grid, const la_col_vec& lowerBound, const la_col_vec& upperBound) const;
};

class SvrCvError{
public:
	SvrCvError (const sample_v& samples, const std::vector<double>& targets, const unsigned folds) : m_samples(samples), m_targets(targets), m_folds(folds) {}
    double operator() (const la_col_vec& arg) const;

private:
    unsigned m_folds;
    const sample_v& m_samples;
    const std::vector<double>& m_targets;
};

// ********** MULTILAYERPERCEPTRON **********

class MultiLayerPerceptron : public MlAlgorithm {
public:
	// constructor, destructor
	MultiLayerPerceptron (const sample_v& samples, const target_v& targets, const algorithm_m& params) : MlAlgorithm(samples,targets,params) {};
	~MultiLayerPerceptron() {};

	// public member functions
	void train () override;
	void predict () override;
	void model_selection () override;

	// helper
	static double cross_validate_regression_network(mlp_model_t& net, const sample_v& samples, const std::vector<double>& targets, const unsigned& folds);

private:
	void get_trained_model (const training_s& data, mlp_model model) const;
	void predict_targets(const mlp_model& model, training_s& data) const;
	grid_t get_grid(const la_col_vec& lowerBound, const la_col_vec& upperBound, const std::vector<unsigned>& numPerDim) const;
	void save_parameters(const mlp_params& slope, const mlp_params& offset, const mlp_params& strength, const mlp_params& duration, algorithm_m& params) const;
	mlp_params select_model (const sample_v& samples, const std::vector<double>& targets,  const grid_t& grid, const la_col_vec& lowerBound, const la_col_vec& upperBound) const;
};

class MlpCvError{
public:
	MlpCvError (const sample_v& samples, const std::vector<double>& targets, const unsigned folds) : m_samples(samples), m_targets(targets), m_folds(folds) {}
    double operator() (const la_col_vec& logArg) const;

private:
    unsigned m_folds;
    const sample_v& m_samples;
    const std::vector<double>& m_targets;
};

#endif /* TRAINING_H_ */
