#pragma once

#include <fstream>
#include <stdarg.h>

class Logger {
  public:
	Logger(const char* msg = "", const char* filename = "log.log") {
		m_stream.open(filename, std::fstream::app);
		if (msg) m_stream << msg << std::endl;
	}
	~Logger() { m_stream.close(); }
	void operator()(const char* logline, ...) {
		va_list argList;
		char cbuffer[1024];
		va_start(argList, logline);
		vsnprintf(cbuffer, 1024, logline, argList);
		va_end(argList);
		m_stream << cbuffer << std::endl;
	}
  private:
	std::ofstream m_stream;
};
