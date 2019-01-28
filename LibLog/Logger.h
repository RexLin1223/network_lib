#pragma once

#include "SpdLogWrapper.h"
#include "Singleton.h"

#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#ifdef ENABLE_LOGGER	
	#define LOG_INSTANCE kt::log::Singleton<kt::log::SpdLogWrapper>::Get()
	#define LOG_SET_NAME(fileName) LOG_INSTANCE->SetLogFileName(fileName)
	#define LOG_TRACE(category, identify, fmt, ...) LOG_INSTANCE->Log(kt::log::SpdLogLevel::Spd_Trace, __FILENAME__, __LINE__, category, identify, fmt, ##__VA_ARGS__)
	#define LOG_DEBUG(category, identify, fmt, ...) LOG_INSTANCE->Log(kt::log::SpdLogLevel::Spd_Debug,__FILENAME__, __LINE__, category, identify, fmt, ##__VA_ARGS__)
	#define LOG_INFO(category, identify, fmt, ...) LOG_INSTANCE->Log(kt::log::SpdLogLevel::Spd_Info,__FILENAME__, __LINE__, category, identify, fmt, ##__VA_ARGS__)
	#define LOG_WARN(category, identify, fmt, ...) LOG_INSTANCE->Log(kt::log::SpdLogLevel::Spd_Warn,__FILENAME__, __LINE__, category, identify, fmt, ##__VA_ARGS__)
	#define LOG_ERROR(category, identify, fmt, ...) LOG_INSTANCE->Log(kt::log::SpdLogLevel::Spd_Error,__FILENAME__, __LINE__, category, identify, fmt, ##__VA_ARGS__)
	#define LOG_CRITICAL(category, identify, fmt, ...) LOG_INSTANCE->Log(kt::log::SpdLogLevel::Spd_Critical,__FILENAME__, __LINE__, category, identify, fmt, ##__VA_ARGS__)
#else
	#define LOG_TRACE(category, identify, fmt, ...)
	#define LOG_DEBUG(category, identify, fmt, ...)
	#define LOG_INFO(category, identify, fmt, ...)
	#define LOG_WARN(category, identify, fmt, ...)
	#define LOG_ERROR(category, identify, fmt, ...)
	#define LOG_CRITICAL(category, identify, fmt, ...)
#endif