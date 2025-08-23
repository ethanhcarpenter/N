#pragma once
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"


#include <string>
#include <vector>
#include <unordered_map>

#include "StopWatch.h"
#include "DataSet.h"


class InputDataManager {
private:
	bool isDataCreated;
	DataSet trainingData;
	DataSet testData;
	std::string type;
	StopWatch stopwatch;
	std::string imageFormat;
	int numberOfSampleImages;
	int inputAmount;
	int outputAmount;
	std::string currentlyHighlighted;
	int currentSmapleNumber;
	std::unordered_map<std::string, ImTextureID> imageTextures;
	std::vector<std::string> imageQuailities;
public:
	void setup(std::vector<std::string> iq, std::string imageFormat);
	std::string getType();
	void drawSpecifiedInputForm();
	void drawImages();
	bool getIsDataCreated();
	GLuint loadTexture(std::string filename);
	void TextCentered(std::string text);
	void createSampleImageTextures();
	std::string getHalfString(std::string s);
	std::vector<std::vector<float>>& getTrainingInputs();
	std::vector<std::vector<float>>& getTrainingOutputs();
	std::vector<std::vector<float>>& getTestInputs();
	std::vector<std::vector<float>>& getTestOutputs();
	void createData(int testInputAmount, int trainInputAmount);
	int getInputSize();
	int getOutputSize();
};