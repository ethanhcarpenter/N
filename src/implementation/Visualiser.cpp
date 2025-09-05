#include "Visualiser.h"
#include "NetworkVisualiserInterface.h"





#pragma region Visualiser-Setup
Visualiser::Visualiser(std::shared_ptr<NetworkVisualiserInterface> ni) :isSetup(false), isNNRunning(false) {
	targetMonitor = nullptr;
	networkInterface.swap(ni);
	activeTab = "console";
};
void Visualiser::setup(const char* name, int targetMonitorIndex, int windowWidth, int windowHeight) {
	if (!glfwInit()) return;
	int count;
	GLFWmonitor** monitors = glfwGetMonitors(&count);
	targetMonitor = monitors[targetMonitorIndex];
	mode = glfwGetVideoMode(targetMonitor);
	windowDimensions = std::make_tuple(
		(windowWidth == -1) ? mode->width : windowWidth,
		(windowHeight == -1) ? mode->height : windowHeight
	);
	glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	window = glfwCreateWindow(
		std::get<0>(windowDimensions),
		std::get<1>(windowDimensions),
		name,
		NULL,
		NULL
	);

	if (!window) {
		glfwTerminate();
	}
	int xpos, ypos;
	glfwGetMonitorPos(targetMonitor, &xpos, &ypos);
	glfwSetWindowPos(window, xpos, ypos);
	glfwMakeContextCurrent(window);

	IMGUI_CHECKVERSION();
	windowFlags = ImGuiWindowFlags_NoTitleBar
		| ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoCollapse
		| ImGuiWindowFlags_NoBringToFrontOnFocus;

	ImGui::CreateContext();

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 130");

	confirmedLayerSizes = false;
	usableHeight = 0;
	initialsedData = false;
	startingAnimation = true;
	usableHeight = static_cast<float>(std::get<1>(windowDimensions));
	io = ImGui::GetIO();
	fontDefault = io.Fonts->AddFontFromFileTTF("libs/fonts/cascadiaMono.ttf", 20.0f, nullptr, io.Fonts->GetGlyphRangesDefault());
	fontLarge = io.Fonts->AddFontFromFileTTF("libs/fonts/cascadiaMono.ttf", 28.0f, nullptr, io.Fonts->GetGlyphRangesDefault());
	setupPixelCanvas();
}
void Visualiser::setupPixelCanvas() {
	pixelCanvasStats.hardness = 0.0f;
	pixelCanvasStats.radius = 1;
}
void Visualiser::postSetupLogic() {
	generateNeuronPositions();
	neuronRadius = calculateNeuronRadius(usableHeight, 0.01f);
	calculateConnectionCount();
}
PixelCanvasStats Visualiser::pixelCanvasStats;
UserImageTestResults Visualiser::userImageTestResults;
const bool Visualiser::isSettingUp() { return !isSetup; }
GLFWwindow* Visualiser::getWindow() { return window; }
void Visualiser::terminate() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	networkInterface->setShouldCloseNetwork(true);
	glfwSetWindowShouldClose(window, true);
	glfwWindowShouldClose(window);
	glfwTerminate();
}
#pragma endregion



#pragma region Neurons
void Visualiser::drawNeurons() {
	glColor3f(0.0f, 0.0f, 1.0f);
	for (auto& layer : positions) {

		for (auto& neuron : layer) {
			drawCircle(neuron.first, neuron.second, neuronRadius, 64);
		}
	}
}
void Visualiser::drawCircle(float cx, float cy, float r, int num_segments) {
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(cx, cy);
	for (int i = 0; i <= num_segments; i++) {
		float theta = 2.0f * M_PI * float(i) / float(num_segments);
		float x = r * cosf(theta);
		float y = r * sinf(theta);
		glVertex2f(cx + x, cy + y);
	}
	glEnd();
}
void Visualiser::generateNeuronPositions() {
	if (layers.empty()) { return; }
	float layerSpacingX = static_cast<float>(std::get<0>(windowDimensions) / (layers.size() + 1));
	for (size_t i = 0; i < layers.size(); i++) {
		std::vector<std::pair<float, float>> layerPositions;
		float neuronSpacingY = usableHeight / (layers[i] + 1);

		for (int j = 0; j < layers[i]; j++) {
			float x = (i + 1) * layerSpacingX;
			float y = (j + 1) * neuronSpacingY;
			y += (std::get<1>(windowDimensions) - usableHeight);
			layerPositions.push_back({ x, y });
		}
		positions.push_back(layerPositions);
	}
}
const float Visualiser::calculateNeuronRadius(float height, float margin) {
	int maxNeurons = *max_element(layers.begin(), layers.end());
	float spacingY = height / (maxNeurons + 1);
	return (spacingY / 2.0f) - margin;
}
#pragma endregion



