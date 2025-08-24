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






struct Gradients {
	std::vector<std::vector<std::vector<float>>> weightGrads;
	std::vector<std::vector<float>> biasGrads;


	Gradients(
		std::vector<std::vector<std::vector<float>>> weights,
		std::vector<Layer>& layers
	)
	{
		weightGrads = std::vector<std::vector<std::vector<float>>>(
			weights.size());
		for (size_t l = 0; l < weights.size(); ++l) {
			weightGrads[l] = std::vector<std::vector<float>>(
				weights[l].size(), std::vector<float>(weights[l][0].size(), 0.0f));
		}

		biasGrads = std::vector<std::vector<float>>(layers.size());
		for (size_t l = 1; l < layers.size(); ++l) {
			biasGrads[l] = std::vector<float>(layers[l].getSize(), 0.0f);
		}
	}

	void apply(std::vector<std::vector<std::vector<float>>>& weights,
		std::vector<Layer>& layers,
		float learningRate, int batchSize)
	{
		for (size_t l = 0; l < weights.size(); ++l) {
			for (size_t i = 0; i < weights[l].size(); ++i) {
				for (size_t j = 0; j < weights[l][i].size(); ++j) {
					weights[l][i][j] += (learningRate / batchSize) * weightGrads[l][i][j];
				}
			}
		}

		for (size_t l = 1; l < layers.size(); ++l) {
			for (size_t j = 0; j < layers[l].getSize(); ++j) {
				layers[l].getNodes()[j].setBias(layers[l].getNodes()[j].getBias() + (learningRate / batchSize) * biasGrads[l][j]);
			}
		}
	}
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
	void accumulateGradients(std::vector<float>& targetVals, Gradients& grads) {
		std::vector<std::vector<float>> deltas(layers.size());

		auto& output = layers.back();
		deltas.back().resize(output.getSize());
		for (size_t i = 0; i < output.getSize(); ++i) {
			float error = targetVals[i] - output.getNodes()[i].getValue();
			deltas.back()[i] = error * Activations::derive(activationType, output.getNodes()[i].getValue());
		}

		for (int l = static_cast<int>(layers.size()) - 2; l > 0; --l) {
			auto& curr = layers[l].getNodes();
			auto& next = layers[l + 1].getNodes();
			deltas[l].resize(curr.size());

			for (size_t i = 0; i < curr.size(); ++i) {
				float sum = 0.0f;
				for (size_t j = 0; j < next.size(); ++j)
					sum += weights[l][i][j] * deltas[l + 1][j];
				deltas[l][i] = sum * Activations::derive(activationType, curr[i].getValue());
			}
		}

		for (size_t l = 0; l < weights.size(); ++l) {
			auto& from = layers[l].getNodes();
			auto& to = layers[l + 1].getNodes();
			for (size_t i = 0; i < from.size(); ++i) {
				for (size_t j = 0; j < to.size(); ++j) {
					grads.weightGrads[l][i][j] += deltas[l + 1][j] * from[i].getValue();
				}
			}
			for (size_t j = 0; j < to.size(); ++j) {
				grads.biasGrads[l + 1][j] += deltas[l + 1][j];
			}
		}
	}
};










