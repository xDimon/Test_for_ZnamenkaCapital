// Copyright © 2020 Dmitriy Khaustov. All rights reserved.
//
// Author: Dmitriy Khaustov aka xDimon
// Contacts: khaustov.dm@gmail.com
// File created on: 2020.02.26

// WorkerManager.cpp

#include "WorkerManager.hpp"

void WorkerManager::start()
{
	getInstance()._workers.resize(std::thread::hardware_concurrency());
}

void WorkerManager::wait()
{
	auto& wm = getInstance();

	std::unique_lock lock(wm._mutex);

	wm._cv.wait(lock);
}

void WorkerManager::notify()
{
	auto& wm = getInstance();
	wm._cv.notify_one();
}
