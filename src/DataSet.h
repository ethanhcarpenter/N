#pragma once
#include <vector>
#include <functional>
#include <fstream>
#include <chrono>
#include <string>
#include <random>


class DataSet {
private:
	std::vector<std::vector<float>> inputs;
	std::vector<std::vector<float>> outputs;
	int numberOfInputs;
public:
	void setNumberOfInputs(int noi);
	void generateImageDataFromTextFile(const char* path, int amount);
	void generateImageDataFromTextFileRandom(const char* path, int amount);
	std::vector<std::vector<float>>& getInputs();
	std::vector<std::vector<float>>& getOutputs();
};