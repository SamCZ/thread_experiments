#pragma once

#include <iostream>
#include "WorkerThread.hpp"

inline int class_run()
{
	auto numThreads = std::thread::hardware_concurrency();
	//auto numThreads = 3;

	std::vector<std::shared_ptr<WorkerThread>> workerThreads;
	workerThreads.resize(numThreads);

	for (int i = 0; i < numThreads; ++i) {
		workerThreads[i] = std::make_shared<WorkerThread>([]() -> void {
			std::cout << "work\n";
		});
	}

	// Enable thread processing
	for (int i = 0; i < numThreads; ++i) {
		workerThreads[i]->Trigger();
	}

	// Wait for all thread to finish working
	for (int i = 0; i < numThreads; ++i) {
		workerThreads[i]->WaitFor();
	}

	std::cout << "end" << std::endl;

	// Destroy all threads
	for (int i = 0; i < numThreads; ++i) {
		workerThreads[i]->MarkForDestroy();
		workerThreads[i]->Trigger();
		workerThreads[i]->Join();
	}

	workerThreads.clear();

	return 0;
}