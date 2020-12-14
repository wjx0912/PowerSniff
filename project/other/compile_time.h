#ifndef __COMPILE_TIME_0354662121_H__
#define __COMPILE_TIME_0354662121_H__
#include <iostream>
#include <sstream>
#include <string>

// usage:
// cout << "Time_t when compiled: " << get_compiled_time() << endl;
// cout << "Time_t now: " << time(0) << endl;

inline bool get_compiled_tm(tm *_t)
{
	std::string datestr = __DATE__;
	std::string timestr = __TIME__;

	std::istringstream iss_date(datestr);
	std::string str_month;
	int year;
	int month = 1;
	int day;
	iss_date >> str_month >> day >> year;

	if (str_month == "Jan") month = 1;
	else if (str_month == "Feb") month = 2;
	else if (str_month == "Mar") month = 3;
	else if (str_month == "Apr") month = 4;
	else if (str_month == "May") month = 5;
	else if (str_month == "Jun") month = 6;
	else if (str_month == "Jul") month = 7;
	else if (str_month == "Aug") month = 8;
	else if (str_month == "Sep") month = 9;
	else if (str_month == "Oct") month = 10;
	else if (str_month == "Nov") month = 11;
	else if (str_month == "Dec") month = 12;
	// else exit(-1);

	for (std::string::size_type pos = timestr.find(':'); pos != std::string::npos; pos = timestr.find(':', pos))
		timestr[pos] = ' ';
	std::istringstream iss_time(timestr);
	int hour, min, sec;
	iss_time >> hour >> min >> sec;

	tm t = { 0 };
	t.tm_mon = month - 1;
	t.tm_mday = day;
	t.tm_year = year - 1900;
	t.tm_hour = hour;
	t.tm_min = min;
	t.tm_sec = sec;
	memcpy(_t, &t, sizeof(tm));
	return true;
}

inline time_t get_compiled_time()
{
	tm _t;
	get_compiled_tm(&_t);
	return mktime(&_t);
}

inline std::string get_compile_strtime()
{
	tm _t;
	get_compiled_tm(&_t);

	char buf[128] = "";
	sprintf_s(buf
		, "%d-%02d-%02d %02d:%02d:%02d"
		, _t.tm_year + 1900
		, _t.tm_mon + 1
		, _t.tm_mday
		, _t.tm_hour
		, _t.tm_min
		, _t.tm_sec
		);

	return buf;
}

#endif