#pragma region Connections-Class

#pragma region Initalise
Connection::Connection(float fX, float fY, float tX, float tY, float w) {
	fromX = fX;
	fromY = fY;
	toX = tX;
	toY = tY;
	weight = w;
}
#pragma endregion


#pragma region Get
const float Connection::getFromX() { return fromX; }
const float Connection::getFromY() { return fromY; }
const float Connection::getToX() { return toX; }
const float Connection::getToY() { return toY; }
#pragma endregion


#pragma region Set
void Connection::setWeight(float w) { weight = w; }
const float Connection::getWeight() { return weight; }
#pragma endregion

#pragma endregion



#pragma region Connections
void Visualiser::drawConnections() {

	if (connections.size() != calculatedConnectionCount) { return; }
	for (auto& c : connections) {
		auto colour = generateColour(c.getWeight());
		float lineWidth = roundf(abs(c.getWeight()) * 10.0f);
		glLineWidth(lineWidth);
		glBegin(GL_LINES);
		glColor4f(get<0>(colour), get<1>(colour), get<2>(colour), 1.0f);
		glVertex2f(c.getFromX(), c.getFromY());
		glVertex2f(c.getToX(), c.getToY());
		glEnd();
	}
	glLineWidth(1.0f);
}
std::tuple<float, float, float, float> Visualiser::generateColour(float weight) {
	float opacity = std::min(1.0f, abs(weight));
	if (weight < 0) {
		return { 1.0f, 0.0f, 0.0f, opacity };
	}
	else {
		return { 0.0f, 1.0f, 0.0f, opacity };
	}
}
void Visualiser::calculateConnectionCount() {
	calculatedConnectionCount = 0;
	for (int i = 0; i < layers.size() - 1; i++) {
		int product = layers[i] * layers[i + 1];
		calculatedConnectionCount += product;
	}
}
std::string Visualiser::generateConnectionUID(int fromLayer, int from, int to) {
	std::string uid = "from layer:" + std::to_string(fromLayer) + " node: " + std::to_string(from) + " To layer: " + std::to_string(fromLayer + 1) + " node: " + std::to_string(to);
	return uid;
}
const float Visualiser::getConnectionWeight(int fromLayer, int from, int to) {
	std::string uid = generateConnectionUID(fromLayer, from, to);
	auto it = connectionsIndexes.find(uid);
	int index = it->second;
	return connections.at(index).getWeight();
}
void Visualiser::updateConnection(int fromLayer, int from, int to, float weight) {

	std::string uid = generateConnectionUID(fromLayer, from, to);
	auto it = connectionsIndexes.find(uid);
	if (it == connectionsIndexes.end()) {
		float fromX = positions[fromLayer][from].first;
		float fromY = positions[fromLayer][from].second;
		float toX = positions[fromLayer + 1][to].first;
		float toY = positions[fromLayer + 1][to].second;

		connections.push_back({ fromX, fromY, toX, toY, weight });
		connectionsIndexes[uid] = static_cast<int>(connections.size() - 1);
	}
	else {
		int index = it->second;
		connections.at(index).setWeight(weight);
	}
}
void Visualiser::addConnectionIndex(int fromLayer, int from, int to) {
	std::string uid = "";
	uid += std::to_string(fromLayer);
	uid += std::to_string(from);
	uid += std::to_string(to);
	connectionsIndexes[uid] = static_cast<int>(connections.size() - 1);
}
#pragma endregion



