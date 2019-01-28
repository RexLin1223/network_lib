#include "SpdLogImp.h"
#include "LogFileHelper.h"

#include <boost/format.hpp>
#include <boost/filesystem.hpp>

#include <spdlog/async.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace kt
{
	namespace log
	{		
		SpdLogImp::SpdLogImp()
			: m_LogFileName("spdlog")
		{

		}

		SpdLogImp::~SpdLogImp()
		{
			DestroyLogger();
		}

		void SpdLogImp::SetLogFileName(const std::string& fileName)
		{
			m_LogFileName = fileName;
		}

		void SpdLogImp::CreateLogger(bool enableConsole /*= true*/)
		{
			std::string path = LogFileHelper::GetModulePath();
			std::string logFolderName = boost::str(boost::format("%s\\log\\") % std::move(LogFileHelper::GetModulePath()));
			boost::system::error_code ec;
			boost::filesystem::create_directory(logFolderName, ec);

			spdlog::init_thread_pool(8192, 1);
			auto stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt >();
			auto rotating_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
				boost::str(boost::format("%s%s.txt") % logFolderName % m_LogFileName), 1024 * 1024 * 100, 3);

			std::vector<spdlog::sink_ptr> sinks;
			if (enableConsole)
			{
				sinks = { stdout_sink, rotating_sink };
			}
			else
			{
				sinks = { rotating_sink };
			}

			m_Logger = std::make_shared<spdlog::async_logger>("local_log",
				sinks.begin(), sinks.end(), spdlog::thread_pool(), spdlog::async_overflow_policy::block);

			spdlog::register_logger(m_Logger);

			spdlog::set_pattern("[%Y-%m-%d] [%H:%M:%S.%e] [%z] [%^%l%$] %v");
			spdlog::set_level(spdlog::level::level_enum::trace);
			spdlog::flush_on(spdlog::level::level_enum::trace);
			spdlog::flush_every(std::chrono::seconds(5));
		}

		void SpdLogImp::DestroyLogger()
		{
			m_Logger.reset();
			spdlog::drop_all();
		}

		void SpdLogImp::trace(const std::string& logMessage)
		{
			if (m_Logger)
			{
				m_Logger->trace(logMessage);
			}
		}

		void SpdLogImp::debug(const std::string& logMessage)
		{
			if (m_Logger)
			{
				m_Logger->debug(logMessage);
			}
		}

		void SpdLogImp::info(const std::string& logMessage)
		{
			if (m_Logger)
			{
				m_Logger->info(logMessage);
			}				
		}

		void SpdLogImp::warn(const std::string& logMessage)
		{
			if (m_Logger)
			{
				m_Logger->warn(logMessage);
			}
		}

		void SpdLogImp::error(const std::string& logMessage)
		{
			if (m_Logger)
			{
				m_Logger->error(logMessage);
			}
		}

		void SpdLogImp::critical(const std::string& logMessage)
		{
			if (m_Logger)
			{
				m_Logger->critical(logMessage);
			}
		}
	}
}