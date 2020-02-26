// Copyright © 2020 Dmitriy Khaustov. All rights reserved.
//
// Author: Dmitriy Khaustov aka xDimon
// Contacts: khaustov.dm@gmail.com
// File created on: 2020.02.26

// WorkerManager.hpp

#pragma once


#include <vector>
#include <mutex>
#include <condition_variable>
#include "Worker.hpp"

class WorkerManager final
{
	std::vector<Worker> _workers;
	std::mutex _mutex;
	std::condition_variable _cv;

public:
	WorkerManager(const WorkerManager&) = delete;
	WorkerManager& operator=(WorkerManager const&) = delete;
	WorkerManager(WorkerManager&& tmp) noexcept = delete;
	WorkerManager& operator=(WorkerManager&& tmp) noexcept = delete;

private:
	WorkerManager() = default;
	~WorkerManager() = default;

	static WorkerManager &getInstance()
	{
		static WorkerManager instance;
		return instance;
	}

public:
	static void start();

	static void wait();
	static void notify();
};



