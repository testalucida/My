#ifndef MYDATE_HPP
#define MYDATE_HPP

//#ifdef WIN32
//#define  sprintf sprintf_s
//#endif


#include <stdio.h>
#include <cstdlib>
#include <my/datetime.h>
#include <my/compat.h>
#include <my/CharBuffer.h>

namespace my {

    MyDate::MyDate(bool setToCurrentDate) {
        if (setToCurrentDate) {
            SetCurrentDate();
        } else {
            _d = _m = _y = 0;
        }
    }

    MyDate::MyDate(int d, int m, int y) {
        SetDate(d, m, y);
    }

	MyDate::MyDate( const char *pDate ) {
        SetDate( pDate );
    }

    MyDate::MyDate(const MyDate& other) {
        Copy(other);
    }

    void MyDate::FromDate( const MyDate& other ) {
        SetDay( other.GetDay() );
        SetMonth( other.GetMonth() );
        SetYear( other.GetYear() );
    }
    
    void MyDate::FromString(const string &date ) {
        const char *check = date.substr(2, 1).c_str();
        if (strcmp(check, ".") == 0) {
            FromEurString( date );
        } else {
            FromIsoString( date );
        }
    }
    
    void MyDate::FromIsoString(const string &date ) {
        SetDay(atoi(date.substr(8, 2).c_str()));
        SetMonth(atoi(date.substr(5, 2).c_str()));
        SetYear(atoi(date.substr(0, 4).c_str()));
    }
    
    void MyDate::FromEurString(const string &date ) {
        SetDay(atoi(date.substr(0, 2).c_str()));
        SetMonth(atoi(date.substr(3, 2).c_str()));
        SetYear(atoi(date.substr(6, 4).c_str()));
    }

    /**
            provides a date, contained in a string, into a given MyDate instance.
            The string representation of date has to be ISO formatted, i.e. "YYYY-MM-DD"
     */
    MyDate& MyDate::FromIsoString(MyDate &date, const string &isodate) {
        date.SetDay(atoi(isodate.substr(8, 2).c_str()));
        date.SetMonth(atoi(isodate.substr(5, 2).c_str()));
        date.SetYear(atoi(isodate.substr(0, 4).c_str()));

        return date;
    }

    /**
            provides a date, contained in a string, into a given MyDate instance.
            The string representation of date has to be EUR formatted, i.e. "DD.MM.YYYY"
     */
    MyDate& MyDate::FromEurString(MyDate &date, const string &eurdate) {
        date.SetDay(atoi(eurdate.substr(0, 2).c_str()));
        date.SetMonth(atoi(eurdate.substr(3, 2).c_str()));
        date.SetYear(atoi(eurdate.substr(6, 4).c_str()));

        return date;
    }

    /**
            provides a date, contained in a string, into a given MyDate instance.
            The string representation of date has to be either EUR  or ISO formatted, 
            i.e. "DD.MM.YYYY" or "YYYY-MM-DD"
     */
    MyDate& MyDate::FromString(MyDate &date, const string &strdate) {
        const char *check = strdate.substr(2, 1).c_str();
        if (strcmp(check, ".") == 0) {
            return FromEurString(date, strdate);
        }
        return FromIsoString(date, strdate);
    }

    /**
            provides a date, contained in a c-string, into a given MyDate instance.
            The string representation of date has to be either EUR  or ISO formatted, 
            i.e. "DD.MM.YYYY" or "YYYY-MM-DD"
     */
    MyDate& MyDate::FromString(MyDate &date, const char *strdate) {
        if (strlen(strdate) < 10) {
            return date;
        }
        if (strdate[2] == '.') {
            return FromEurString(date, strdate);
        }
        return FromIsoString(date, strdate);
    }

    MyDate& MyDate::operator=(const MyDate& other) {
        if (this == &other) return *this;
        Copy(other);
        return *this;
    }

    bool MyDate::operator==(const MyDate& other) const {
        return ( (_y == other._y) &&
                (_m == other._m) &&
                (_d == other._d));
    }

