// Copyright © 2020 Dmitriy Khaustov. All rights reserved.
//
// Author: Dmitriy Khaustov aka xDimon
// Contacts: khaustov.dm@gmail.com
// File created on: 2020.02.20

// main.cpp

#include <iostream>
#include <thread>
#include <vector>
#include "AddressManager.hpp"
#include "ConnectionManager.hpp"
#include "Worker.hpp"
#include "WorkerManager.hpp"

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		std::cerr
			<< "Usage:\n"
			<< "\t" << argv[0] << " <filepath>\n"
			<< "\t\tfilepart - path of file containing addresses"
			<< std::endl;
		exit(EXIT_FAILURE);
	}

	AddressManager::load(argv[1]);

	AddressManager::start();

	WorkerManager::start();

	ConnectionManager::dispatch();
}
