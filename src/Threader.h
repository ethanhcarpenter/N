#pragma once
#include <queue>
#include <atomic>
#include <thread>
#include <mutex>
#include <functional>




class Threader {
private:
	std::queue<std::function<void()>> sideVisualiserThreadFunction;
	std::atomic<bool> sideVisualiserThreadRunning;
	std::mutex queueMutex;
	std::condition_variable cv;
	std::thread mainVisualiserThread;
	std::thread sideVisualiserThread;
public:
	Threader();
	void visualiserWorker();
	std::thread& getMainVisualiserThread();
	std::thread& getSideVisualiserThread();
	std::mutex& getQueueMutex();
	std::queue<std::function<void()>>& getQueue();
	std::condition_variable& getCV();
	void setSideVisualiserThreadRunning(bool v);
	void joinAllThreads();

};

