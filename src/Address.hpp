// Copyright © 2020 Dmitriy Khaustov. All rights reserved.
//
// Author: Dmitriy Khaustov aka xDimon
// Contacts: khaustov.dm@gmail.com
// File created on: 2020.02.20

// Address.hpp

#pragma once

#include <cstring>
#include <netinet/in.h>
#include <chrono>

class Address final
{
public:
	enum Status
	{
		Unknown,
		Up,
		Down
	};

	int sock = -1;

private:
	sockaddr_in _sockaddr{};
	Status _prevStatus = Status::Unknown;
	Status _currStatus = Status::Unknown;
	std::chrono::system_clock::time_point _ts = std::chrono::system_clock::now();

public:
	Address() = delete; // Default-constructor
	Address(Address&&) noexcept = default; // Move-constructor
	Address(const Address&) = delete; // Copy-constructor
	~Address() = default; // Destructor
	Address& operator=(Address const&) = delete; // Copy-assignment

	Address(uint32_t ip, uint16_t port)
	{
		memset(&_sockaddr, 0, sizeof(_sockaddr));
		_sockaddr.sin_family = AF_INET;
		_sockaddr.sin_addr.s_addr = htobe32(ip);
		_sockaddr.sin_port = htobe16(port);
	}

	[[nodiscard]] const sockaddr_in& sockaddr() const
	{
		return _sockaddr;
	}

	[[nodiscard]]
	auto timestamp() const
	{
		return _ts;
	}

	void trying()
	{
		_ts = std::chrono::system_clock::now();
	}

	void report();

};