#pragma region NN-Tab
void Visualiser::drawNeuralNetwork(int winWidth, int winHeight) {
	drawImGuiBriefNNStats(winWidth, winHeight);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, winWidth, winHeight, 0, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	drawNeurons();
	drawConnections();

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}
void Visualiser::drawImGuiBriefNNStats(int winWidth, int winHeight) {
	ImGui::PushFont(fontDefault);
	ImGui::SetNextWindowPos(ImVec2(winWidth - 320.0f, winHeight - usableHeight), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(300, 280), ImGuiCond_Always);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 0.7f));

	ImGui::Begin("Training Overlay", nullptr,
		ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing);

	ImGui::TextColored(ImVec4(0.2f, 0.7f, 1.0f, 1.0f), "Epoch %d", networkInterface->getCurrentEpoch());

	float inputProgress = static_cast<float>(networkInterface->getCurrentInput()) / networkInterface->getTotalInputs();
	ImGui::Text("Input: %d / %d", networkInterface->getCurrentInput() + 1, networkInterface->getTotalInputs());
	ImGui::ProgressBar(inputProgress, ImVec2(-1, 0), "");

	ImGui::TextColored(ImVec4(0.2f, 0.7f, 1.0f, 1.0f), "LET: %.0f ms", networkInterface->getLastEpochTime());
	ImGui::TextColored(ImVec4(0.2f, 0.7f, 1.0f, 1.0f), "AET: %.0f ms", networkInterface->getAverageEpochTime());
	ImGui::TextColored(ImVec4(0.2f, 0.7f, 1.0f, 1.0f), "Accuracy: %.0f%%", networkInterface->getTestAccuracy());

	bool stopTrainingPressed = drawButton(
		ButtonStyle{
			ImVec2(ImGui::GetContentRegionAvail().x, 50),
			"Start Training",
			ImVec4(0.8f, 0.2f, 0.2f, 1.0f),
			ImVec4(0.9f, 0.3f, 0.3f, 1.0f),
			ImVec4(0.7f, 0.1f, 0.1f, 1.0f),
		},
		ButtonStyle{
			ImVec2(ImGui::GetContentRegionAvail().x, 50),
			"Stop Training",
			ImVec4(0.2f, 0.8f, 0.2f, 1.0f),
			ImVec4(0.3f, 0.9f, 0.3f, 1.0f),
			ImVec4(0.1f, 0.7f, 0.1f, 1.0f),
		}, networkInterface->isNeuralNetworkRunning());

	bool startTesting = drawButton(
		ButtonStyle{
			ImVec2(ImGui::GetContentRegionAvail().x, 50),
			"Test Network",
			ImVec4(0.8f, 0.2f, 0.2f, 1.0f),
			ImVec4(0.9f, 0.3f, 0.3f, 1.0f),
			ImVec4(0.7f, 0.1f, 0.1f, 1.0f),
		});
	if (startTesting) {
		activeTab = "drawer";
		std::string imageQaulity = networkInterface->getInputDataManager()->getImageQuality();
		int width = std::stoi(imageQaulity.substr(0, imageQaulity.size() / 2));
		int height = std::stoi(imageQaulity.substr(imageQaulity.size() / 2));
		pixelCanvas.setup(width, height);

	}
	else if (stopTrainingPressed) { toggleTraining(); }

	ImGui::End();

	ImGui::PopStyleColor();
	ImGui::PopStyleVar(2);
	ImGui::PopFont();
}
#pragma endregion



