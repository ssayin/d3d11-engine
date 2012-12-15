#include <Windows.h>
#include "logger.h"

Log::Logger Log::cLogger;

namespace Log
{
	void Log(const std::wstring& wstrMessage)
	{
		cLogger.Log(wstrMessage);
	}

	void Log(const wchar_t *format, ...)
	{
		wchar_t buffer[LOG_STRINGMAX];
		va_list valist;
		va_start(valist, format);
		_vsnwprintf(buffer, LOG_STRINGMAX, format, valist);
		va_end(valist);

		cLogger.Log(buffer);
	}

	void Log(const char *format, ...)
	{
		char buffer[LOG_STRINGMAX];
		va_list valist;
		va_start(valist, format);
		_vsnprintf(buffer, LOG_STRINGMAX, format, valist);
		va_end(valist);

		std::wstringstream wstrMessage;
		wstrMessage << buffer;
		cLogger.Log(wstrMessage.str());
	}

	Logger::Logger()
	{
		_cLogStream.open("../Debug/Log.txt", (std::ios_base::out | std::ios_base::app));
	}

	Logger::~Logger()
	{
		_cLogStream << std::endl 
					<< std::endl
					<< std::flush;

		_cLogStream.close();
	}

	std::wstring Logger::GetDate() const
	{
		std::wstringstream wss;

		SYSTEMTIME sysTime;
		::GetLocalTime(&sysTime);

		wss << sysTime.wYear << L"/"
			<< sysTime.wMonth << L"/"
			<< sysTime.wDay;

		return wss.str();
	}

	std::wstring Logger::GetTime() const
	{
		std::wstringstream wss;

		SYSTEMTIME sysTime;
		::GetLocalTime(&sysTime);

		wss << sysTime.wHour << L":"
			<< sysTime.wMinute << L":"
			<< sysTime.wSecond;

		return wss.str();
	}

	void Logger::Log(std::wstring wstrMessage)
	{
		_cLogStream << GetDate() << L"  " 
					<< GetTime() << L"  "
					<< wstrMessage
					<< std::endl
					<< std::flush;
#ifdef _DEBUG
		wstrMessage += L"\n";
		OutputDebugString(wstrMessage.c_str());
#endif
	}
}