#pragma once
#include <string>
#include <memory>
#include <boost/format.hpp>

namespace kt
{	
	namespace log
	{						
		enum class SpdLogLevel : unsigned short
		{
			Spd_Trace = 0,
			Spd_Debug,
			Spd_Info,
			Spd_Warn,
			Spd_Error,
			Spd_Critical
		};

		class SpdLogImp;		
		class SpdLogWrapper
		{
		public:
			SpdLogWrapper();
			~SpdLogWrapper();

		public:
			template<typename... Arguments>
			static std::string Format(const std::string& fmt, Arguments&&... args)
			{
				boost::format f(fmt);
				int unroll[]{ 0, (f % std::forward<Arguments>(args), 0)... };
				static_cast<void>(unroll);
				return boost::str(f);
			}

			template<typename... Arguments>
			void Log(const SpdLogLevel& level, const std::string& fileName, int line, const std::string& category, const std::string& identify, const char* fmt, Arguments&&... args)
			{
				std::string logMessage = SpdLogWrapper::Format(fmt, std::forward<Arguments>(args)...);
				onLog(level, fileName, line, category, identify, logMessage);
			}

			void SetLogFileName(const std::string& logFileName);

		private:
			void onLog(const SpdLogLevel& level, const std::string& fileName, int line, const std::string& category, const std::string& identify, const std::string& logMessage);

		private:
			std::unique_ptr<SpdLogImp> m_Imp;			
		};
	}
}
