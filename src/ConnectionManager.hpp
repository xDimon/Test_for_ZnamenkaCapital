// Copyright © 2020 Dmitriy Khaustov. All rights reserved.
//
// Author: Dmitriy Khaustov aka xDimon
// Contacts: khaustov.dm@gmail.com
// File created on: 2020.02.20

// ConnectionManager.hpp

#pragma once

#include <sys/epoll.h>
#include <mutex>
#include <map>
#include <queue>
#include <condition_variable>
#include "Address.hpp"

class ConnectionManager final
{
	static const int poolSize = 1u<<14u;

	int _epfd = -1;
	epoll_event _epev[poolSize]{};

	std::mutex _mutex;

	std::map<const Address *, const std::shared_ptr<Address>> _all;

	std::mutex _readyMutex;
	std::condition_variable _conditionVariable;
	std::queue<std::shared_ptr<Address>> _ready;

public:
	ConnectionManager(const ConnectionManager&) = delete;
	ConnectionManager& operator=(ConnectionManager const&) = delete;
	ConnectionManager(ConnectionManager&& tmp) noexcept = delete;
	ConnectionManager& operator=(ConnectionManager&& tmp) noexcept = delete;

private:
	ConnectionManager();
	~ConnectionManager();

	static ConnectionManager instance;
	static ConnectionManager &getInstance()
	{
		return instance;
	}

public:
	static void check(std::shared_ptr<Address>&& addr);

	static void dispatch();

	static std::shared_ptr<Address> getReady();
};


