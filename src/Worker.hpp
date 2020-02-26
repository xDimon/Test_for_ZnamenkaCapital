// Copyright © 2020 Dmitriy Khaustov. All rights reserved.
//
// Author: Dmitriy Khaustov aka xDimon
// Contacts: khaustov.dm@gmail.com
// File created on: 2020.02.20

// Worker.hpp

#pragma once

#include <thread>

class Worker final
{
	std::thread _thread;

public:
	Worker(Worker&&) noexcept = default; // Move-constructor
	Worker(const Worker&) = delete; // Copy-constructor
	Worker& operator=(Worker&&) noexcept = delete; // Move-assignment
	Worker& operator=(Worker const&) = delete; // Copy-assignment

	Worker(); // Default-constructor
	~Worker(); // Destructor

	static void run();
};


