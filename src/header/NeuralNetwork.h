#pragma once
#include <string>
#include <vector>
#include <atomic>
#include <tuple>
#include <random>
#include <shared_mutex>


#include "VisualUpdate.h"

#include "Activations.h"
#include "StopWatch.h"





static float randomWeight(const std::string& activationType, int fanIn, int fanOut);


enum NodeType {
	Input,
	Hidden,
	Output
};

class Node {
	NodeType type;
	float value;
	float bias;

public:
	Node(NodeType t);
	inline float getValue();
	void setValue(float v);
	inline float getBias();
	void setBias(float b);
	inline NodeType getType();
};

class Layer {
	std::vector<Node> nodes;

public:
	Layer(int size, NodeType type);
	std::vector<Node>& getNodes();
	int getSize();
};

class NetworkVisualiserInterface;






class Gradients {
private:
	std::vector<std::vector<std::vector<float>>> weightGradients;
	std::vector<std::vector<float>> biasGradients;
public:
	Gradients(
		std::vector<std::vector<std::vector<float>>> weights,
		std::vector<Layer>& layers
	);
	
	void apply(std::vector<std::vector<std::vector<float>>>& weights,
		std::vector<Layer>& layers,
		float learningRate, int batchSize
	);
	std::vector<std::vector<std::vector<float>>>& getWeightGradients();
	std::vector<std::vector<float>>& getBiasGradients();
	
};











class NeuralNetwork {

	std::shared_ptr<NetworkVisualiserInterface> visualiserInterface;

	std::vector<Layer> layers;
	std::vector<int> layerSizes;
	std::vector<std::vector<std::vector<float>>> weights;
	float learningRate;
	StopWatch stopwatch;
	std::string activationType;
	int maxEpochs;
	

public:
	NeuralNetwork();
	void setup(std::shared_ptr<NetworkVisualiserInterface> vi);
	void initWeights();
	void feedforward(std::vector<float>& inputVals, bool firstpass = false, bool updateWeights = false);
	void backpropagate(std::vector<float>& targetVals);
	std::vector<float> getPredictedOutput();
	std::vector<std::vector<std::vector<float>>>& getWeights();
	void train();
	void test();
	void updateParameters();
	void shutdown();
	void accumulateGradients(std::vector<float>& targetVals, Gradients& grads);
};










