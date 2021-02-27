#include <iostream>

#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>

#include <chrono>
#include <vector>
#include <map>
#include <array>

static constexpr uint32_t MaxThreads = 16;

std::array<std::atomic_bool, MaxThreads> readStates;
std::array<std::atomic_bool, MaxThreads> processedStates;
std::array<std::atomic_bool, MaxThreads> destroyStates;
std::array<std::mutex, MaxThreads> mutexList;
std::array<std::condition_variable, MaxThreads> conditionVariableList;

void worker_thread(int index)
{
	while(true) {
		std::unique_lock<std::mutex> lk(mutexList[index]);
		conditionVariableList[index].wait(lk, [index]{ return (bool)readStates[index];});

		if(destroyStates[index]) {
			break;
		}

		std::cout << "Worker thread is processing data\n";

		processedStates[index] = true;
		readStates[index] = false;

		lk.unlock();
		conditionVariableList[index].notify_one();
	}

	std::cout << "Thread end " << index << "\n";
}

inline int inline_run()
{
	std::vector<std::thread> threads;
	threads.reserve(MaxThreads);

	for (int i = 0; i < MaxThreads; ++i) {
		threads.emplace_back([i]() -> void {worker_thread(i);});

		destroyStates[i] = false;
	}

	std::cout << "Hello, World!" << std::endl;

	auto tick = [](int a) -> void {
		std::cout << "tick " << a << std::endl;
		// send data to the worker thread
		for (int i = 0; i < MaxThreads; ++i) {
			std::lock_guard<std::mutex> lk(mutexList[i]);
			readStates[i] = true;
			std::cout << "main() signals data ready for processing\n";

			conditionVariableList[i].notify_one();
		}

		// wait for the worker
		for (int i = 0; i < MaxThreads; ++i) {
			std::unique_lock<std::mutex> lk(mutexList[i]);
			conditionVariableList[i].wait(lk, [i]{return (bool)processedStates[i];});
		}
	};

	for (int i = 0; i < 2000; ++i) {
		tick(i);
	}

	std::cout << "end" << std::endl;

	for (int i = 0; i < MaxThreads; ++i) {
		destroyStates[i] = true;

		std::lock_guard<std::mutex> lk(mutexList[i]);
		readStates[i] = true;
		std::cout << "main() signals data ready for processing\n";

		conditionVariableList[i].notify_one();
	}

	for (int i = 0; i < MaxThreads; ++i) {
		if(threads[i].joinable())
			threads[i].join();
	}

	return 0;
}
