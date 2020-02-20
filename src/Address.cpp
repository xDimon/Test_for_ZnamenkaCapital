// Copyright © 2020 Dmitriy Khaustov. All rights reserved.
//
// Author: Dmitriy Khaustov aka xDimon
// Contacts: khaustov.dm@gmail.com
// File created on: 2020.02.20

// Address.cpp

#include "Address.hpp"
#include <arpa/inet.h>
#include <iostream>
#include <unistd.h>
#include <iomanip>

void Address::report()
{
	char ip[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &_sockaddr.sin_addr.s_addr, ip, sizeof(ip));
	uint16_t port = htobe16(_sockaddr.sin_port);

	int result;
	socklen_t result_len = sizeof(result);
	int getsockoptResult = getsockopt(sock, SOL_SOCKET, SO_ERROR, &result, &result_len);
	if (getsockoptResult == 0 && result == 0)
	{
//		std::ostringstream oss;
//		oss << ip << ':' << port << " - " << sock << " OK\n";
//		std::cout << oss.str();
//		std::cout.flush();

		close(sock);
		sock = -1;
		_prevStatus = _currStatus;
		_currStatus = Up;
		_ts = std::chrono::system_clock::now();
	}
	else
	{
//		std::ostringstream oss;
//		oss << ip << ':' << port << " - " << sock << " ERR (gso=" << getsockoptResult << ", res=" << result << ")\n";
//		std::cout << oss.str();
//		std::cout.flush();

		close(sock);
		sock = -1;
		_prevStatus = _currStatus;
		_currStatus = Down;
	}

	if (_prevStatus != _currStatus)
	{
		auto ts = std::chrono::system_clock::to_time_t(_ts);
		auto tm = std::localtime(&ts);
		auto ms_ = _ts.time_since_epoch() - std::chrono::seconds(ts);
		auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(ms_).count();

		char ip[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &_sockaddr.sin_addr.s_addr, ip, sizeof(ip));
		uint16_t port = htobe16(_sockaddr.sin_port);

		std::ostringstream oss;
		oss << std::put_time(tm, "%d-%m-%Y %X") << '.' << std::setw(3) << std::setfill('0') << ms << ", "
			<< ip << ':' << port << ' '
			<< ((_currStatus == Address::Status::Down) ? "down" :
			    (_currStatus == Address::Status::Up) ? "up" : "unknown")
			<< '\n';
		std::cout << oss.str();
		std::cout.flush();
	}

	_ts = std::chrono::system_clock::now();
}