    bool MyDate::operator<(const MyDate& other) const {
        if (this->_y < other._y) {
            return true;
        } else if (other._y < this->_y) {
            return false;
        }

        //still here, so years are equal, let's look
        //at the months
        if (this->_m < other._m) {
            return true;
        } else if (other._m < this->_m) {
            return false;
        }

        //still here, so months are equal, too
        //let's look at the days:
        return ( this->_d < other._d);
    }

    bool MyDate::operator>(const MyDate& other) const {
        if (this->_y > other._y) {
            return true;
        } else if (other._y > this->_y) {
            return false;
        }

        //still here, so years are equal, let's look
        //at the months
        if (this->_m > other._m) {
            return true;
        } else if (other._m > this->_m) {
            return false;
        }

        //still here, so months are equal, too
        //let's look at the days:
        return ( this->_d > other._d);
    }

    bool MyDate::operator <=(const MyDate &other) const {
        return ( (*this < other) || (*this == other));
    }

	bool MyDate::operator >=(const MyDate &other) const {
        return ( (*this > other) || (*this == other));
    }

    void MyDate::Copy(const MyDate & other) {
        _d = other._d;
        _m = other._m;
        _y = other._y;
        _sIsoDate = other._sIsoDate;
        _sEurDate = other._sEurDate;
        _isEurFormatted = other._isEurFormatted;
        _isIsoFormatted = other._isIsoFormatted;
    }

    void MyDate::SetCurrentDate() {
        time_t now;
        struct tm tmNow;
        time(&now);
        Compat::localtime(&tmNow, &now);
        SetDate(tmNow.tm_mday, tmNow.tm_mon + 1, 1900 + tmNow.tm_year);
    }

    void MyDate::SetDate(int d, int m, int y) {
        SetDay(d);
        SetMonth(m);
        SetYear(y);
    }

	void MyDate::SetDate( const char *pDate ) {
		MyDate d;
		MyDate::FromString( d, pDate );
		SetDay( d.GetDay() );
		SetMonth( d.GetMonth() );
		SetYear( d.GetYear() );
    }

    int MyDate::GetWeekday() {
        tm tm1;
        tm1.tm_year = _y - 1900;
        tm1.tm_mon = _m - 1;
        tm1.tm_mday = _d;
        tm1.tm_hour = tm1.tm_min = tm1.tm_sec = 0;
        time_t tt = mktime(&tm1);
        Compat::localtime(&tm1, &tt);
        return tm1.tm_wday;
    }
    
    int MyDate::GetWeekNumber( const char *pFormat ) const {
        tm tm1;
        tm1.tm_year = _y - 1900;
        tm1.tm_mon = _m - 1;
        tm1.tm_mday = _d;
        tm1.tm_hour = tm1.tm_min = tm1.tm_sec = 0;
        time_t tt = mktime(&tm1);
        char buffer[5];
        strftime( buffer, 5, pFormat, &tm1 );
        return atoi( buffer );
    }

    void MyDate::SetDay(int d) {
        _d = d;
        SetFormatted(false);
    }

    int MyDate::GetDay() const {
        return _d;
    }

    void MyDate::SetMonth(int m) {
        _m = m;
        SetFormatted(false);
    }

    int MyDate::GetMonth() const {
        return _m;
    }

    void MyDate::SetYear(int y) {
        _y = y;
        SetFormatted(false);
    }

    int MyDate::GetYear() const {
        return _y;
    }

    bool MyDate::IsSet() const {
        return _y > 0 && _m > 0 && _d > 0;
    }

    const char * MyDate::GetMonthName() const {
        return MONTHNAME[_m];
    }

    int MyDate::GetNumberOfDays(int month, int year) {
        if (month != 2) {
            return DAYS_PER_MONTH[month];
        } else {
            return MyDate::IsLeapYear(year) ? 29 : 28;
        }
    }

    /**
            if the given year is a leap year
     */
    bool MyDate::IsLeapYear(int year) {
        int n = sizeof ( LEAPYEAR);
        for (int i = 0; i < n; i++) {
            if (year == LEAPYEAR[i]) {
                return true;
            }
        }
        return false;
    }

