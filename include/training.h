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
#include "types.h"

class MlaTrainer : public dlib::document_handler{
public:
	// constructors
	MlaTrainer (const std::string& trainingFile, const std::string& algorithmFile);
	void perform_task (const dlib::command_line_parser& parser);

private:
	// private member functions
	void read_samples (const std::string& sampleFile);
	void scale_samples (const double& lower, const double& upper);
	void normalize_samples ();

	// data members
	sample_v 		m_samples;
	qta_target_v	m_targets;
	algorithm_m 	m_algorithmParams;

	// document_handler functions
    virtual void start_document () {};
    virtual void end_document () {};
    virtual void start_element (const unsigned long line_number, const std::string& name, const dlib::attribute_list& atts);
    virtual void end_element (const unsigned long line_number, const std::string& name) {};
    virtual void characters (const std::string& data) {};
    virtual void processing_instruction (const unsigned long line_number, const std::string& target, const std::string& data) {};
};

class MlAlgorithm {
public:
	// public member functions
	MlAlgorithm (const sample_v& samples, const qta_target_v& targets, const algorithm_m& params);
	virtual ~MlAlgorithm() {};
	void perform_task (const dlib::command_line_parser& parser);

protected:
	virtual void train()=0;
	virtual void predict()=0;
	virtual void cross_validation()=0;
	virtual void model_selection()=0;
	double get_value(std::string param);

	sample_v 			m_samples;
	training_target_s	m_targets;
	algorithm_m 		m_params;
};

class SupportVectorRegression : public MlAlgorithm {
public:
	// constructor, destructor
	SupportVectorRegression (const sample_v& samples, const qta_target_v& targets, const algorithm_m& params) : MlAlgorithm(samples,targets,params) {};
	~SupportVectorRegression() {};

	// public member functions
	void train () override;
	void predict () override;
	void cross_validation () override;
	void model_selection () override;

	// additional
	static svr_trainer_t build_trainer(const svr_params& params);
	static svr_model_t train(const svr_trainer_t& trainer, const sample_v& samples, const std::vector<double>& targets);
	static la_col_vec cross_validation(const svr_trainer_t& trainer, const sample_v& samples, const std::vector<double>& targets);
	static la_col_vec model_selection(const sample_v& samples, const std::vector<double>& targets);
	static dlib::matrix<double> get_grid(const la_col_vec& lowerBound, const la_col_vec& upperBound, const unsigned& numPerDim);
};

class SvrCvError{
public:
	SvrCvError (const sample_v& samples, const std::vector<double> targets) : m_samples(samples), m_targets(targets) {}
    double operator() (const la_col_vec& arg) const;

private:
    const sample_v& m_samples;
    const std::vector<double>& m_targets;
};

class MultiLayerPerceptron : public MlAlgorithm {
public:
	// constructor, destructor
	MultiLayerPerceptron (const sample_v& samples, const qta_target_v& targets, const algorithm_m& params) : MlAlgorithm(samples,targets,params) {};
	~MultiLayerPerceptron() {};

private:
	// public member functions
	void train () override {};
	void predict () override {};
	void cross_validation () override {};
	void model_selection () override {};

};

#endif /* TRAINING_H_ */
