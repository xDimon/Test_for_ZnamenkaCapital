// Copyright © 2020 Dmitriy Khaustov. All rights reserved.
//
// Author: Dmitriy Khaustov aka xDimon
// Contacts: khaustov.dm@gmail.com
// File created on: 2020.02.26

// Log.hpp

#pragma once


#include <ostream>
#include <fstream>
#include <iostream>

class Log final
{
	class DoubleOstream: public std::ostream
	{
	public:
	    DoubleOstream(std::ostream& stream1, std::ostream& stream2)
	    : s1(stream1)
	    , s2(stream2)
	    {}

	    template<class T>
	    DoubleOstream& operator<<(T o)
	    {
	        s1 << o;
	        s2 << o;

	        return *this;
	    }

	    inline DoubleOstream&
	    operator<<(std::ostream& (*pFunction)(std::ostream&))
	    {
	        s1 << pFunction;
	        s2 << pFunction;
	        return *this;
	    }

	private:
	    std::ostream& s1;
		std::ostream& s2;
	};

public:
	Log(const Log&) = delete;
	Log& operator=(Log const&) = delete;
	Log(Log&& tmp) noexcept = delete;
	Log& operator=(Log&& tmp) noexcept = delete;

private:
	Log()
	: _ofs("output.txt", std::ios::app)
	, _os(std::cout, _ofs)
	{
		_ofs.flush();
	}
	~Log() = default;

	static Log &getInstance()
	{
		static Log instance;
		return instance;
	}

	std::ofstream _ofs;
	DoubleOstream _os;

public:
	static auto& stream()
	{
		return getInstance()._os;
	}
};


