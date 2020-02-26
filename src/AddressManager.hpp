// Copyright © 2020 Dmitriy Khaustov. All rights reserved.
//
// Author: Dmitriy Khaustov aka xDimon
// Contacts: khaustov.dm@gmail.com
// File created on: 2020.02.20

// AddressManager.hpp

#pragma once

#include <mutex>
#include <deque>
#include <memory>
#include "Address.hpp"

class AddressManager final
{
	std::mutex _mutex;
	std::deque<std::shared_ptr<Address>> _deque;

public:
	AddressManager(AddressManager&&) noexcept = delete; // Move-constructor
	AddressManager(const AddressManager&) = delete; // Copy-constructor

	AddressManager& operator=(AddressManager&&) noexcept = delete; // Move-assignment
	AddressManager& operator=(AddressManager const&) = delete; // Copy-assignment

private:
	AddressManager() = default; // Default-constructor
	~AddressManager() = default; // Destructor

	static AddressManager _instance;
	static AddressManager& getInstance()
	{
		return _instance;
	}

public:
	static void load(const std::string& filepath);

	static void start();

	static std::shared_ptr<Address> get();

	static void put(std::shared_ptr<Address>&& addr);
};

