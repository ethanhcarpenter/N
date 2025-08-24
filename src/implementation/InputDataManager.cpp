#define STB_IMAGE_IMPLEMENTATION
#include "InputDataManager.h"
#include <stb_image.h>
#include <iostream>


GLuint InputDataManager::loadTexture(std::string filename) {
	int width, height, channels;
	std::string filepath = "data/images/" + filename;
	std::cout<< filepath <<"\n";
	unsigned char* data = stbi_load(filepath.c_str(), &width, &height, &channels, 4);
	if (!data) {
		std::cerr << "Failed to load image: " << filename << std::endl;
		return 0;
	}

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,
		0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	stbi_image_free(data);
	return textureID;
}

void InputDataManager::setup(std::vector<std::string> iq, std::string imF) {
	numberOfSampleImages = static_cast<int>(iq.size());
	currentlyHighlighted = iq[0];
	imageQuailities = iq;
	isDataCreated = false;
	currentSmapleNumber = 0;
	imageFormat = imF;
	stopwatch.start();
	inputAmount = 0;
	outputAmount = 0;
}
std::string InputDataManager::getType() {
	return type;
}

void InputDataManager::drawSpecifiedInputForm() {
	float spacing = ImGui::GetStyle().ItemSpacing.x;
	float totalWidth = ImGui::GetContentRegionAvail().x;
	float childWidth = (totalWidth - (numberOfSampleImages - 1) * spacing) / numberOfSampleImages;
	float textHeight = ImGui::GetTextLineHeight();
	float childHeight = childWidth + textHeight + 4;
	float padding = ImGui::GetStyle().FramePadding.y * 2;

	ImVec2 inputsBoxSize(totalWidth, childHeight + padding + ImGui::GetTextLineHeight() + 100);

	ImGui::BeginChild("InputsBox", inputsBoxSize, true);

	ImGui::Text("Input Type");
	static int inputType = 0;
	const char* inputs[] = { "Images" };
	ImGui::Combo("Type", &inputType, inputs, IM_ARRAYSIZE(inputs));
	type = inputs[inputType];
	if (type == "Images") {
		if (imageTextures.size() == 0) { createSampleImageTextures(); }
		drawImages();
		inputAmount = static_cast<int>(std::powf(static_cast<float>(std::stoi(getHalfString(currentlyHighlighted))), 2.0f));
		outputAmount = 10;
	}

	ImGui::EndChild();
}
void InputDataManager::createSampleImageTextures() {
	for (int i = 0; i < 10; i++) {
		std::string filenameNumber = std::to_string(i);
		for (auto& quality : imageQuailities) {
			std::string filenameQuality = quality+"/"+filenameNumber;
			std::string filename = filenameQuality + imageFormat;
			imageTextures[filenameQuality] = (ImTextureID)(intptr_t)loadTexture(filename);
		}
	}
}

void InputDataManager::drawImages() {
	ImGui::Text("Pick A Resolution");
	float totalWidth = ImGui::GetContentRegionAvail().x;
	float spacing = ImGui::GetStyle().ItemSpacing.x;
	float maxImageWidth = (totalWidth - (numberOfSampleImages - 1) * spacing) / numberOfSampleImages;
	float imageHeight = maxImageWidth;

	ImVec2 imageSize(maxImageWidth, imageHeight);

	float textHeight = ImGui::GetTextLineHeight();
	ImVec2 childSize(maxImageWidth, imageHeight + textHeight + spacing);


	for (auto& quality : imageQuailities) {
		ImGui::BeginChild(("SampleImage" + quality).c_str(), childSize, false);

		ImVec2 pos = ImGui::GetCursorScreenPos();
		ImTextureID currentImage = imageTextures[(quality + "/" + std::to_string(currentSmapleNumber))];

		ImGui::Image((ImTextureID)(intptr_t)currentImage, imageSize);
		std::string sizeString = getHalfString(quality);
		TextCentered(getHalfString(sizeString + "x" + sizeString));


		if (quality == currentlyHighlighted) {
			ImDrawList* draw_list = ImGui::GetWindowDrawList();
			float thickness = 16.0f;
			float half = thickness * 0.5f;
			draw_list->AddRect(
				ImVec2(pos.x + half, pos.y + half),
				ImVec2(pos.x + imageSize.x - half, pos.y + imageSize.y - half),
				IM_COL32(128, 0, 128, 255),
				0.0f,
				0,
				thickness
			);
		}
		ImGui::SetCursorScreenPos(pos);
		ImGui::InvisibleButton(("btn_" + quality).c_str(), imageSize);
		if (ImGui::IsItemClicked()) {
			currentlyHighlighted = quality;
		}
		ImGui::EndChild();
		ImGui::SameLine();
	}
	if (stopwatch.getElapsedMilliSeconds() > 500) {
		currentSmapleNumber = (currentSmapleNumber + 1) % 10;
		stopwatch.reset();
	}
}



std::string InputDataManager::getHalfString(std::string s) {
	size_t mid = s.size() / 2;
	return s.substr(0, mid);
}

void InputDataManager::TextCentered(std::string text) {
	float width = ImGui::GetContentRegionAvail()[0];
	float textWidth = ImGui::CalcTextSize(text.c_str()).x;
	ImGui::SetCursorPosX((width - textWidth) * 0.5f);
	ImGui::Text("%s", text.c_str());
}

void InputDataManager::createData(int trainAmount, int testAmount) {
	if (type == "Images") {
		inputAmount = static_cast<int>(std::powf(static_cast<float>(std::stoi(getHalfString(currentlyHighlighted))), 2.0f));
		trainingData.setNumberOfInputs(inputAmount);
		trainingData.generateImageDataFromTextFileRandom(("data/txtImages/data" + currentlyHighlighted + ".txt").c_str(), trainAmount);
		testData.setNumberOfInputs(inputAmount);
		testData.generateImageDataFromTextFileRandom(("data/txtImages/data" + currentlyHighlighted + ".txt").c_str(), testAmount);

	}
	isDataCreated = true;
}
bool InputDataManager::getIsDataCreated() {
	return isDataCreated;
}

int InputDataManager::getInputSize() {
	return inputAmount;
}

int InputDataManager::getOutputSize() {
	return outputAmount;
}

std::vector<std::vector<float>>& InputDataManager::getTrainingInputs() {
	return trainingData.getInputs();
}

std::vector<std::vector<float>>& InputDataManager::getTrainingOutputs() {
	return trainingData.getOutputs();
}

std::vector<std::vector<float>>& InputDataManager::getTestInputs() {
	return testData.getInputs();
}

std::vector<std::vector<float>>& InputDataManager::getTestOutputs() {
	return testData.getOutputs();
}

