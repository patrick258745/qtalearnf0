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

class DataScaler {
public:
	// constructors
	DataScaler(const double lower, const double upper) : m_lower(lower), m_upper(upper) {};

	// public members
	void min_max_scale(sample_v& samples);
	void min_max_scale(training_s& trainingData);
	void min_max_rescale(training_s& trainingData);
	void min_max_rescale(target_v& data);

private:
	// private member functions
	scaler_s min_max_scale(std::vector<double>& data) const;
	void min_max_rescale(std::vector<double>& data, const scaler_s& scale) const;
	void min_max_rescale(double& data, const scaler_s& scale) const;

	// data members
	double m_lower;
	double m_upper;
	scaler_s m_slopeScale;
	scaler_s m_offsetScale;
	scaler_s m_strengthScale;
	scaler_s m_durationScale;
};

class MlAlgorithm {
public:
	// public member functions
	MlAlgorithm (const sample_v& samples, const target_v& targets, const algorithm_m& params);
	virtual ~MlAlgorithm() {};
	void perform_task (const dlib::command_line_parser& parser);

protected:
	virtual void train()=0;
	virtual void predict()=0;
	virtual void cross_validation()=0;
	virtual void model_selection()=0;

	// protected member functions
	void get_separated_data(training_s& trainingData, training_s& testData, const double& fraction) const;
	void write_algorithm_file(const std::string& algFile) const;
	double get_value(const std::string& param) const;
	static void randomize_data(target_v& targets, sample_v& samples);
	static double loss (const double& x, const double& y);

	training_s	m_data;
	algorithm_m m_params;
	target_v	m_targets;
	DataScaler 	m_scaler;
};

class LinearRidgeRegression : public MlAlgorithm {
public:
	// constructor, destructor
	LinearRidgeRegression (const sample_v& samples, const target_v& targets, const algorithm_m& params) : MlAlgorithm(samples,targets,params) {};
	~LinearRidgeRegression() {};

	// public member functions
	void train 				() override;
	void predict 			() override;
	void cross_validation 	() override;
	void model_selection 	() override;

};

class SupportVectorRegression : public MlAlgorithm {
public:
	// constructor, destructor
	SupportVectorRegression (const sample_v& samples, const target_v& targets, const algorithm_m& params) : MlAlgorithm(samples,targets,params) {};
	~SupportVectorRegression() {};

	// public member functions
	void train 				() override;
	void predict 			() override;
	void cross_validation 	() override;
	void model_selection 	() override;

	// helper
	static svr_trainer_t 		build_trainer(const svr_params& params);
	static grid_t get_grid(const la_col_vec& lowerBound, const la_col_vec& upperBound, const std::vector<unsigned>& numPerDim);

private:
	svr_model	get_trained_model (const training_s& data) const;
	svr_params	select_model (const sample_v& samples, const std::vector<double>& targets) const;
	void 		predict_targets(const svr_model& model, const training_s& data, const std::string targetFile);
};

class SvrCvError{
public:
	SvrCvError (const sample_v& samples, const std::vector<double>& targets) : m_samples(samples), m_targets(targets) {}
    double operator() (const la_col_vec& arg) const;

private:
    const sample_v& m_samples;
    const std::vector<double>& m_targets;
};

class MultiLayerPerceptron : public MlAlgorithm {
public:
	// constructor, destructor
	MultiLayerPerceptron (const sample_v& samples, const target_v& targets, const algorithm_m& params) : MlAlgorithm(samples,targets,params) {};
	~MultiLayerPerceptron() {};

	static la_col_vec cross_validation(mlp_params params, const training_s& trainingData, unsigned folds);
	static double measure_error(const la_col_vec& mses);

private:
	// public member functions
	void train () override;
	void predict () override;
	void cross_validation () override;
	void model_selection () override;

	// helper
	mlp_params get_default_params() const;
	static dlib::matrix<double> get_grid(const la_col_vec& lowerBound, const la_col_vec& upperBound, const unsigned& numPerDim);

	static void train (mlp_kernel_t& network, const training_s& data);
	static la_col_vec predict (mlp_kernel_t& network, training_s& testData);
	static la_col_vec model_selection(const training_s& data);

};

class MlpCvError{
public:
	MlpCvError (const training_s& data) : m_data(data) {}
    double operator() (const la_col_vec& arg) const;

private:
    const training_s& m_data;
};

#endif /* TRAINING_H_ */