    const string & MyDate::ToIsoString() const {
        if (!_isIsoFormatted) {
            char buf[6];
            //convert year:
            sprintf(buf, "%4d-", _y);
            _sIsoDate.clear();
            _sIsoDate.append(buf);

            //convert month:
            sprintf(buf, "%02d-", _m);
            _sIsoDate.append(buf);

            //convert day:
            sprintf(buf, "%02d", _d);
            _sIsoDate.append(buf);

            _isIsoFormatted = true;
        }
        return _sIsoDate;
    }

    const string & MyDate::ToEurString() const {
        if (!_isEurFormatted) {
            char buf[6];
            //convert year:
            sprintf(buf, "%02d.", _d);
            _sEurDate.clear();
            _sEurDate.append(buf);

            //convert month:
            sprintf(buf, "%02d.", _m);
            _sEurDate.append(buf);

            //convert day:
            sprintf(buf, "%4d", _y);
            _sEurDate.append(buf);

            _isEurFormatted = true;
        }
        return _sEurDate;
    }

    void MyDate::SetFormatted(bool formatted) {
        _isEurFormatted = formatted;
        _isIsoFormatted = formatted;
    }

    MyTime::MyTime(int hour, int minute, int second, int fraction) {
        _h = hour;
        _m = minute;
        _s = second;
        _f = fraction;
    }

    MyTime::MyTime(const MyTime & other) {
        Copy(other);
    }

	MyTimePtr MyTime::FromString( const char *pTime ) {
		MyTimePtr pT( new MyTime() );
		string time( pTime );
		int h = atoi( time.substr( 0, 2 ).c_str() );
		int m = atoi( time.substr( 3, 2 ).c_str() );
		int s = atoi( time.substr( 6, 2 ).c_str() );
		long n = atol( time.substr( 9 ).c_str() );
		pT->SetTime( h, m, s, n );
		return pT;
	}

    MyTime& MyTime::operator=(const MyTime& other) {
        if (this == &other) return *this;
        Copy(other);
        return *this;
    }

    bool MyTime::operator==(const MyTime& other) const {
        return ( (_h == other._h) &&
                (_m == other._m) &&
                (_s == other._s) &&
                (_f == other._f));
    }

    bool MyTime::operator<(const MyTime& other) const {
        if (this->_h < other._h) {
            return true;
        } else if (other._h < this->_h) {
            return false;
        }

        //still here, so hours are equal, let's look
        //at the minutes
        if (this->_m < other._m) {
            return true;
        } else if (other._m < this->_m) {
            return false;
        }

        //still here, so minutes are equal, too
        //let's look at the seconds
        if (this->_s < other._s) {
            return true;
        } else if (other._s < this->_s) {
            return false;
        }

        //still her, so seconds are equal too
        //let's look at the fractions
        return ( this->_f < other._f);
    }

    bool MyTime::operator>(const MyTime& other) const {
        if (this->_h > other._h) {
            return true;
        } else if (other._h > this->_h) {
            return false;
        }

        //still here, so hours are equal, let's look
        //at the minutes
        if (this->_m > other._m) {
            return true;
        } else if (other._m > this->_m) {
            return false;
        }

        //still here, so minutes are equal too
        //let's look at the seconds:
        if (this->_s > other._s) {
            return true;
        } else if (other._s > this->_s) {
            return false;
        }

        //still here, so seconds are equal too
        //let's look at the fractions:
        return ( this->_f > other._f);
    }

    void MyTime::Copy(const MyTime & other) {
        _h = other._h;
        _m = other._m;
        _s = other._s;
        _f = other._f;
    }

    void MyTime::SetTime(int hour, int minute, int second, int fraction) {
        _h = hour;
        _m = minute;
        _s = second;
        _f = fraction;
    }

    const string & MyTime::ToString() const {
        if (_toString.length() == 0) {
            ConvertToString();
        }
        return _toString;
    }

