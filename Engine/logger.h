#pragma once

#include <sstream>
#include <fstream>

#ifndef _LOGGER_NO_LOG_
#define LOG_ERROR(M) { Log::Log("ERROR: %s", M); }
#define LOG_WARNING(M) { Log::Log("WARNING: %s", M); }
#define LOG_INFO(M) { Log::Log("INFO: %s", M); }
#define LOG_ERROR_ARGS(M, ...) { Log::Log(M, __VA_ARGS__); }
#define LOG_WARNING_ARGS(M, ...) { Log::Log(M, __VA_ARGS__); }
#define LOG_INFO_ARGS(M, ...) { Log::Log(M, __VA_ARGS__); }
#else
#define LOG_ERROR(M)
#define LOG_INFO(M)
#define LOG_WARNING(M)
#define LOG_ERROR_ARGS(M, ...)
#define LOG_WARNING_ARGS(M, ...)
#define LOG_INFO_ARGS(M, ...)
#endif

namespace Log
{
	void Log(const std::wstring& wstrMessage);
    void Log(const wchar_t *format, ...);
    void Log(const char *format, ...);

	enum { LOG_STRINGMAX = 1024 };

	class Logger
	{
	public:
		Logger();
		~Logger();
		void Log(const std::wstring wstrMessage);

	private:
		std::wstring GetTime() const;
		std::wstring GetDate() const;

		std::wofstream _cLogStream;
	};

	extern Logger cLogger;
}