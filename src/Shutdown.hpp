// Copyright © 2020 Dmitriy Khaustov. All rights reserved.
//
// Author: Dmitriy Khaustov aka xDimon
// Contacts: khaustov.dm@gmail.com
// File created on: 2020.02.20

// Shutdown.hpp

#pragma once

#include <atomic>

extern std::atomic_bool _shutingdown;

bool isShutingdown();
void Shutdown();