#pragma region Console-Tab
void Visualiser::drawConsole() {
	ImGui::PushFont(fontLarge);

	if (confirmedLayerSizes) { ImGui::BeginDisabled(); }
	networkInterface->getInputDataManager()->drawSpecifiedInputForm();

	ImGui::Dummy(ImVec2(0.0f, 10.0f));
	std::tuple<int, int> dataInputs = drawDataInputs();
	ImGui::Dummy(ImVec2(0.0f, 10.0f));

	std::vector<int> layerArch = drawLayerInputs();
	if (confirmedLayerSizes) { ImGui::EndDisabled(); }

	ImGui::Dummy(ImVec2(0.0f, 10.0f));

	if (isNNRunning) { ImGui::BeginDisabled(); }

	std::tuple<int, int, float> numericInputs = drawNumericInputs();

	ImGui::Dummy(ImVec2(0.0f, 10.0f));
	std::string activation = drawActivationInput();
	ImGui::Dummy(ImVec2(0.0f, 10.0f));

	bool confirmInputs = drawButton(
		ButtonStyle{
			ImVec2((ImGui::GetContentRegionAvail().x), 50),
			"Initialise Setup",
			ImVec4(0.2f, 0.8f, 0.2f, 1.0f),
			ImVec4(0.3f, 0.9f, 0.3f, 1.0f),
			ImVec4(0.1f, 0.7f, 0.1f, 1.0f),
		},
		ButtonStyle{
			ImVec2((ImGui::GetContentRegionAvail().x), 50),
			"Change Inputs",
			ImVec4(0.2f, 0.5f, 0.9f, 1.0f),
			ImVec4(0.3f, 0.6f, 1.0f, 1.0f),
			ImVec4(0.1f, 0.4f, 0.8f, 1.0f),
		}, isSetup);

	if (isNNRunning) { ImGui::EndDisabled(); }

	ImGui::Dummy(ImVec2(0.0f, 10.0f));
	ImGui::Dummy(ImVec2(0.0f, 10.0f));
	if ((networkInterface->getInputDataManager()->getIsDataCreated()) == 0) { ImGui::BeginDisabled(); }
	bool startTrainingPressed = drawButton(
		ButtonStyle{
			ImVec2(ImGui::GetContentRegionAvail().x, 50),
			"Start Training",
			ImVec4(0.8f, 0.2f, 0.2f, 1.0f),
			ImVec4(0.9f, 0.3f, 0.3f, 1.0f),
			ImVec4(0.7f, 0.1f, 0.1f, 1.0f),
		},
		ButtonStyle{
			ImVec2(ImGui::GetContentRegionAvail().x, 50),
			"Stop Training",
			ImVec4(0.2f, 0.8f, 0.2f, 1.0f),
			ImVec4(0.3f, 0.9f, 0.3f, 1.0f),
			ImVec4(0.1f, 0.7f, 0.1f, 1.0f),
		}, networkInterface->isNeuralNetworkRunning());
	if ((networkInterface->getInputDataManager()->getIsDataCreated()) == 0) { ImGui::EndDisabled(); }
	if (confirmInputs) {
		networkInterface->updateStats(layerArch, numericInputs, activation, confirmedLayerSizes);
		layers = layerArch;
		if (!confirmedLayerSizes) {
			confirmedLayerSizes = true;
			isSetup = true;
			postSetupLogic();

			if (!networkInterface->getInputDataManager()->getIsDataCreated() == 1) {
				networkInterface->createData(std::get<0>(dataInputs), std::get<1>(dataInputs));
			}
		}
	}
	if (startTrainingPressed) { toggleTraining(); }
	ImGui::PopFont();

}
void Visualiser::toggleTraining() {
	networkInterface->invertNeuralNetworkRunning();
	isNNRunning = !isNNRunning;
	if (isNNRunning) { activeTab = "nn"; }
	if (!isNNRunning) { activeTab = "console"; }
}
bool Visualiser::drawButton(ButtonStyle defaultStyle, ButtonStyle constantPressedStyle, bool lock) {
	bool confirm = false;
	if (lock) {
		ImGui::PushStyleColor(ImGuiCol_Button, constantPressedStyle.colour);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, constantPressedStyle.hoverColour);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, constantPressedStyle.activeColour);
		confirm = (ImGui::Button(constantPressedStyle.text.c_str(), constantPressedStyle.dimensions));
	}
	else {
		ImGui::PushStyleColor(ImGuiCol_Button, defaultStyle.colour);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, defaultStyle.hoverColour);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, defaultStyle.activeColour);
		confirm = (ImGui::Button(defaultStyle.text.c_str(), defaultStyle.dimensions));
	}

	ImGui::PopStyleColor(3);
	return confirm;
}
std::vector<int> Visualiser::drawLayerInputs() {
	ImGui::Text("Network Architecture");

	static int numLayers = 3;
	ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(100, 100, 250, 100));
	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));
	ImGui::InputInt("Number of Layers", &numLayers);
	if (numLayers < 2) numLayers = 2;
	if (numLayers > 63) numLayers = 63;
	ImGui::PopStyleColor(2);

	int inputSize = networkInterface->getInputDataManager()->getInputSize();
	int outputSize = networkInterface->getInputDataManager()->getOutputSize();

	static std::vector<int> inputtedLayers = { inputSize, 12, outputSize };
	inputtedLayers[0] = inputSize;
	int layersSize = static_cast<int>(inputtedLayers.size());
	if (layersSize > 1) inputtedLayers.back() = outputSize;

	int numHidden = numLayers - 2;
	int currentHidden = layersSize - 2;

	if (numHidden > currentHidden) {
		inputtedLayers.insert(inputtedLayers.begin() + 1, numHidden - currentHidden, 1);
	}
	else if (numHidden < currentHidden) {
		inputtedLayers.erase(inputtedLayers.begin() + 1 + numHidden, inputtedLayers.begin() + 1 + currentHidden);
	}

	float baseLayerHeight = 32.0f;
	float maxChildHeight = 300.0f;
	float childHeight = std::min(baseLayerHeight * numLayers + 40.0f, maxChildHeight);

	ImGui::BeginChild("NetworkArch", ImVec2(0, childHeight), true);
	for (int i = 0; i < numLayers; i++) {
		char label[64];
		if (i == 0) snprintf(label, sizeof(label), "Input Layer");
		else if (i == numLayers - 1) snprintf(label, sizeof(label), "Output Layer");
		else snprintf(label, sizeof(label), "Hidden Layer %d", i);
		ImGui::PushItemWidth(150);
		if (i == 0 || i == numLayers - 1) {
			ImGui::BeginDisabled();
			ImGui::InputInt(label, &inputtedLayers[i]);
			ImGui::EndDisabled();
		}
		else {
			ImGui::InputInt(label, &inputtedLayers[i]);
		}
		if (inputtedLayers[i] < 1) inputtedLayers[i] = 1;
		ImGui::PopItemWidth();
	}
	ImGui::EndChild();

	return inputtedLayers;
}
std::tuple<int, int> Visualiser::drawDataInputs() {
	ImGui::Text("Data Parameters");
	ImGui::BeginChild("DataParams", ImVec2(0, 120), true, ImGuiWindowFlags_AlwaysUseWindowPadding);
	static int trainAmount = 1000;
	static int testAmount = 100;
	ImGui::InputInt("Training Data Size", &trainAmount, 10, 100);
	ImGui::InputInt("Test Data Size", &testAmount, 10, 100);
	ImGui::Dummy(ImVec2(0.0f, 10.0f));
	ImGui::EndChild();
	return std::make_tuple(trainAmount, testAmount);
}
std::tuple<int, int, float> Visualiser::drawNumericInputs() {
	ImGui::Text("Training Parameters");
	ImGui::BeginChild("TrainingParams", ImVec2(0, 150), true, ImGuiWindowFlags_AlwaysUseWindowPadding);
	static int epochs = 100;
	static float learningRate = 0.005f;
	static int batchSize = 10;
	ImGui::InputInt("Batch Size", &batchSize, 1, 10);
	ImGui::InputInt("Maximum Epochs", &epochs, 10, 100);
	ImGui::InputFloat("Learning Rate", &learningRate, 0.001f, 0.01f, "%.5f");
	ImGui::EndChild();
	return std::make_tuple(batchSize, epochs, learningRate);
}
std::string Visualiser::drawActivationInput() {
	ImGui::Text("Activation Function");
	ImGui::BeginChild("Activation", ImVec2(0, 90), true);
	static int activationType = 0;
	const char* activations[] = { "relu", "sigmoid", "tanh", "linear" };
	ImGui::Combo("Type", &activationType, activations, IM_ARRAYSIZE(activations));
	ImGui::EndChild();
	return activations[activationType];
}
#pragma endregion



