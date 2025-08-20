#include "NetworkToVisualiserInterface.h"

NetworkToVisualiserInterface::NetworkToVisualiserInterface() {
	statsMutex.store(new std::shared_ptr<std::shared_mutex>(std::make_shared<std::shared_mutex>()));
	stats.store(new std::shared_ptr<Statistics>(std::make_shared<Statistics>()));

}
bool NetworkToVisualiserInterface::isVisualiserSetup() {
	std::shared_lock<std::shared_mutex> lock(*statsMutex.load()->get());
	return stats.load()->get()->getIsVisualiserSetup();
}

std::tuple<std::string, float, std::vector<int>> NetworkToVisualiserInterface::getInitialInputs() {
	std::shared_lock<std::shared_mutex> lock(*statsMutex.load()->get());
	return std::make_tuple(
		stats.load()->get()->getActivationType(),
		stats.load()->get()->getLearningRate(),
		stats.load()->get()->getLayerSizes()
	);
}

void NetworkToVisualiserInterface::initialiseWorkerThread() {
	threader.getSideVisualiserThread() = std::thread(&Threader::visualiserWorker, &threader);
}

void NetworkToVisualiserInterface::joinAllThreads() {
	threader.joinAllThreads();
}

void NetworkToVisualiserInterface::updateConnections(std::vector<VisualUpdate> updates, bool firstPass) {
	std::unique_lock<std::mutex> threadLock(threader.getQueueMutex());
	std::shared_lock<std::shared_mutex> statsLock(*statsMutex.load()->get());
	if (threader.getQueue().size() != 0) { return; }
	threader.getQueue().emplace([updates = std::move(updates), firstPass, visualiser = stats.load()->get()->getVisualiser()]() {
		if (!visualiser) return;
		for (auto& u : updates) {
			int layer = u.layer;
			int from = u.from;
			int to = u.to;
			float weight = u.weight;
			float lastweight = 1.1f;
			if (!firstPass) { lastweight = visualiser->getConnectionWeight(layer, from, to, weight); }
			visualiser->updateConnection(layer, from, to, weight);
		}
		});

	threader.getCV().notify_one();
}

void NetworkToVisualiserInterface::initialiseVisualiseThread() {
	std::shared_lock<std::shared_mutex> lock(*statsMutex.load()->get());
	threader.getMainVisualiserThread() = std::thread([&, this, visualiser = stats.load()->get()->getVisualiser()] {
		visualiser->setup("Neural Network", 0);
		visualiser->mainLoop();
		});
}

std::tuple<std::string,float,int> NetworkToVisualiserInterface::getParametersFromVisualiser() {
	std::shared_lock<std::shared_mutex> lock(*statsMutex.load()->get());
	return std::make_tuple(
		stats.load()->get()->getActivationType(),
		stats.load()->get()->getLearningRate(),
		stats.load()->get()->getMaxEpochs()
	);
}

int NetworkToVisualiserInterface::startTraining(int inputSize) {
	int maxEpochs = 0;
	if (auto mtxPtr = statsMutex.load()) {
		std::unique_lock<std::shared_mutex> lock(*mtxPtr->get());
		maxEpochs = stats.load()->get()->getMaxEpochs();
		stats.load()->get()->setTotalInputs(inputSize);
	}
	return maxEpochs;
}

bool NetworkToVisualiserInterface::isNeuralNetworkRunning() {
	std::shared_lock<std::shared_mutex> lock(*statsMutex.load()->get());
	return stats.load()->get()->getRunning();
}

void NetworkToVisualiserInterface::updateNeuralNetworkParametersFromVisualiser(std::function<void()> updateParameters) {
	if (auto mtxPtr = statsMutex.load()) {
		std::shared_lock<std::shared_mutex> lock(*statsMutex.load()->get());
		if (stats.load()->get()->getNeuralNetworkNeedsUpdating()) {
			updateParameters();
			stats.load()->get()->setNeuralNetworkNeedsUpdating(false);
		}
		
	}
}

