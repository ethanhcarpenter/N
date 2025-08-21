#pragma once

#include <atomic>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <tuple>
#include <vector>
#include <string>
#include <functional>
#include <iostream>


#include "Statistics.h"
#include "Threader.h"
#include "VisualUpdate.h"
#include "StopWatch.h"

class Visualiser;
class NeuralNetwork;

class NetworkVisualiserInterface{
private:
	std::atomic <std::shared_ptr<Statistics>*> stats;
	std::atomic<std::shared_ptr<std::shared_mutex>*> statsMutex;
	Threader threader;
public:
	NetworkVisualiserInterface();
	bool isVisualiserSetup();
	std::tuple<std::string,float,std::vector<int>> getInitialInputs();
	void initialiseWorkerThread();
	void joinAllThreads();
	void updateConnections(std::vector<VisualUpdate> updates, bool firstPass);
	void initialiseVisualiseThread();
	std::tuple<std::string, float, int> getParametersFromVisualiser();
	int startTraining(int inputSize);
	bool isNeuralNetworkRunning();
	void updateNeuralNetworkParametersFromVisualiser(std::function<void()> updateParameters);
	void updateInputStatistic();
	void updateWeightStatistic(std::vector<std::vector<std::vector<float>>> weights);
	void updateEpochStatistic(StopWatch& stopwatch);
	void updateAccuracyStatistic(float accuracy);
	int getCurrentInput();
	int getCurrentEpoch();
	int getTotalInputs();
	float getLastEpochTime();
	float getAverageEpochTime();
	float getTestAccuracy();
	void setVisualiser(std::shared_ptr<Visualiser> v);
	void setMainNeuralNetwork(std::shared_ptr<NeuralNetwork> n);
	void setInputDataManager(std::shared_ptr<InputDataManager> m);
	std::shared_ptr<InputDataManager> getInputDataManager();
	void invertNeuralNetworkRunning();
	void updateStats( std::vector<int> ls, std::tuple<int, float> ni, std::string at,bool cls);

};