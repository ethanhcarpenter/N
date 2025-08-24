#include "Threader.h"


#pragma region Initialise
Threader::Threader() :sideVisualiserThreadRunning(true) {};
void Threader::joinAllThreads() {
	setSideVisualiserThreadRunning(false);            
	getCV().notify_all();               

	if (getMainVisualiserThread().joinable())                
		getMainVisualiserThread().join();
	if (getSideVisualiserThread().joinable())                
		getSideVisualiserThread().join();
}
#pragma endregion



#pragma region Worker
void Threader::visualiserWorker() {
	while (true) {
		std::function<void()> task;
		{
			std::unique_lock<std::mutex> lock(queueMutex);
			cv.wait(lock, [&] { return !sideVisualiserThreadRunning || !sideVisualiserThreadFunction.empty(); });
			if (!sideVisualiserThreadRunning && sideVisualiserThreadFunction.empty()) { break; }
			task = std::move(sideVisualiserThreadFunction.front());
			sideVisualiserThreadFunction.pop();
		}
		task();
	}
}
#pragma endregion



#pragma region Get
std::thread& Threader::getMainVisualiserThread() { return mainVisualiserThread; }
std::thread& Threader::getSideVisualiserThread() { return sideVisualiserThread; }
std::mutex& Threader::getQueueMutex() { return queueMutex; }
std::queue<std::function<void()>>& Threader::getQueue() { return sideVisualiserThreadFunction; }
std::condition_variable& Threader::getCV() { return cv; }
#pragma endregion



#pragma region Set
void Threader::setSideVisualiserThreadRunning(bool v) { sideVisualiserThreadRunning = v; }
#pragma endregion