void NetworkToVisualiserInterface::updateInputStatistic() {
	if (auto mtxPtr = statsMutex.load()) {
		std::unique_lock<std::shared_mutex> lock(*mtxPtr->get());
		stats.load()->get()->nextInput();
	}
}

void NetworkToVisualiserInterface::updateWeightStatistic(std::vector<std::vector<std::vector<float>>> weights) {
	if (auto mtxPtr = statsMutex.load()) {
		std::unique_lock<std::shared_mutex> lock(*mtxPtr->get());
		stats.load()->get()->setWeights(weights);
	}
}

void NetworkToVisualiserInterface::updateEpochStatistic(StopWatch& stopwatch) {
	if (auto mtxPtr = statsMutex.load()) {
		std::unique_lock<std::shared_mutex> lock(*mtxPtr->get());
		stats.load()->get()->nextEpoch(stopwatch);
		stats.load()->get()->resetInput();
	}
}

void NetworkToVisualiserInterface::updateAccuracyStatistic(float accuracy) {
	if (auto mtxPtr = statsMutex.load()) {
		std::unique_lock<std::shared_mutex> lock(*mtxPtr->get());
		stats.load()->get()->setTestAccuracy(accuracy);
	}
}


int NetworkToVisualiserInterface::getCurrentInput() {
	std::shared_lock<std::shared_mutex> lock(*statsMutex.load()->get());
	return stats.load()->get()->getInput();
}
int NetworkToVisualiserInterface::getCurrentEpoch() {
	std::shared_lock<std::shared_mutex> lock(*statsMutex.load()->get());
	return stats.load()->get()->getEpoch();
}
int NetworkToVisualiserInterface::getTotalInputs() {
	std::shared_lock<std::shared_mutex> lock(*statsMutex.load()->get());
	return stats.load()->get()->getTotalInputs();
}
float NetworkToVisualiserInterface::getLastEpochTime() {
	std::shared_lock<std::shared_mutex> lock(*statsMutex.load()->get());
	return stats.load()->get()->getLastEpochTime();
}
float NetworkToVisualiserInterface::getAverageEpochTime() {
	std::shared_lock<std::shared_mutex> lock(*statsMutex.load()->get());
	return stats.load()->get()->getAverageEpochTime();
}
float NetworkToVisualiserInterface::getTestAccuracy() {
	std::shared_lock<std::shared_mutex> lock(*statsMutex.load()->get());
	return stats.load()->get()->getTestAccuracy();
}

void NetworkToVisualiserInterface::invertNeuralNetworkRunning() {
	std::shared_lock<std::shared_mutex> lock(*statsMutex.load()->get());
	stats.load()->get()->setRunning(!isNeuralNetworkRunning());
}


void NetworkToVisualiserInterface::updateStats( std::vector<int> ls, std::tuple<int, float> ni, std::string at,bool cls) {
	std::shared_lock<std::shared_mutex> lock(*statsMutex.load()->get());
	auto& s = *stats.load()->get();
	s.setLayerSizes(ls);
	s.setMaxEpochs(get<0>(ni));
	s.setLearningRate(get<1>(ni));
	s.setActivationType(at);
	if (!cls) { s.setIsVisualiserSetup(true); }
	if(cls) { s.setNeuralNetworkNeedsUpdating(true); }
}

void NetworkToVisualiserInterface::setMainNeuralNetwork(std::shared_ptr<NeuralNetwork> n) {
	std::shared_lock<std::shared_mutex> lock(*statsMutex.load()->get());
	stats.load()->get()->setMainNeuralNetwork(n);

}
void NetworkToVisualiserInterface::setVisualiser(std::shared_ptr<Visualiser> v) {
	std::shared_lock<std::shared_mutex> lock(*statsMutex.load()->get());
	stats.load()->get()->setVisualiser(v);
}