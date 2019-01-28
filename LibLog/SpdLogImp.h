#pragma once
#include <string>
#include <spdlog/spdlog.h>

namespace kt
{
	namespace log
	{
		class SpdLogImp
		{
		public:
			SpdLogImp();
			~SpdLogImp();

		public:
			void SetLogFileName(const std::string& fileName);
			void CreateLogger(bool enableConsole = true);
			void DestroyLogger();

		public:
			void trace(const std::string& logMessage);
			void debug(const std::string& logMessage);
			void info(const std::string& logMessage);
			void warn(const std::string& logMessage);
			void error(const std::string& logMessage);
			void critical(const std::string& logMessage);

		private:
			std::string m_LogFileName;
			std::shared_ptr<spdlog::logger> m_Logger;
		};
	}
}