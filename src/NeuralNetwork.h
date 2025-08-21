#pragma once
#include <string>
#include <vector>
#include <atomic>
#include <tuple>
#include <random>
#include <shared_mutex>


#include "VisualUpdate.h"
#include "DataSet.h"
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
	void train(DataSet& data);
	void test(DataSet& data);
	void threadVisualise();
	void updateParameters();
	void shutdown();
};


