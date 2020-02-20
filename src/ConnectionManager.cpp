// Copyright © 2020 Dmitriy Khaustov. All rights reserved.
//
// Author: Dmitriy Khaustov aka xDimon
// Contacts: khaustov.dm@gmail.com
// File created on: 2020.02.19

// ConnectionManager.cpp

#include "ConnectionManager.hpp"
#include "Shutdown.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <iostream>

ConnectionManager ConnectionManager::instance;

ConnectionManager::ConnectionManager()
: _epfd(epoll_create(poolSize))
{
	if (_epfd == -1)
	{
		std::cerr << "Internal error: Can't open epool: " << strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}
}

ConnectionManager::~ConnectionManager()
{
	::close(_epfd);
}

void ConnectionManager::check(std::shared_ptr<Address>&& addr)
{
	auto& cm = getInstance();

	addr->sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (addr->sock == -1)
	{
		throw std::runtime_error(std::string() + "Can't create socket" + strerror(errno));
	}

	const int reuseaddr = 1;
	setsockopt(addr->sock, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(reuseaddr));

//	const timeval timeout{3, 0};
//	setsockopt(addr->sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
//	setsockopt(addr->sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

	int rrc = fcntl(addr->sock, F_GETFL, 0);
	fcntl(addr->sock, F_SETFL, rrc | O_NONBLOCK);

//	char ip[INET_ADDRSTRLEN];
//	inet_ntop(AF_INET, &addr->sockaddr().sin_addr.s_addr, ip, sizeof(ip));
//	uint16_t port = htobe16(addr->sockaddr().sin_port);
//
//	std::ostringstream oss;
//	oss << "\n" << ip << ':' << port << " - " << addr->sock << " CHECK\n";
//	std::cout << oss.str();
//	std::cout.flush();

	addr->trying();

	do
	{
		errno = 0;
		::connect(addr->sock, reinterpret_cast<const sockaddr*>(&addr->sockaddr()), sizeof(addr->sockaddr()));
	}
	while (errno == EINTR);
	if (errno != EINPROGRESS)
	{
		std::lock_guard lg(cm._readyMutex);
		close(addr->sock);
		addr->sock = -1;
		cm._ready.push(std::move(addr));
		cm._conditionVariable.notify_one();
	}
	else
	{
		if (std::lock_guard lg(cm._mutex); !cm._all.emplace(addr.get(), addr).second)
		{
			return;
		}

		epoll_event ev{EPOLLERR | EPOLLOUT | EPOLLONESHOT, addr.get()};

		// Включаем наблюдение
		if (epoll_ctl(cm._epfd, EPOLL_CTL_ADD, addr->sock, &ev) == -1)
		{
			throw std::runtime_error(std::string() + "Fail add socket for watching: " + strerror(errno));
		}
	}
}

void ConnectionManager::dispatch()
{
	auto& cm = getInstance();

	while (!isShutingdown())
	{
		int n = 0;

		for (;;)
		{
			n = epoll_wait(cm._epfd, cm._epev, poolSize, 5000);
			if (n == 0)
			{
				continue;
			}
			else if (n > 0 || errno != EINTR)
			{
				break;
			}
		}

		// Перебираем полученые события
		for (int i = 0; i < n; i++)
		{
			std::shared_ptr<Address> addr;
			{
				std::lock_guard lg(cm._mutex);
				// Игнорируем незарегистрированные соединения
				auto it = cm._all.find(static_cast<Address*>(cm._epev[i].data.ptr));
				if (it == cm._all.end())
				{
					continue;
				}
				addr = it->second;
				cm._all.erase(it);

				if (epoll_ctl(cm._epfd, EPOLL_CTL_DEL, addr->sock, nullptr) == -1)
				{
					throw std::runtime_error(std::string() + "Fail remove socket from watching: " + strerror(errno));
				}
			}

//			char ip[INET_ADDRSTRLEN];
//			inet_ntop(AF_INET, &addr->sockaddr().sin_addr.s_addr, ip, sizeof(ip));
//			uint16_t port = htobe16(addr->sockaddr().sin_port);
//
//			std::ostringstream oss;
//			oss << ip << ':' << port << " - " << addr->sock << " RESULT\n";
//			std::cout << oss.str();
//			std::cout.flush();

			{
				std::lock_guard lg(cm._readyMutex);
				cm._ready.push(std::move(addr));
				cm._conditionVariable.notify_one();
			}
		}
	}
}

std::shared_ptr<Address> ConnectionManager::getReady()
{
	std::shared_ptr<Address> addr;

	auto& cm = getInstance();

	while (!isShutingdown())
	{
		std::unique_lock lock(cm._readyMutex);

		if (!cm._conditionVariable.wait_for(lock, std::chrono::seconds(1), [&cm]{return !cm._ready.empty();}))
		{
			continue;
		}

		addr = std::move(cm._ready.front());
		cm._ready.pop();

		return std::move(addr);
	}

	return {};
}
