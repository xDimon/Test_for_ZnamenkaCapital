// Copyright © 2020 Dmitriy Khaustov. All rights reserved.
//
// Author: Dmitriy Khaustov aka xDimon
// Contacts: khaustov.dm@gmail.com
// File created on: 2020.02.20

// Worker.cpp

#include "Worker.hpp"
#include "ConnectionManager.hpp"
#include "AddressManager.hpp"
#include "Shutdown.hpp"
#include "WorkerManager.hpp"

Worker::Worker()
: _thread(run)
{
}

Worker::~Worker()
{
	if (_thread.joinable())
	{
		_thread.join();
	}
}

void Worker::run()
{
	while (!isShutingdown())
	{
		WorkerManager::wait();

		if (auto addr = ConnectionManager::getReady())
		{
			addr->report();

			AddressManager::put(std::move(addr));
		}

		if (auto addr = AddressManager::get())
		{
			ConnectionManager::check(std::move(addr));
		}
	}
}