#pragma region Image-Drawer
void Visualiser::drawImageDrawer() {

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);

	ImGui::Begin("CanvasWindow", nullptr,
		ImGuiWindowFlags_NoDecoration |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoFocusOnAppearing);

	float canvasSize = viewport->Size.y;
	ImGui::BeginChild("CanvasPanel", ImVec2(canvasSize, canvasSize), false,
		ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar);

	pixelCanvas.updateStats(pixelCanvasStats);
	pixelCanvas.draw("CanvasChild", canvasSize, canvasSize);
	ImGui::EndChild();

	ImGui::SameLine();
	//ImGui::Text("Training Parameters");
	ImGui::BeginChild("RightPanel", ImVec2(viewport->Size.x - viewport->Size.y - 20.0f, viewport->Size.y - 10.0f), true);
	bool clear = drawButton(
		ButtonStyle{
			ImVec2(ImGui::GetContentRegionAvail().x, 50),
			"Clear Canvas",
			ImVec4(0.8f, 0.2f, 0.2f, 1.0f),
			ImVec4(0.9f, 0.3f, 0.3f, 1.0f),
			ImVec4(0.7f, 0.1f, 0.1f, 1.0f),
		});

	ImGui::InputInt("Brush radius", &pixelCanvasStats.radius, 1, 10);
	ImGui::InputFloat("Bursh Hardness", &pixelCanvasStats.hardness, 0.1f, 0.5f);
	bool test = drawButton(
		ButtonStyle{
			ImVec2(ImGui::GetContentRegionAvail().x, 50),
			"Test Image",
			ImVec4(0.5f, 0.2f, 0.7f, 1.0f),
			ImVec4(0.6f, 0.3f, 0.8f, 1.0f),
			ImVec4(0.4f, 0.1f, 0.6f, 1.0f)
		});
	ImGui::Dummy(ImVec2(100.0f, 0.0f));
	ImGui::SameLine();
	generateUserDrawnOutput(test, ImVec2(viewport->Size.x - viewport->Size.y - 190.0f - 20.0f, 700));
	if (clear) { pixelCanvas.resetCanvas(); }
	ImGui::EndChild();



	ImGui::End();

}
void PixelCanvas::updateStats(PixelCanvasStats& pcs) {
	stats.hardness = pcs.hardness;
	stats.radius = pcs.radius;
}
void PixelCanvas::resetCanvas() {
	for (auto& c : pixels) {
		c = IM_COL32(0, 0, 0, 255);
	}
}
void PixelCanvas::setup(int w, int h) {
	width = w;
	height = h;
	pixels = std::vector<ImU32>(static_cast<size_t>(width * height), IM_COL32(0, 0, 0, 255));

}
ImU32 PixelCanvas::blendColour(ImU32 destinationColour, ImU32 sourceColour) {
	
	float destinationA = ((destinationColour >> IM_COL32_A_SHIFT) & 0xFF) / 255.0f;
	float destinationR = ((destinationColour >> IM_COL32_R_SHIFT) & 0xFF) / 255.0f;
	float destinationG = ((destinationColour >> IM_COL32_G_SHIFT) & 0xFF) / 255.0f;
	float destinationB = ((destinationColour >> IM_COL32_B_SHIFT) & 0xFF) / 255.0f;

	float sourceA = ((sourceColour >> IM_COL32_A_SHIFT) & 0xFF) / 255.0f;
	float sourceR = ((sourceColour >> IM_COL32_R_SHIFT) & 0xFF) / 255.0f;
	float sourceG = ((sourceColour >> IM_COL32_G_SHIFT) & 0xFF) / 255.0f;
	float sourceB = ((sourceColour >> IM_COL32_B_SHIFT) & 0xFF) / 255.0f;

	float outA = sourceA + destinationA * (1.0f - sourceA);
	if (outA <= 1e-6f) return 0;
	float outR = (sourceR * sourceA + destinationR * destinationA * (1.0f - sourceA)) / outA;
	float outG = (sourceG * sourceA + destinationG * destinationA * (1.0f - sourceA)) / outA;
	float outB = (sourceB * sourceA + destinationB * destinationA * (1.0f - sourceA)) / outA;

	return IM_COL32(int(outR * 255.0f), int(outG * 255.0f), int(outB * 255.0f), int(outA * 255.0f));
}
ImU32 PixelCanvas::setAlpha(ImU32 c, int a) {
	return (c & ~IM_COL32_A_MASK) | (ImU32(a) << IM_COL32_A_SHIFT);
}
void PixelCanvas::applyBrush(int cx, int cy, bool leftClick) {
	ImU32 baseInk = IM_COL32(255, 255, 255, 20);
	ImU32 baseEraser = IM_COL32(0, 0, 0, 255);

	const int radius = stats.radius;
	const float inverseRadius = 1.0f / std::max(1, radius);

	for (int offsetY = -radius; offsetY <= radius; ++offsetY) {
		for (int offsetX = -radius; offsetX <= radius; ++offsetX) {
			int px = cx + offsetX;
			int py = cy + offsetY;
			if (px < 0 || py < 0 || px >= width || py >= height) { continue; }

			float distance = std::sqrt(float(offsetX * offsetX + offsetY * offsetY));
			if (distance > radius + 0.5f) { continue; }

			float t = 1.0f - (distance * inverseRadius);
			t = std::clamp(t, 0.0f, 1.0f);
			float k = (stats.hardness <= 0.0f) ? t : std::pow(t, 1.0f / std::max(1e-3f, 1.0f - stats.hardness));

			int idx = py * width + px;

			ImU32 base = leftClick ? baseInk : baseEraser;
			int baseA = (base >> IM_COL32_A_SHIFT) & 0xFF;
			int scaledA = (int)std::round(baseA * k);
			if (scaledA <= 0) continue;

			ImU32 src = setAlpha(base, scaledA);
			pixels[idx] = blendColour(pixels[idx], src);
		}
	}
}
void PixelCanvas::draw(const char* label, float canvasW, float canvasH) {
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::BeginChild(label, ImVec2(canvasW, canvasH), false,
		ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar);

	ImGui::SetCursorPos(ImVec2(0, 0));
	ImVec2 pos = ImGui::GetCursorScreenPos();
	float cellSize = std::min(canvasW / width, canvasH / height);

	auto* drawList = ImGui::GetWindowDrawList();

	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			int idx = y * width + x;
			ImU32 col = pixels[idx];

			ImVec2 cellMin = ImVec2(
				std::round(pos.x + x * cellSize),
				std::round(pos.y + y * cellSize)
			);
			ImVec2 cellMax = ImVec2(
				std::round(pos.x + (x + 1) * cellSize),
				std::round(pos.y + (y + 1) * cellSize)
			);

			drawList->AddRectFilled(cellMin, cellMax, col);

			if (ImGui::IsMouseHoveringRect(cellMin, cellMax)) {
				if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
					applyBrush(x, y, true);
				if (ImGui::IsMouseDown(ImGuiMouseButton_Right))
					applyBrush(x, y, false);
			}
		}
	}

	ImGui::EndChild();
	ImGui::PopStyleVar();
}
const std::vector<ImU32>& PixelCanvas::getPixels() const { return  pixels; }
int PixelCanvas::getWidth() const { return width; }
int PixelCanvas::getHeight() const { return height; }
void Visualiser::drawOutputs(std::vector<float> values, ImVec2 size) {
	ImVec2 pos = ImGui::GetCursorScreenPos();
	ImVec2 avail = size;

	auto* drawList = ImGui::GetWindowDrawList();
	float barHeight = avail.y / values.size();
	float labelOffsetX = -40.0f;


	float sum = std::accumulate(values.begin(), values.end(), 0.0f);
	if (sum < 1e-6f) sum = 1.0f;


	for (int i = 0; i < values.size(); i++) {
		float p = values[i] / sum;

		ImVec2 barMin = ImVec2(pos.x, pos.y + i * barHeight);
		ImVec2 filledMax = ImVec2(pos.x + p * avail.x, pos.y + (i + 1) * barHeight - 2);
		ImVec2 fullBarMax = ImVec2(pos.x + avail.x, pos.y + (i + 1) * barHeight - 2);

		ImU32 col = IM_COL32(
			100 + int(155 * p),
			50 + int(150 * p),
			200,
			255
		);
		drawList->AddRectFilled(barMin, filledMax, col, 4.0f);

		drawList->AddRect(barMin, fullBarMax, IM_COL32(100, 100, 100, 255));

		std::string label = std::to_string(i);
		ImVec2 labelPos = ImVec2(barMin.x + labelOffsetX, barMin.y + barHeight * 0.25f);
		drawList->AddText(fontLarge, 14.0f, labelPos, IM_COL32(255, 255, 255, 255), label.c_str());

		std::string valueLabel = std::to_string(values[i]);
		ImVec2 textSize = ImGui::CalcTextSize(valueLabel.c_str());
		ImVec2 valuePos = ImVec2(
			barMin.x + (avail.x - textSize.x) * 0.5f,
			barMin.y + (barHeight - textSize.y) * 0.5f
		);
		drawList->AddText(fontLarge, 14.0f, valuePos, IM_COL32(255, 255, 255, 255), valueLabel.c_str());
	}

	ImGui::Dummy(avail); 
}

