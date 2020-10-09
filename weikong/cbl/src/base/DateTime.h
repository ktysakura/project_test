#ifndef PI_DATETIME_H_20190828
#define PI_DATETIME_H_20190828

#include <iostream>
#include <time.h>
#include <string.h>
#include <Windows.h>

using namespace std;

namespace cbl {

#define NullTime	-1

enum TimeSpec {
	LocalTime,		//时区时间
	UTC,			//格林威治时间
	OffsetFromUTC
};

class WDate
{
public:
	WDate() { jd = 0; }
	WDate(int y, int m, int d);

	bool isNull() const { return jd == 0; }
	bool isValid() const;

	int year() const;			//返回年
	int month() const;			//返回月
	int day() const;			//返回日
	int dayOfWeek() const;		//返回星期几
	int dayOfYear() const;		//返回该日在一年中的第几天
	int daysInMonth() const;	//返回该月的总天数
	int daysInYear() const;		//返回该年的总天数

	bool setDate(int year, int month, int day);
	int getDate(int *year, int *month, int *day);

	WDate addDays(int days) const;		//返回加上天数后的对象
	WDate addMonths(int months) const;	//返回加上月数后的对象
	WDate addYears(int years) const;	//返回加上年数后的对象

	int daysTo(const WDate &) const;	//返回相差天数，可正可负

	//比较运算符
	bool operator==(const WDate &other) const { return jd == other.jd; }
	bool operator!=(const WDate &other) const { return jd != other.jd; }
	bool operator<(const WDate &other) const { return jd < other.jd; }
	bool operator<=(const WDate &other) const { return jd <= other.jd; }
	bool operator>(const WDate &other) const { return jd > other.jd; }
	bool operator>=(const WDate &other) const { return jd >= other.jd; }

	static bool isValid(int y, int m, int d);
	static bool isLeapYear(int year);
	static WDate currentDate();		//返回当前日期(local)

	inline int toJulianDay() const { return jd; }

private:
	friend class WDateTime;
	friend class WDateTimePrivate;
	unsigned int jd;
};

class WTime
{
public:
	WTime(): mds(NullTime) {}
	WTime(int h, int m, int s = 0, int ms = 0);

	bool isNull() const { return mds == NullTime; }
	bool isValid() const;

	int hour() const;		//返回小时
	int minute() const;		//返回分钟
	int second() const;		//返回秒
	int msec() const;		//返回毫秒

	bool setHMS(int h, int m, int s, int ms = 0);
	int getHMS(int *h, int *m, int *s, int *ms); 

	WTime addSecs(int secs) const;		//返回加上秒数后的对象
	int secsTo(const WTime &) const;	//返回相差秒，可正可负
	WTime addMSecs(int ms) const;		//返回加上毫秒后的对象
	int msecsTo(const WTime &) const;	//返回相毫差秒，可正可负

	void start();			//开始计时
	int restart();			//重新开始计数
	int elapsed() const;	//从start或restart后经过的时间，单位：毫秒

	//比较运算符
	bool operator==(const WTime &other) const { return mds == other.mds; }
	bool operator!=(const WTime &other) const { return mds != other.mds; }
	bool operator<(const WTime &other) const { return mds < other.mds; }
	bool operator<=(const WTime &other) const { return mds <= other.mds; }
	bool operator>(const WTime &other) const { return mds > other.mds; }
	bool operator>=(const WTime &other) const { return mds >= other.mds; }

	static bool isValid(int h, int m, int s, int ms = 0);
	static WTime currentTime(); //返回当前时间(local)

private:
	friend class WDateTime;
	friend class WDateTimePrivate;
	inline int ds() const { return mds == -1 ? 0 : mds; }

private:
	int mds;
};

class WDateTimePrivate;

class WDateTime
{
public:
	WDateTime();
	explicit WDateTime(const WDate &);
	WDateTime(const WDate &, const WTime &, cbl::TimeSpec spec = cbl::LocalTime);
	WDateTime(const WDateTime &other);
	~WDateTime();

	WDateTime &operator=(const WDateTime &other);

	bool isNull() const;
	bool isValid() const;

	WDate date() const;
	WTime time() const;
	void setDate(const WDate &date);
	void setTime(const WTime &time);

	//设置时间标准，主要用到UTC标准和时区时间标准
	cbl::TimeSpec timeSpec() const;
	void setTimeSpec(cbl::TimeSpec spec);
	WDateTime toTimeSpec(cbl::TimeSpec spec) const;

	//按UTC标准计算从1970/01/01 00:00:00:000开始经过的毫秒数
	long long toMSecsSinceEpoch() const;
	void setMSecsSinceEpoch(long long msecs);

	//按UTC标准计算从1970/01/01 00:00:00:000开始经过的秒数
	//最大有效值是2038/01/19 03:14:06
	unsigned int toTime_t() const;
	void setTime_t(unsigned int secsSince1Jan1970UTC);
	
	WDateTime addDays(int days) const;
	WDateTime addMonths(int months) const;
	WDateTime addYears(int years) const;
	WDateTime addSecs(int secs) const;
	WDateTime addMSecs(unsigned int msecs) const;

	//转换为本地时间，也就是时区时间
	inline WDateTime toLocalTime() const { return toTimeSpec(cbl::LocalTime); }

	//转换为UTC时间，也就是标准时间
	inline WDateTime toUTC() const { return toTimeSpec(cbl::UTC); }

	int daysTo(const WDateTime &) const;		//返回相差天数，可正可负
	int secsTo(const WDateTime &) const;		//返回相差秒数，可正可负
	long long msecsTo(const WDateTime &) const;	//返回相差毫秒数，可正可负

	//比较运算符
	bool operator==(const WDateTime &other) const;
	inline bool operator!=(const WDateTime &other) const { return !(*this == other); }
	bool operator<(const WDateTime &other) const;
	inline bool operator<=(const WDateTime &other) const { return !(other < *this); }
	inline bool operator>(const WDateTime &other) const { return other < *this; }
	inline bool operator>=(const WDateTime &other) const { return !(*this < other); }

	//返回当前日期和时间(local)
	static WDateTime currentDateTime();

	//返回当前日期和时间(utc)
	static WDateTime currentDateTimeUtc();

	//返回time_t值构造的对象
	static WDateTime fromTime_t(unsigned int secsSince1Jan1970UTC);

	//返回msecs值构造的对象
	static WDateTime fromMSecsSinceEpoch(long long msecs);

	//返回当前日期时间距离1970/1/1 00:00:00:000的毫秒数
	static long long currentMSecsSinceEpoch();

private:
	friend class WDateTimePrivate;
	WDateTimePrivate *d;
};

}

#endif