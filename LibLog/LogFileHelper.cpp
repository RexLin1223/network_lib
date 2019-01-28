#include "LogFileHelper.h"
#include <codecvt>
#include <wtypes.h>

namespace kt
{
	namespace log
	{
		std::string LogFileHelper::GetModulePath()
		{
			WCHAR buff[MAX_PATH + 1];
			std::wstring wsTemp = L"";

			if (GetModuleFileName(NULL, buff, MAX_PATH) > 0) {
				std::wstring str = buff;
				int find = (int)str.rfind('\\');
				if (find != -1) {
					wsTemp = str.substr(0, find + 1);
				}
			}

			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> myconv;
			return myconv.to_bytes(wsTemp);
		}
	}
			
}