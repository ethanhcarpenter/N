#pragma once
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"



#include <string>



class InputDataManager {
private:
	std::string type;
public:
	std::string getType();
	void setType(std::string t);
	void drawSpecifiedInputForm();
	void drawImages();
	GLuint loadTexture(std::string filename);
	void TextCentered(std::string text);
};