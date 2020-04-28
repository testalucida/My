#ifndef DATETIME_H_INCLUDED
#define DATETIME_H_INCLUDED

#include <string>
#include <memory>

using namespace std;

static const char *MONTHNAME[] = {"none", "Januar", "Februar", "März", "April", "Mai", "Juni", "Juli",
    "August", "September", "Oktober", "November", "Dezember"};

static int DAYS_PER_MONTH[] = {-1, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
static int LEAPYEAR[] = {2000, 2004, 2008, 2012, 2016, 2020, 2024, 2028, 2032};

namespace my {

    class MyDate {
    public:
        /** Construction */
        MyDate(bool setToCurrentDate = false);
        MyDate(int d, int m, int y);
		MyDate( const char * );
        MyDate(const MyDate& other);
		
        /**
         * changes this MyDate to the given date.
         * @param other: the date to which this date will be set
         */
        void FromDate( const MyDate &other );
        
        /**
         * changes this MyDate to the given date.
         * @param date: must be provided by YYYY-MM-DD (Iso) 
         * or DD.MM.YYYY (Eur)
         */
        void FromString( const string &date );
        
        /**
         * changes this MyDate to the given date.
         * @param date: must be provided by YYYY-MM-DD (Iso) 
         */
        void FromIsoString( const string &date );
        
        /**
         * changes this MyDate to the given date.
         * @param date: must be provided by DD.MM.YYYY (Eur)
         */
        void FromEurString( const string &date );

        /**
                provides a date, contained in a string, into a given MyDate instance.
                The string representation of date has to be ISO formatted, i.e. "YYYY-MM-DD"
         */
        static MyDate& FromIsoString(MyDate &date, const string &isodate);

        /**
                provides a date, contained in a string, into a given MyDate instance.
                The string representation of date has to be EUR formatted, i.e. "DD.MM.YYYY"
         */
        static MyDate& FromEurString(MyDate &date, const string &eurdate);

        /**
                provides a date, contained in a string, into a given MyDate instance.
                The string representation of date has to be either EUR  or ISO formatted, 
                i.e. "DD.MM.YYYY" or "YYYY-MM-DD"
         */
        static MyDate& FromString(MyDate &date, const string &strdate);

        /**
                provides a date, contained in a c-string, into a given MyDate instance.
                The string representation of date has to be either EUR  or ISO formatted, 
                i.e. "DD.MM.YYYY" or "YYYY-MM-DD"
         */
        static MyDate& FromString(MyDate &date, const char *strdate);

        /** Destruction */
        virtual ~MyDate() {
        }

        MyDate& operator=(const MyDate& other);

        /** operators */
        bool operator==(const MyDate& other) const;
        bool operator<(const MyDate& other) const;
        bool operator>(const MyDate& other) const;
        bool operator <=(const MyDate &other) const;
		bool operator >=(const MyDate &other) const;

        friend ostream& operator <<(ostream& os, MyDate dt) {
            os << dt.ToIsoString();
            return os;
        }

        /** public methods and functions */
        void SetCurrentDate();
        void SetDate(int d, int m, int y);
		void SetDate( const char * );
        void SetDay(int d);
        void SetMonth(int m);
        void SetYear(int y);
        
        int GetDay() const;
        int GetWeekday();
        int GetWeekNumber( const char *pFormat = "%U" ) const;
        int GetMonth() const;      
        int GetYear() const;
        bool IsSet() const;
        const char * GetMonthName() const;
        static int GetNumberOfDays(int month, int year);
        /**
                if the given year is a leap year
         */
        static bool IsLeapYear(int year);
        const string & ToIsoString() const;
        const string & ToEurString() const;

    protected:

        void Copy(const MyDate & other);

    private:
        int _d, _m, _y;
        mutable string _sIsoDate, _sEurDate;
        mutable bool _isIsoFormatted, _isEurFormatted;

        void SetFormatted(bool formatted);
    };

	class MyTime;
	typedef std::shared_ptr< MyTime > MyTimePtr;

    class MyTime {
    public:

        /** construction */
        MyTime() {
        }
        MyTime(int hour, int minute, int second, int fraction);
        MyTime(const MyTime & other);
		/** returns a time-pointer built from a given c-string.
            The c-string's format has to be "HH.mm.ss.nnnnnn"  */
		static MyTimePtr FromString( const char * );
        /** Destruction */
        virtual ~MyTime() {
        }

        /** operators */
        MyTime& operator=(const MyTime& other);
        bool operator==(const MyTime& other) const;
        bool operator<(const MyTime& other) const;
        bool operator>(const MyTime& other) const;

        friend ostream& operator <<(ostream& os, MyTime t) {
            os << t.ToString();
            return os;
        }

        /** methods */
        void SetTime(int hour, int minute, int second, int fraction);
        virtual const string & ToString() const;

    protected:
        void Copy(const MyTime & other);
        virtual void ConvertToString() const;

    private:
        int _h, _m, _s, _f /*fraction*/;
        mutable string _toString;

    };

	class MyTimestamp;
	typedef std::shared_ptr< MyTimestamp > MyTimestampPtr;
	class CharBuffer;

    class MyTimestamp {
    public:
        /** construction */
        MyTimestamp( bool setToCurrentTS = false );
        MyTimestamp(int year, int month, int day, int hour, int minute, int second, int fraction);
        MyTimestamp(const MyTimestamp & other);
		/** returns a timestamp-pointer built from a given c-string.
            The c-string's format has to be ISO formatted, i.e. "YYYY-MM-DD-HH.mm.ss.nnnnnn"  */
		static MyTimestampPtr FromIsoString( const char * );
        /** destruction */
        virtual ~MyTimestamp() {
        }

        /** operators */
        MyTimestamp& operator=(const MyTimestamp& other);
        bool operator==(const MyTimestamp& other) const;
        bool operator<(const MyTimestamp& other) const;
        bool operator>(const MyTimestamp& other) const;

        friend ostream& operator <<(ostream& os, MyTimestamp ts) {
            os << ts.ToIsoString();
            return os;
        }

        /** methods*/
        void SetTimestamp(int year, int month, int day, int hour, int minute, int second, int fraction);
        virtual const string & ToIsoString() const;
        virtual const string & ToEurString() const;

    protected:
        void Copy(const MyTimestamp & other);
        virtual void ConvertToIsoString() const ;
        virtual void ConvertToEurString() const;

    private:
        MyDate _date;
        MyTime _time;
        mutable string _toIsoString;
        mutable string _toEurString;
    };

} //my

#endif // DATETIME_H_INCLUDED