void Visualiser::generateUserDrawnOutput(bool t, ImVec2 size) {
	if (userImageTestResults.outputs.size() > 0) {
		drawOutputs(userImageTestResults.outputs, size);
		//size_t predictedClass = std::distance(userImageTestResults.outputs.begin(), std::max_element(userImageTestResults.outputs.begin(), userImageTestResults.outputs.end()));
		
	}
	if (t) {
		NeuralNetwork n;
		std::vector<float> inputs = convertColourToNNFormat();
		auto userImageLayers = networkInterface->getLayers();
		n.visualiserFeedforward(
			inputs,
			userImageLayers,
			networkInterface->getWeights(),
			networkInterface->getActivationType()
		);
		userImageTestResults.outputs = n.getOutputs(userImageLayers);
		
		userImageTestResults.pixels = pixelCanvas.getPixels();
	}
	//std::cout << networkInterface->getTestAccuracy() << "\n";


}
std::vector<float> Visualiser::convertColourToNNFormat() {
	std::vector<float> result;
	result.reserve(pixelCanvas.getPixels().size());

	for (ImU32 c : pixelCanvas.getPixels()) {
		// Extract channels (ImU32 is ABGR)
		int r = (c >> IM_COL32_R_SHIFT) & 0xFF;
		int g = (c >> IM_COL32_G_SHIFT) & 0xFF;
		int b = (c >> IM_COL32_B_SHIFT) & 0xFF;

		// Normalize to [0,1] grayscale
		float gray = (0.299f * r + 0.587f * g + 0.114f * b) / 255.0f;

		result.push_back(gray);
	}
	return result;
}

#pragma endregion




#pragma region Main-Loop
void Visualiser::mainLoop() {

	/*glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {*/
	glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
		mods;
		scancode;
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		});

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		int winWidth, winHeight;
		glfwGetFramebufferSize(window, &winWidth, &winHeight);
		glViewport(0, 0, winWidth, winHeight);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();


		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(ImVec2(static_cast<float>(winWidth), static_cast<float>(winHeight)));
		if (ImGui::Begin("Main Tabs", nullptr, windowFlags | ImGuiWindowFlags_NoBackground)) {

			if (activeTab == "console") {
				drawConsole();
			}
			else if (activeTab == "nn") {
				drawNeuralNetwork(winWidth, winHeight);
			}
			else if (activeTab == "drawer") {
				drawImageDrawer();
			}

			ImGui::End();
		}
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}

	terminate();
}
#pragma endregion
















