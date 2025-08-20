#pragma once
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <atomic>
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <shared_mutex>



constexpr float M_PI = 3.14159265358979323846;

class NetworkToVisualiserInterface;

class Connection {
private:
	float fromX, fromY, toX, toY;
	float weight;
public:
	Connection(float fX, float fY, float tX, float tY, float w);
	void setWeight(float w);
	const float getWeight();
	const float getFromX();
	const float getFromY();
	const float getToX();
	const float getToY();

};
struct ButtonStyle {
	ImVec2 dimensions;
	std::string text;
	ImVec4 colour;
	ImVec4 hoverColour;
	ImVec4 activeColour;
};

class Visualiser {
private:
	std::shared_ptr<NetworkToVisualiserInterface> networkInterface;
	GLFWmonitor* targetMonitor;
	const GLFWvidmode* mode;
	GLFWwindow* window;
	std::vector<std::vector<std::pair<float, float>>> positions;
	std::vector<int> layers;
	std::pair<int, int> windowDimensions;
	int usableHeight;
	std::vector<Connection> connections;
	std::unordered_map<std::string, int>connectionsIndexes;
	float lastUpdate;
	size_t currentConnection;
	bool startingAnimation;
	bool isSetup;
	bool isNNRunning;
	float neuronRadius;
	ImGuiWindowFlags windowFlags;
	int calculatedConnectionCount;
	ImGuiIO io;
	ImFont* fontDefault;
	ImFont* fontLarge;
	bool confirmedLayerSizes;
public:
	Visualiser(std::shared_ptr<NetworkToVisualiserInterface> ni);
	void setup(const char* name, int targetMonitorIndex, int windowWidth = -1, int windowHeight = -1);
	int getTabContentHeight();
	void generateNeuronPositions(const std::vector<int>& layers, float width, float height);
	void drawCircle(float cx, float cy, float r, int num_segments);
	const float calculateNeuronRadius(float height, float margin);
	void drawNeurons();
	std::tuple<float, float, float, float> generateColour(float weight);
	void drawConnections();
	void terminate();
	const int getCurrentConnection();
	std::vector<Connection>& getConnections();
	GLFWwindow* getWindow();
	void addConnectionIndex(int fromLayer, int from, int to);
	std::string generateConnectionUID(int fromLayer, int from, int to);
	const float getConnectionWeight(int fromLayer, int from, int to, float weight);
	void mainLoop();
	const bool isSettingUp();
	void updateConnection(int fromLayer, int from, int to, float weight);
	void drawNeuralNetwork(int winWidth, int winHeight);
	void drawConsole(int winWidth, int winHeight);
	std::vector<int> drawLayerInputs();
	std::tuple<int, float> drawNumericInputs();
	std::string drawActivationInput();
	bool drawButton(ButtonStyle defaultStyle, ButtonStyle constantPressedStyle = {}, bool lock = false);
	void calculateConnectionCount();
	void updateStats(std::vector<int> ls, std::tuple<int, float> ni, std::string at);
	void postSetupLogic();
	void drawImGuiBriefNNStats(int winWidth, int winHeight);
};