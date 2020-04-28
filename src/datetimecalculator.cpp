#ifndef DATETIMECALCULATOR_HPP
#define DATETIMECALCULATOR_HPP

#include <time.h>
//#include <math.h>
#include "my/datetimecalculator.h"
#include "my/compat.h"

namespace my {

    /**
        Gets the duration in days from a start to an end date.
        Both start and end day will be respected, e.g.:
        Start date = 2011-01-01
        End date   = 2011-01-02
        => Duration = 2 days.
     */
    int DateTimeCalculator::GetDuration(const MyDate & d1, const MyDate & d2) {
        time_t tt1, tt2;
        tm tm1, tm2;
        if (d1 < d2) {
            ProvideTm(d1, tm1);
            ProvideTm(d2, tm2);
        } else {
            if (d1 == d2) {
                return 1;
            }
            ProvideTm(d2, tm1);
            ProvideTm(d1, tm2);
        }

        tt1 = mktime(&tm1);
        tt2 = mktime(&tm2);

        double days = (difftime(tt2, tt1) / (double) SEC_PER_DAY);
        days += 0.5;
        return (int) days + 1;
    }

    /**
     * returns the number of days by adding all days of 
     * the concerned months.
     * Lap years are ignored, so february will always be assumed
     * to having 28 days.
     * @param startMonth: 1 to 12
     * @param startYear
     * @param endMonth: 1 to 12
     * @param endYear
     */
    int DateTimeCalculator::GetDays(int startMonth, int startYear, int endMonth, int endYear) {
        int days = 0;
        for (int y = startYear; y <= endYear; y++) {
            int mmax = y < endYear ? 12 : endMonth;
            for (int m = startMonth; m <= mmax; m++) {
                days += DAYS_PER_MONTH[m];
            }
            startMonth = 1;
        }
        return days;
    }

    /**
            Returns the number of concerned months.
            All effected months will be counted.
            e.g. startDate = 2011-01-31
                     endDate   = 2011-02-01
            ==> method will return 2.
     */
    int DateTimeCalculator::GetMonths(const MyDate & startDate, const MyDate & endDate) {
        int n = 0;
        int m1 = startDate.GetMonth();
        int m2;
        for (int y = startDate.GetYear(), ymax = endDate.GetYear(); y <= ymax; y++) {
            m2 = y < ymax ? 12 : endDate.GetMonth();
            for (; m1 <= m2; m1++) {
                n++;
            }
            m1 = 1;
        }
        return n;
    }

    /**
            Adds or subtracts an amount of days, months or years to or from 
            a given date.
            @ mydate: Date to compute with
            @ unit: days, months or years
            @ delta: number of units to add/subtract from mydate. 
                            Values > 0 will be added, values < 0 will be subtracted
                            from mydate
            @ returns: mydate
     */
    MyDate & DateTimeCalculator::Compute(MyDate & mydate, Unit unit, int delta) {
        switch (unit) {
            case DAY:
                return ComputeDays(mydate, delta);
            case MONTH:
                return ComputeMonths(mydate, delta);
            default:
                return ComputeYears(mydate, delta);
        }
    }

    /**
            Adds or subtracts an amount of days from 
            a given date.
            @ mydate: Date to compute with			
            @ delta: number of days to add/subtract from mydate. 
                            Values > 0 will be added, values < 0 will be subtracted
                            from mydate
            @ returns: mydate
     */
    MyDate & DateTimeCalculator::ComputeDays(MyDate & mydate, int delta) {
        int d = mydate.GetDay();
        int m = mydate.GetMonth();
        int y = mydate.GetYear();
        int dmax = MyDate::GetNumberOfDays(m, y);
        int smd = delta > 0 ? 1 : -1;
        int nmax = delta * smd;

        for (int n = 0; n < nmax; n++) {
            d += smd;
            if (d > dmax || d < 1) {
                m += smd;
                if (m > 12 || m < 1) {
                    m = (delta > 0) ? 1 : 12;
                    y += smd;
                }

                dmax = MyDate::GetNumberOfDays(m, y);
                d = (delta > 0) ? 1 : dmax;
            }
        }

        mydate.SetDay(d);
        mydate.SetMonth(m);
        mydate.SetYear(y);
        return mydate;
    }

    /**
            Adds or subtracts an amount of months from 
            a given date.
            @ mydate: Date to compute with			
            @ delta: number of months to add/subtract from mydate. 
                            Values > 0 will be added, values < 0 will be subtracted
                            from mydate
            @ returns: mydate
     */
    MyDate & DateTimeCalculator::ComputeMonths(MyDate & mydate, int delta) {
        int m = mydate.GetMonth();
        int y = mydate.GetYear();
        for (int d = 0; d < delta; d++) {
            m++;
            if (m > 12) {
                m = 1;
                y++;
            }
        }
        mydate.SetMonth(m);
        mydate.SetYear(y);
        return mydate;
    }

    /**
            Adds or subtracts an amount of years from 
            a given date.
            @ mydate: Date to compute with			
            @ delta: number of years to add/subtract from mydate. 
                            Values > 0 will be added, values < 0 will be subtracted
                            from mydate
            @ returns: mydate
     */
    MyDate & DateTimeCalculator::ComputeYears(MyDate & mydate, int delta) {
        mydate.SetYear(mydate.GetYear() + delta);
        return mydate;
    }

    void DateTimeCalculator::ProvideTm(const MyDate & date, tm & rtm) {
        rtm.tm_year = date.GetYear() - 1900;
        rtm.tm_mon = date.GetMonth() - 1;
        rtm.tm_mday = date.GetDay();
        //            if( useMidnight ) {
        //                rtm.tm_hour = 23;
        //                rtm.tm_min = 59;
        //                rtm.tm_sec = 59;
        //
        //            } else {
        rtm.tm_hour = 0;
        rtm.tm_min = 0;
        rtm.tm_sec = 0;
        //            }
    }


}

#endif // DATETIMECALCULATOR_HPP
