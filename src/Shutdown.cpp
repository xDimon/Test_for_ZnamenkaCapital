// Copyright © 2020 Dmitriy Khaustov. All rights reserved.
//
// Author: Dmitriy Khaustov aka xDimon
// Contacts: khaustov.dm@gmail.com
// File created on: 2020.02.20

// Shutdown.cpp

#include "Shutdown.hpp"

std::atomic_bool _shutingdown = false;

bool isShutingdown()
{
	return _shutingdown;
}

void Shutdown()
{
	_shutingdown = true;
}
