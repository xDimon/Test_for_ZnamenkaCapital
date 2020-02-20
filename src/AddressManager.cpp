// Copyright © 2020 Dmitriy Khaustov. All rights reserved.
//
// Author: Dmitriy Khaustov aka xDimon
// Contacts: khaustov.dm@gmail.com
// File created on: 2020.02.20

// AddressManager.cpp

#include "AddressManager.hpp"
#include <set>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "ConnectionManager.hpp"

AddressManager AddressManager::_instance;

void AddressManager::load(const std::string& filepath)
{
	auto& am = getInstance();

	std::set<std::pair<uint32_t,uint16_t>> uniq;

	std::ifstream ifs(filepath);
	if (!ifs.is_open())
	{
		throw std::runtime_error("Can't open " + filepath + ": " + strerror(errno));
	}

	size_t count = 0;
	std::string line;
	while (++count, std::getline(ifs, line))
	{
		if (line.empty()) continue;

		int64_t octet[4];
		char dot[3];
		char colon;
		int64_t port;

		std::istringstream iss(line);

		iss >> octet[0] >> dot[0] >> octet[1] >> dot[1] >> octet[2] >> dot[2] >> octet[3] >> colon >> port;
		if (!iss)
		{
			throw std::runtime_error("Unexpected end of file on line " + std::to_string(colon));
		}
		if (dot[0] != '.' || dot[1] != '.' || dot[2] != '.' || colon != ':')
		{
			throw std::runtime_error("Malformed address on line " + std::to_string(colon));
		}
		if (!std::all_of(std::begin(octet), std::end(octet), [](auto item){ return item >=0 && item <= 255; }))
		{
			throw std::runtime_error("Invalid IP on line " + std::to_string(colon));
		}
		if (port <= 0 || port > 65536)
		{
			throw std::runtime_error("Invalid port on line " + std::to_string(colon));
		}

		uint32_t ip =
			(static_cast<uint32_t>(octet[0]) << 24u) |
				(static_cast<uint32_t>(octet[1]) << 16u) |
				(static_cast<uint32_t>(octet[2]) << 8u) |
				static_cast<uint32_t>(octet[3]);

		if (auto [_, ok] = uniq.emplace(ip, port); ok)
		{
			std::lock_guard lg(am._mutex);
			am._deque.emplace_back(std::make_shared<Address>(ip, port));
		}
	}
}

void AddressManager::start()
{
	auto& am = getInstance();
	std::lock_guard lg(am._mutex);
	for (auto&& addr : am._deque)
	{
		ConnectionManager::check(std::move(addr));
	}
	am._deque.clear();
}

std::shared_ptr<Address> AddressManager::get()
{
	auto& am = getInstance();
	for(;;)
	{
		std::unique_lock lock(am._mutex);

		if (!am._conditionVariable.wait_for(
			lock,
			std::chrono::milliseconds(30),
			[&am]{
				auto r =  !am._deque.empty() &&
				am._deque.front()->timestamp() + std::chrono::seconds(1) <= std::chrono::system_clock::now();
				return r;
			}
		))
		{
			continue;
		}

		std::lock_guard lg(am._mutex);
		if (am._deque.empty())
		{
			return {};
		}

		auto addr = std::move(am._deque.front());
		am._deque.pop_front();
		return addr;
	}
}

void AddressManager::put(std::shared_ptr<Address>&& addr)
{
	auto& am = getInstance();
	std::lock_guard lg(am._mutex);
	am._deque.emplace_back(std::move(addr));
	am._conditionVariable.notify_one();
}
