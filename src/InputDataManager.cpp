#define STB_IMAGE_IMPLEMENTATION
#include "InputDataManager.h"
#include <stb_image.h>
#include <iostream>


GLuint InputDataManager::loadTexture(std::string filename) {
	int width, height, channels;
	std::string filepath = "data/images/" + filename;
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

void InputDataManager::setType(std::string t) {
	type = t;
}
std::string InputDataManager::getType() {
	return type; 
}

void InputDataManager::drawSpecifiedInputForm() {
	// Calculate total width and child height dynamically
	int numImages = 3;
	float spacing = ImGui::GetStyle().ItemSpacing.x;
	float totalWidth = ImGui::GetContentRegionAvail().x;
	float childWidth = (totalWidth - (numImages - 1) * spacing) / numImages;

	float textHeight = ImGui::GetTextLineHeight();
	float childHeight = childWidth + textHeight + 4; // child box height
	float padding = ImGui::GetStyle().FramePadding.y * 2;

	ImVec2 inputsBoxSize(totalWidth, childHeight + padding + ImGui::GetTextLineHeight() + 100); // add space for "Input Type" text

	ImGui::BeginChild("InputsBox", inputsBoxSize, true);

	ImGui::Text("Input Type");
	static int inputType = 0;
	const char* inputs[] = { "Images" };
	ImGui::Combo("Type", &inputType, inputs, IM_ARRAYSIZE(inputs));

	drawImages(); // dynamically sized images

	ImGui::EndChild();
}

void InputDataManager::drawImages() {
	ImGui::Text("Pick A Resolution");

	ImTextureID tex1 = loadTexture("0_77.png");
	ImTextureID tex2 = loadTexture("0_1414.png");
	ImTextureID tex3 = loadTexture("0_2828.png");

	float totalWidth = ImGui::GetContentRegionAvail().x; // available width for this row
	int numImages = 3;
	float spacing = ImGui::GetStyle().ItemSpacing.x; // horizontal spacing between images

	// calculate max width per image
	float maxImageWidth = (totalWidth - (numImages - 1) * spacing) / numImages;
	float imageHeight = maxImageWidth; // keep square

	ImVec2 imageSize(maxImageWidth, imageHeight);

	// for child boxes including text
	float textHeight = ImGui::GetTextLineHeight();
	ImVec2 childSize(maxImageWidth, imageHeight + textHeight + spacing);

	// Image 1
	ImGui::BeginChild("Img77", childSize, false);
	ImGui::Image((ImTextureID)(intptr_t)tex1, imageSize);
	TextCentered("7x7");
	ImGui::EndChild();

	ImGui::SameLine();

	// Image 2
	ImGui::BeginChild("Img1414", childSize, false);
	ImGui::Image((ImTextureID)(intptr_t)tex2, imageSize);
	TextCentered("14x14");
	ImGui::EndChild();

	ImGui::SameLine();

	// Image 3
	ImGui::BeginChild("Img2828", childSize, false);
	ImGui::Image((ImTextureID)(intptr_t)tex3, imageSize);
	TextCentered("28x28");
	ImGui::EndChild();
}

// Helper for centering text inside child
void InputDataManager::TextCentered(std::string text) {
	float width = ImGui::GetContentRegionAvail()[0];
	float textWidth = ImGui::CalcTextSize(text.c_str()).x;
	ImGui::SetCursorPosX((width - textWidth) * 0.5f);
	ImGui::Text("%s", text);
}
