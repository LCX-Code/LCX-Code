
#include <windows.h>
#include <minwindef.h>
#include <string>
#include <vector>
#include <regex>
#include "utils.h"


long GetElapsedProcessTime()   //�õ����й���ʱ��
{
	FILETIME createTime;
	FILETIME exitTime;
	FILETIME kernelTime;    //����ʱ��
	FILETIME userTime;

	long ElapsedTime;   //����ʱ��
	if (GetProcessTimes(GetCurrentProcess(), &createTime, &exitTime, &kernelTime, &userTime) != 0)
	{
		//  Returns total user time.
		SYSTEMTIME userSystemTime;
		if (FileTimeToSystemTime(&userTime, &userSystemTime) != -1)
		{
			ElapsedTime = (userSystemTime.wDay - 1) * 24 * 3600 * 1000
						  + userSystemTime.wHour * 3600 * 1000 +
						  userSystemTime.wMinute * 60 * 1000 +
						  userSystemTime.wSecond * 1000 +
						  userSystemTime.wMilliseconds;
		}
		else
		{
			ElapsedTime = 0;
		}
	}
	else
	{
		ElapsedTime = 0;
	}
	return ElapsedTime;
}

std::vector<std::string> StringSplit(const std::string& str, char delim)
{
	std::string s;
	s.append(1, delim);
	std::regex reg(s);
	std::vector<std::string> elems(std::sregex_token_iterator(str.begin(), str.end(), reg, -1),
		std::sregex_token_iterator());
	return elems;
}