    void MyTime::ConvertToString() const {
        char buf[10];
        Compat::intToCString(buf, 10, "%02d.", _h);
        //sprintf( buf, "%02d.", _h );
        _toString.append(buf);
        Compat::intToCString(buf, 10, "%02d.", _m);
        //sprintf( buf, "%02d.", _m );
        _toString.append(buf);
        Compat::intToCString(buf, 10, "%02d.", _s);
        //sprintf( buf, "%02d.", _s );
        _toString.append(buf);
        Compat::intToCString(buf, 10, "%06d", _f);
        //sprintf( buf, "%06d", _f );
        _toString.append(buf);
    }

	MyTimestamp::MyTimestamp( bool setToCurrentTS ) {
		if( setToCurrentTS ) {
			time_t now;
			struct tm tmNow;
			time(&now);
			Compat::localtime(&tmNow, &now);
			SetTimestamp( 1900 + tmNow.tm_year, tmNow.tm_mon + 1, tmNow.tm_mday, 
						  tmNow.tm_hour, tmNow.tm_min, tmNow.tm_sec, 0 );
		}
	}

    MyTimestamp::MyTimestamp(int year, int month, int day, int hour, int minute, int second, int fraction) {
        SetTimestamp(year, month, day, hour, minute, second, fraction);
    }

    MyTimestamp::MyTimestamp(const MyTimestamp & other) {
        Copy(other);
    }


	MyTimestampPtr MyTimestamp::FromIsoString( const char *pIso ) {
		MyTimestampPtr pTs( new MyTimestamp() );
		string iso( pIso );
		int y = atoi( iso.substr( 0, 4 ).c_str() );
		int M = atoi( iso.substr( 5, 2 ).c_str() );
		int d = atoi( iso.substr( 8, 2 ).c_str() );
		int h = atoi( iso.substr( 11, 2 ).c_str() );
		int m = atoi( iso.substr( 14, 2 ).c_str() );
		int s = atoi( iso.substr( 17, 2 ).c_str() );
		long n = atol( iso.substr( 20 ).c_str() );
		pTs->SetTimestamp( y, M, d, h, m, s, n );
		return pTs;
	}


    MyTimestamp& MyTimestamp::operator=(const MyTimestamp& other) {
        if (this == &other) return *this;
        Copy(other);
        return *this;
    }

    bool MyTimestamp::operator==(const MyTimestamp& other) const {
        return ( (_date == other._date) &&
                (_time == other._time));
    }

    bool MyTimestamp::operator<(const MyTimestamp& other) const {
        if (this->_date < other._date) {
            return true;
        } else if (other._date < this->_date) {
            return false;
        }

        //still here, so dates are equal, let's look
        //at the time       
        return ( this->_time < other._time);
    }

    bool MyTimestamp::operator>(const MyTimestamp& other) const {
        if (this->_date > other._date) {
            return true;
        } else if (other._date > this->_date) {
            return false;
        }

        //still here, so dates are equal, let's look
        //at the time
        return ( this->_time > other._time);
    }


    void MyTimestamp::Copy(const MyTimestamp & other) {
        _date = other._date;
        _time = other._time;
    }

    void MyTimestamp::SetTimestamp(int year, int month, int day, int hour, int minute, int second, int fraction) {
        _date.SetDate(day, month, year);
        _time.SetTime(hour, minute, second, fraction);
    }

    const string & MyTimestamp::ToIsoString() const {
        if (_toIsoString.length() == 0) {
            ConvertToIsoString();
        }
        return _toIsoString;
    }

    const string & MyTimestamp::ToEurString() const {
        if (_toEurString.length() == 0) {
            ConvertToEurString();
        }
        return _toEurString;
    }

    void MyTimestamp::ConvertToIsoString() const {
        _toIsoString.append( _date.ToIsoString() );
        _toIsoString.append("-");
        _toIsoString.append( _time.ToString() );
    }

    void MyTimestamp::ConvertToEurString() const {
        _toEurString.append( _date.ToEurString() );
        _toEurString.append(" ");
        _toEurString.append( _time.ToString() );
    }

}

#endif // MYDATE_HPP
