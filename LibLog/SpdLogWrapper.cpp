#include "SpdLogWrapper.h"
#include "SpdLogImp.h"
#include <Windows.h>

namespace kt
{
	namespace log
	{	
		SpdLogWrapper::SpdLogWrapper()
			: m_Imp(std::make_unique<SpdLogImp>())
		{
			m_Imp->CreateLogger(false);
		}

		SpdLogWrapper::~SpdLogWrapper()
		{

		}

		void SpdLogWrapper::SetLogFileName(const std::string& logFileName)
		{
			m_Imp->DestroyLogger();
			m_Imp->SetLogFileName(logFileName);
			m_Imp->CreateLogger(false);
		}

		void SpdLogWrapper::onLog(const SpdLogLevel& level, const std::string& fileName, int line, const std::string& category, const std::string& identify, const std::string& logMessage)
		{
			std::string reformatLog = boost::str(boost::format("[%s] [pid:%d] [tid:%d] [%s(%d)] [%s]")
				% identify
				% ::GetCurrentProcessId()
				% ::GetCurrentThreadId()
				% fileName
				% line
				% logMessage);


			switch (level)
			{
			case SpdLogLevel::Spd_Trace:
				m_Imp->trace(reformatLog);
				break;
			case SpdLogLevel::Spd_Debug:
				m_Imp->debug(reformatLog);
				break;
			case SpdLogLevel::Spd_Info:
				m_Imp->info(reformatLog);
				break;
			case SpdLogLevel::Spd_Warn:
				m_Imp->warn(reformatLog);
				break;
			case SpdLogLevel::Spd_Error:
				m_Imp->error(reformatLog);
				break;
			case SpdLogLevel::Spd_Critical:
				m_Imp->critical(reformatLog);
				break;
			}
		}

	}
}
