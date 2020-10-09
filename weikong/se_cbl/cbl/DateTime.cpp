#include "DateTime.h"

namespace cbl {

enum {
	FIRST_YEAR = -4713,
	FIRST_MONTH = 1,
	FIRST_DAY = 2,
	SECS_PER_DAY = 86400,
	MSECS_PER_DAY = 86400000,
	SECS_PER_HOUR = 3600,
	MSECS_PER_HOUR = 3600000,
	SECS_PER_MIN = 60,
	MSECS_PER_MIN = 60000,
	JULIAN_DAY_FOR_EPOCH = 2440588 // result of julianDayFromGregorianDate(1970, 1, 1)
};

static const char monthDays[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
static const int LowerYear = 1980;
static const int UpperYear = 2037;

static inline WDate fixedDate(int y, int m, int d)
{
	WDate result(y, m, 1);
	int days = result.daysInMonth();

	result.setDate(y, m, (d < days) ? d : days);
	return result;
}

static inline unsigned int msecsFromDecomposed(int hour, int minute, int sec, int msec = 0)
{
	return MSECS_PER_HOUR * hour + MSECS_PER_MIN * minute + 1000 * sec + msec;
}

static inline unsigned int julianDayFromGregorianDate(int year, int month, int day)
{
	// Gregorian calendar starting from October 15, 1582
	// Algorithm from Henry F. Fliegel and Thomas C. Van Flandern
	return (1461 * (year + 4800 + (month - 14) / 12)) / 4
		+ (367 * (month - 2 - 12 * ((month - 14) / 12))) / 12
		- (3 * ((year + 4900 + (month - 14) / 12) / 100)) / 4
		+ day - 32075;
}

static unsigned int julianDayFromDate(int year, int month, int day)
{
	if (year < 0)
		++year;

	if (year > 1582 || (year == 1582 && (month > 10 || (month == 10 && day >= 15)))) {
		return julianDayFromGregorianDate(year, month, day);
	} else if (year < 1582 || (year == 1582 && (month < 10 || (month == 10 && day <= 4)))) {
		// Julian calendar until October 4, 1582
		// Algorithm from Frequently Asked Questions about Calendars by Claus Toendering
		int a = (14 - month) / 12;
		return (153 * (month + (12 * a) - 3) + 2) / 5
			+ (1461 * (year + 4800 - a)) / 4
			+ day - 32083;
	} else {
		// the day following October 4, 1582 is October 15, 1582
		return 0;
	}
}

static void getDateFromJulianDay(unsigned int julianDay, int *year, int *month, int *day)
{
	int y, m, d;

	if (julianDay >= 2299161) {
		// Gregorian calendar starting from October 15, 1582
		// This algorithm is from Henry F. Fliegel and Thomas C. Van Flandern
		unsigned long long ell, n, i, j;
		ell = (unsigned long long)(julianDay) + 68569;
		n = (4 * ell) / 146097;
		ell = ell - (146097 * n + 3) / 4;
		i = (4000 * (ell + 1)) / 1461001;
		ell = ell - (1461 * i) / 4 + 31;
		j = (80 * ell) / 2447;
		d = int(ell - (2447 * j) / 80);
		ell = j / 11;
		m = int(j + 2 - (12 * ell));
		y = int(100 * (n - 49) + i + ell);
	} else {
		// Julian calendar until October 4, 1582
		// Algorithm from Frequently Asked Questions about Calendars by Claus Toendering
		julianDay += 32082;
		int dd = (4 * julianDay + 3) / 1461;
		int ee = julianDay - (1461 * dd) / 4;
		int mm = ((5 * ee) + 2) / 153;
		d = ee - (153 * mm + 2) / 5 + 1;
		m = mm + 3 - 12 * (mm / 10);
		y = dd - 4800 + (mm / 10);
		if (y <= 0)
			--y;
	}
	if (year)
		*year = y;
	if (month)
		*month = m;
	if (day)
		*day = d;
}

WDate::WDate(int y, int m, int d)
{
	setDate(y, m, d);
}

bool WDate::isValid() const
{
	return !isNull();
}

bool WDate::isValid(int year, int month, int day)
{
	if (year < FIRST_YEAR
		|| (year == FIRST_YEAR && (month < FIRST_MONTH || (month == FIRST_MONTH && day < FIRST_DAY)))
		|| year == 0) {
		return false;
	}

	//历史上没有这几天
	if (year == 1582 && month == 10 && day > 4 && day < 15) {
		return false;
	}

	return (day > 0 && month > 0 && month <= 12) &&
		(day <= monthDays[month] || (day == 29 && month == 2 && isLeapYear(year)));
}

bool WDate::isLeapYear(int y)
{
	if (y < 1582) {
		if (y < 1) {  // No year 0 in Julian calendar, so -1, -5, -9 etc are leap years
			++y;
		}
		return y % 4 == 0;
	} else {
		return (y % 4 == 0 && y % 100 != 0) || y % 400 == 0;
	}
}

bool WDate::setDate(int year, int month, int day)
{
	if (!isValid(year, month, day)) {
		jd = 0;
	} else {
		jd = julianDayFromDate(year, month, day);
	}
	return jd != 0;
}

int WDate::year() const
{
	int y;

	getDateFromJulianDay(jd, &y, 0, 0);
	return y;
}

int WDate::month() const
{
	int m;

	getDateFromJulianDay(jd, 0, &m, 0);
	return m;
}

int WDate::day() const
{
	int d;

	getDateFromJulianDay(jd, 0, 0, &d);
	return d;
}

int WDate::dayOfWeek() const
{
	return (jd % 7) + 1;
}

int WDate::dayOfYear() const
{
	return jd - julianDayFromDate(year(), 1, 1) + 1;
}

int WDate::daysInMonth() const
{
	int y, m, d;
	
	getDateFromJulianDay(jd, &y, &m, &d);
	if (m == 2 && isLeapYear(y)) {
		return 29;
	} else {
		return monthDays[m];
	}
}

int WDate::daysInYear() const
{
	int y, m, d;

	getDateFromJulianDay(jd, &y, &m, &d);
	return isLeapYear(y) ? 366 : 365;
}

int WDate::getDate(int *year, int *month, int *day)
{
	if (year == NULL || month == NULL || day == NULL) {
		return -1;
	}

	getDateFromJulianDay(jd, year, month, day);
	return 0;
}

WDate WDate::addDays(int ndays) const
{
	WDate d;

	if (ndays >= 0)
		d.jd = (jd + ndays >= jd) ? jd + ndays : 0;
	else
		d.jd = (jd + ndays < jd) ? jd + ndays : 0;
	return d;
}

WDate WDate::addMonths(int nmonths) const
{
	if (!isValid()) {
		return WDate();
	}

	if (!nmonths) {
		return *this;
	}

	int old_y, y, m, d;
	getDateFromJulianDay(jd, &y, &m, &d);
	old_y = y;

	bool increasing = nmonths > 0;

	while (nmonths != 0) {
		if (nmonths < 0 && nmonths + 12 <= 0) {
			y--;
			nmonths+=12;
		} else if (nmonths < 0) {
			m+= nmonths;
			nmonths = 0;
			if (m <= 0) {
				--y;
				m += 12;
			}
		} else if (nmonths - 12 >= 0) {
			y++;
			nmonths -= 12;
		} else if (m == 12) {
			y++;
			m = 0;
		} else {
			m += nmonths;
			nmonths = 0;
			if (m > 12) {
				++y;
				m -= 12;
			}
		}
	}

	// was there a sign change?
	if ((old_y > 0 && y <= 0) ||
		(old_y < 0 && y >= 0))
		// yes, adjust the date by +1 or -1 years
		y += increasing ? +1 : -1;

	// did we end up in the Gregorian/Julian conversion hole?
	if (y == 1582 && m == 10 && d > 4 && d < 15)
		d = increasing ? 15 : 4;

	return fixedDate(y, m, d);
}

WDate WDate::addYears(int nyears) const
{
	if (!isValid()) {
		return WDate();
	}

	int y, m, d;
	getDateFromJulianDay(jd, &y, &m, &d);

	int old_y = y;
	y += nyears;

	// was there a sign change?
	if ((old_y > 0 && y <= 0) ||
		(old_y < 0 && y >= 0))
		// yes, adjust the date by +1 or -1 years
		y += nyears > 0 ? +1 : -1;

	return fixedDate(y, m, d);
}

int WDate::daysTo(const WDate &d) const
{
	return d.jd - jd;
}

WDate WDate::currentDate()
{
	WDate d;
	SYSTEMTIME st;

	memset(&st, 0, sizeof(SYSTEMTIME));
	GetLocalTime(&st);
	d.jd = julianDayFromDate(st.wYear, st.wMonth, st.wDay);
	return d;
}

WTime::WTime(int h, int m, int s, int ms)
{
	setHMS(h, m, s, ms);
}

bool WTime::isValid(int h, int m, int s, int ms)
{
	return (unsigned int)h < 24 && (unsigned int)m < 60 && (unsigned int)s < 60 && (unsigned int)ms < 1000;
}

bool WTime::isValid() const
{
	return mds > NullTime && mds < MSECS_PER_DAY;
}

bool WTime::setHMS(int h, int m, int s, int ms)
{
	if (!isValid(h,m,s,ms)) {
		mds = NullTime;  
		return false;
	}

	mds = (h*SECS_PER_HOUR + m*SECS_PER_MIN + s)*1000 + ms;
	return true;
}

int WTime::getHMS(int *h, int *m, int *s, int *ms)
{
	if (h == NULL || m == NULL || s == NULL || ms == NULL) {
		return -1;
	}

	*h = hour();
	*m = minute();
	*s = second();
	*ms = msec();
	return 0;
}

int WTime::hour() const
{
	return ds() / MSECS_PER_HOUR;
}

int WTime::minute() const
{
	return (ds() % MSECS_PER_HOUR) / MSECS_PER_MIN;
}

int WTime::second() const
{
	return (ds() / 1000)%SECS_PER_MIN;
}

int WTime::msec() const
{
	return ds() % 1000;
}

int WTime::secsTo(const WTime &t) const
{
	return (t.ds() - ds()) / 1000;
}

WTime WTime::addMSecs(int ms) const
{
	WTime t;
	if (ms < 0) {
		// % not well-defined for -ve, but / is.
		int negdays = (MSECS_PER_DAY - ms) / MSECS_PER_DAY;
		t.mds = (ds() + ms + negdays * MSECS_PER_DAY) % MSECS_PER_DAY;
	} else {
		t.mds = (ds() + ms) % MSECS_PER_DAY;
	}

	return t;
}

int WTime::msecsTo(const WTime &t) const
{
	return t.ds() - ds();
}

WTime WTime::currentTime()
{
	WTime ct;
	SYSTEMTIME st;

	memset(&st, 0, sizeof(SYSTEMTIME));
	GetLocalTime(&st);

	ct.mds = msecsFromDecomposed(st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	return ct;
}

void WTime::start()
{
	*this = currentTime();
}

int WTime::restart()
{
	WTime t = currentTime();
	int n = msecsTo(t);
	if (n < 0)                            
		n += 86400*1000;
	*this = t;
	return n;
}

int WTime::elapsed() const
{
	int n = msecsTo(currentTime());
	if (n < 0)                       
		n += 86400 * 1000;
	return n;
}

static WDate adjustDate(WDate date)
{
	WDate lowerLimit(LowerYear, 1, 2);
	WDate upperLimit(UpperYear, 12, 30);

	if (date > lowerLimit && date < upperLimit)
		return date;

	int month = date.month();
	int day = date.day();

	// neither 1970 nor 2037 are leap years, so make sure date isn't Feb 29
	if (month == 2 && day == 29)
		--day;

	if (date < lowerLimit)
		date.setDate(LowerYear, month, day);
	else
		date.setDate(UpperYear, month, day);

	return date;
}

static long long toMSecsSinceEpoch_helper(long long jd, int msecs)
{
	long long days = jd - JULIAN_DAY_FOR_EPOCH;
	long long retval = (days * MSECS_PER_DAY) + msecs;
	return retval;
}

class WDateTimePrivate
{
public:
	enum Spec { LocalUnknown = -1, LocalStandard = 0, LocalDST = 1, UTC = 2, OffsetFromUTC = 3};
	WDateTimePrivate() : spec(LocalUnknown), utcOffset(0) {}

	WDateTimePrivate(const WDateTimePrivate &other)
		: date(other.date), time(other.time), spec(other.spec), utcOffset(other.utcOffset)
	{}

	WDate date;
	WTime time;
	Spec spec;
	int utcOffset;

	Spec getLocal(WDate &outDate, WTime &outTime) const;
	void getUTC(WDate &outDate, WTime &outTime) const;
	static WDateTime addMSecs(const WDateTime &dt, long long msecs);
	static void addMSecs(WDate &utcDate, WTime &utcTime, long long msecs);
};

static WDateTimePrivate::Spec utcToLocal(WDate &date, WTime &time)
{
	WDate fakeDate = adjustDate(date);

	// won't overflow because of fakeDate
	time_t secsSince1Jan1970UTC = toMSecsSinceEpoch_helper(fakeDate.toJulianDay(), WTime().msecsTo(time)) / 1000;
	tm *brokenDown = 0;

	brokenDown = localtime(&secsSince1Jan1970UTC);

	if (!brokenDown) {
		date = WDate(1970, 1, 1);
		time = WTime();
		return WDateTimePrivate::LocalUnknown;
	} else {
		int deltaDays = fakeDate.daysTo(date);
		date = WDate(brokenDown->tm_year + 1900, brokenDown->tm_mon + 1, brokenDown->tm_mday);
		time = WTime(brokenDown->tm_hour, brokenDown->tm_min, brokenDown->tm_sec, time.msec());
		date = date.addDays(deltaDays);
		if (brokenDown->tm_isdst > 0)
			return WDateTimePrivate::LocalDST;
		else if (brokenDown->tm_isdst < 0)
			return WDateTimePrivate::LocalUnknown;
		else
			return WDateTimePrivate::LocalStandard;
	}
}

static void localToUtc(WDate &date, WTime &time, int isdst)
{
	if (!date.isValid())
		return;

	WDate fakeDate = adjustDate(date);

	tm localTM;
	localTM.tm_sec = time.second();
	localTM.tm_min = time.minute();
	localTM.tm_hour = time.hour();
	localTM.tm_mday = fakeDate.day();
	localTM.tm_mon = fakeDate.month() - 1;
	localTM.tm_year = fakeDate.year() - 1900;
	localTM.tm_isdst = (int)isdst;

#ifdef WIN32
	_tzset();
#endif

	time_t secsSince1Jan1970UTC = mktime(&localTM);
	tm *brokenDown = 0;
	brokenDown = gmtime(&secsSince1Jan1970UTC);

	if (!brokenDown) {
		date = WDate(1970, 1, 1);
		time = WTime();
	} else {
		int deltaDays = fakeDate.daysTo(date);
		date = WDate(brokenDown->tm_year + 1900, brokenDown->tm_mon + 1, brokenDown->tm_mday);
		time = WTime(brokenDown->tm_hour, brokenDown->tm_min, brokenDown->tm_sec, time.msec());
		date = date.addDays(deltaDays);
	}
}

WDateTimePrivate::Spec WDateTimePrivate::getLocal(WDate &outDate, WTime &outTime) const
{
	outDate = date;
	outTime = time;
	if (spec == WDateTimePrivate::UTC) {
		return utcToLocal(outDate, outTime);
	}
	return spec;
}

void WDateTimePrivate::getUTC(WDate &outDate, WTime &outTime) const
{
	outDate = date;
	outTime = time;
	const bool isOffset = spec == WDateTimePrivate::OffsetFromUTC;

	if (spec != WDateTimePrivate::UTC && !isOffset)
		localToUtc(outDate, outTime, (int)spec);

	if (isOffset)
		addMSecs(outDate, outTime, -((long long)(utcOffset) * 1000));
}

WDateTime WDateTimePrivate::addMSecs(const WDateTime &dt, long long msecs)
{
	WDate utcDate;
	WTime utcTime;
	dt.d->getUTC(utcDate, utcTime);

	addMSecs(utcDate, utcTime, msecs);

	return WDateTime(utcDate, utcTime, cbl::UTC).toTimeSpec(dt.timeSpec());
}

WDateTime::WDateTime() : d(new WDateTimePrivate)
{
}

WDateTime::WDateTime(const WDate &date) : d(new WDateTimePrivate)
{
	d->date = date;
	d->time = WTime(0, 0, 0);
}

WDateTime::WDateTime(const WDateTime &other) 
{
	d = new WDateTimePrivate(*other.d);
	//memcpy(d, other.d, sizeof(*other.d));
}

WDateTime::WDateTime(const WDate &date, const WTime &time, cbl::TimeSpec spec) : d(new WDateTimePrivate)
{
	d->date = date;
	d->time = date.isValid() && !time.isValid() ? WTime(0, 0, 0) : time;
	d->spec = (spec == cbl::UTC) ? WDateTimePrivate::UTC : WDateTimePrivate::LocalUnknown;
}

WDateTime::~WDateTime()
{
	if (d) {
		delete d;
		d = NULL;
	}
}

WDateTime &WDateTime::operator=(const WDateTime &other)
{
	d = new WDateTimePrivate(*other.d);
	//memcpy(d, other.d, sizeof(*other.d));
	return *this;
}

bool WDateTime::isNull() const
{
	return d->date.isNull() && d->time.isNull();
}

bool WDateTime::isValid() const
{
	return d->date.isValid() && d->time.isValid();
}

WDate WDateTime::date() const
{
	return d->date;
}

WTime WDateTime::time() const
{
	return d->time;
}

cbl::TimeSpec WDateTime::timeSpec() const
{
	switch(d->spec)
	{
	case WDateTimePrivate::UTC:
		return cbl::UTC;

	case WDateTimePrivate::OffsetFromUTC:
		return cbl::OffsetFromUTC;

	default:
		return cbl::LocalTime;
	}
}

long long WDateTime::toMSecsSinceEpoch() const
{
	WDate utcDate;
	WTime utcTime;
	d->getUTC(utcDate, utcTime);

	return toMSecsSinceEpoch_helper(utcDate.jd, utcTime.ds());
}

void WDateTime::setTime_t(unsigned int secsSince1Jan1970UTC)
{
	WDateTimePrivate::Spec oldSpec = d->spec;

	d->date = WDate(1970, 1, 1).addDays(secsSince1Jan1970UTC / SECS_PER_DAY);
	d->time = WTime().addSecs(secsSince1Jan1970UTC % SECS_PER_DAY);
	d->spec = WDateTimePrivate::UTC;

	if (oldSpec != WDateTimePrivate::UTC)
		d->spec = d->getLocal(d->date, d->time);
}

WDateTime WDateTime::addDays(int ndays) const
{
	return WDateTime(d->date.addDays(ndays), d->time, timeSpec());
}

WDateTime WDateTime::addMonths(int nmonths) const
{
	return WDateTime(d->date.addMonths(nmonths), d->time, timeSpec());
}

WDateTime WDateTime::addYears(int nyears) const
{
	return WDateTime(d->date.addYears(nyears), d->time, timeSpec());
}

WTime WTime::addSecs(int s) const
{
	return addMSecs(s * 1000);
}

void WDateTimePrivate::addMSecs(WDate &utcDate, WTime &utcTime, long long msecs)
{
	unsigned int dd = utcDate.jd;
	int tt = utcTime.ds();
	int sign = 1;
	if (msecs < 0) {
		msecs = -msecs;
		sign = -1;
	}
	if (msecs >= int(MSECS_PER_DAY)) {
		dd += (unsigned int)(sign * (msecs / MSECS_PER_DAY));
		msecs %= MSECS_PER_DAY;
	}

	tt += int(sign * msecs);
	if (tt < 0) {
		tt = MSECS_PER_DAY - tt - 1;
		dd -= tt / MSECS_PER_DAY;
		tt = tt % MSECS_PER_DAY;
		tt = MSECS_PER_DAY - tt - 1;
	} else if (tt >= int(MSECS_PER_DAY)) {
		dd += tt / MSECS_PER_DAY;
		tt = tt % MSECS_PER_DAY;
	}

	utcDate.jd = dd;
	utcTime.mds = tt;
}

WDateTime WDateTime::toTimeSpec(cbl::TimeSpec spec) const
{
	if ((d->spec == WDateTimePrivate::UTC) == (spec == cbl::UTC))
		return *this;

	WDateTime ret;
	if (spec == cbl::UTC) {
		d->getUTC(ret.d->date, ret.d->time);
		ret.d->spec = WDateTimePrivate::UTC;
	} else {
		ret.d->spec = d->getLocal(ret.d->date, ret.d->time);
	}
	return ret;
}

int WDateTime::daysTo(const WDateTime &other) const
{
	return d->date.daysTo(other.d->date);
}

int WDateTime::secsTo(const WDateTime &other) const
{
	WDate date1, date2;
	WTime time1, time2;

	d->getUTC(date1, time1);
	other.d->getUTC(date2, time2);

	return (date1.daysTo(date2) * SECS_PER_DAY) + time1.secsTo(time2);
}

long long WDateTime::msecsTo(const WDateTime &other) const
{
	WDate selfDate;
	WDate otherDate;
	WTime selfTime;
	WTime otherTime;

	d->getUTC(selfDate, selfTime);
	other.d->getUTC(otherDate, otherTime);

	return (static_cast<long long>(selfDate.daysTo(otherDate)) * static_cast<long long>(MSECS_PER_DAY))
		+ static_cast<long long>(selfTime.msecsTo(otherTime));
}

bool WDateTime::operator==(const WDateTime &other) const
{
	if (d->spec == other.d->spec && d->utcOffset == other.d->utcOffset)
		return d->time == other.d->time && d->date == other.d->date;
	else {
		WDate date1, date2;
		WTime time1, time2;

		d->getUTC(date1, time1);
		other.d->getUTC(date2, time2);
		return time1 == time2 && date1 == date2;
	}
}

bool WDateTime::operator<(const WDateTime &other) const
{
	if (d->spec == other.d->spec && d->spec != WDateTimePrivate::OffsetFromUTC) {
		if (d->date != other.d->date)
			return d->date < other.d->date;
		return d->time < other.d->time;
	} else {
		WDate date1, date2;
		WTime time1, time2;
		d->getUTC(date1, time1);
		other.d->getUTC(date2, time2);
		if (date1 != date2)
			return date1 < date2;
		return time1 < time2;
	}
}

WDateTime WDateTime::currentDateTime()
{
	WDate d;
	WTime t;
	SYSTEMTIME st;

	memset(&st, 0, sizeof(SYSTEMTIME));
	GetLocalTime(&st);
	d.jd = julianDayFromDate(st.wYear, st.wMonth, st.wDay);
	t.mds = msecsFromDecomposed(st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	return WDateTime(d, t);
}

WDateTime WDateTime::currentDateTimeUtc()
{
	WDate d;
	WTime t;
	SYSTEMTIME st;

	memset(&st, 0, sizeof(SYSTEMTIME));
	GetSystemTime(&st);
	d.jd = julianDayFromDate(st.wYear, st.wMonth, st.wDay);
	t.mds = msecsFromDecomposed(st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	return WDateTime(d, t, cbl::UTC);
}

WDateTime WDateTime::fromTime_t(unsigned int seconds)
{
	WDateTime d;
	d.setTime_t(seconds);
	return d;
}

WDateTime WDateTime::fromMSecsSinceEpoch(long long msecs)
{
	WDateTime d;
	d.setMSecsSinceEpoch(msecs);
	return d;
}

void WDateTime::setMSecsSinceEpoch(long long msecs)
{
	WDateTimePrivate::Spec oldSpec = d->spec;

	int ddays = int(msecs / MSECS_PER_DAY);
	msecs %= MSECS_PER_DAY;
	if (msecs < 0) {
		// negative
		--ddays;
		msecs += MSECS_PER_DAY;
	}

	d->date = WDate(1970, 1, 1).addDays(ddays);
	d->time = WTime().addMSecs((int)msecs);
	d->spec = WDateTimePrivate::UTC;

	if (oldSpec != WDateTimePrivate::UTC)
		d->spec = d->getLocal(d->date, d->time);
}

unsigned int WDateTime::toTime_t() const
{
	long long retval = toMSecsSinceEpoch() / 1000;
	if ((unsigned long long)(retval) >= 0xFFFFFFFF)
		return (unsigned int)(-1);
	return (unsigned int)(retval);
}

void WDateTime::setDate(const WDate &date)
{
	d->date = date;
	if (d->spec == WDateTimePrivate::LocalStandard
		|| d->spec == WDateTimePrivate::LocalDST)
		d->spec = WDateTimePrivate::LocalUnknown;
	if (date.isValid() && !d->time.isValid())
		d->time = WTime(0, 0, 0);
}

void WDateTime::setTime(const WTime &time)
{
	if (d->spec == WDateTimePrivate::LocalStandard
		|| d->spec == WDateTimePrivate::LocalDST)
		d->spec = WDateTimePrivate::LocalUnknown;
	d->time = time;
}

void WDateTime::setTimeSpec(cbl::TimeSpec spec)
{
	switch(spec)
	{
	case cbl::UTC:
		d->spec = WDateTimePrivate::UTC;
		break;
	case cbl::OffsetFromUTC:
		d->spec = WDateTimePrivate::OffsetFromUTC;
		break;
	default:
		d->spec = WDateTimePrivate::LocalUnknown;
		break;
	}
}

long long WDateTime::currentMSecsSinceEpoch()
{
	WDate d;
	WTime t;
	SYSTEMTIME st;

	memset(&st, 0, sizeof(SYSTEMTIME));
	GetSystemTime(&st);

	return msecsFromDecomposed(st.wHour, st.wMinute, st.wSecond, st.wMilliseconds) +
		(long long)(julianDayFromGregorianDate(st.wYear, st.wMonth, st.wDay)
		- julianDayFromGregorianDate(1970, 1, 1)) * 86400000;
}

}