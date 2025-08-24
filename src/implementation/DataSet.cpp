#include "DataSet.h"



void DataSet::setNumberOfInputs(int noi) {
	numberOfInputs = noi;
}

void DataSet::generateImageDataFromTextFile(const char* path, int amount) {
	std::srand(static_cast<int>(std::time(0)));
	inputs.clear();
	outputs.clear();
	std::ifstream file(path);
	std::string line;
	int amountAdded = 0;
	while (amountAdded < amount && getline(file, line)) {
		int label;
		std::vector<float> pixels;
		float val;
		std::istringstream iss(line);
		iss >> label;
		while (iss >> val) {
			pixels.push_back(val);
		}
		inputs.push_back(pixels);
		std::vector<float> target(10, 0.0f);
		target[label] = 1.0f;
		outputs.push_back(target);
		amountAdded++;
	}
	file.close();
}
void DataSet::generateImageDataFromTextFileRandom(const char* path, int amount) {
	std::srand(static_cast<int>(std::time(0)));
	inputs.clear();
	outputs.clear();
	std::ifstream file(path, std::ios::binary);
	std::vector<std::streampos> offsets;
	offsets.push_back(file.tellg());
	std::string line;
	while (std::getline(file, line)) {
		offsets.push_back(file.tellg());
	}
	if (amount > (int)offsets.size() - 1) amount = static_cast<int>(offsets.size()) - 1;
	std::vector<int> indices(offsets.size() - 1);
	for (int i = 0; i < (int)indices.size(); ++i) indices[i] = i;
	std::random_device rd;
	std::mt19937 g(rd());
	std::shuffle(indices.begin(), indices.end(), g);
	indices.resize(amount);
	for (int idx : indices) {
		file.clear();
		file.seekg(offsets[idx]);

		if (std::getline(file, line)) {
			int label;
			std::vector<float> pixels;
			std::istringstream iss(line);
			iss >> label;
			float val;
			while (iss >> val) pixels.push_back(val);

			inputs.push_back(pixels);
			std::vector<float> target(10, 0.0f);
			target[label] = 1.0f;
			outputs.push_back(target);

		}
	}
	file.close();
}



std::vector<std::vector<float>>& DataSet::getInputs() {
	return inputs;
}
std::vector<std::vector<float>>& DataSet::getOutputs() {
	return outputs;
}