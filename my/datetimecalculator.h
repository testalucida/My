/* 
 * File:   datetimecalculator.h
 * Author: testalucida
 *
 * Created on 21. November 2011, 11:16
 */

#ifndef DATETIMECALCULATOR_H
#define	DATETIMECALCULATOR_H

#include "my/datetime.h"

namespace my {

    enum Unit {
        DAY,
        MONTH,
        YEAR
    };

    class DateTimeCalculator {
        //doesn't work with Win: static const double SEC_PER_DAY = 86400.00;
        static const int SEC_PER_DAY = 86400;

    public:
        /**
            Gets the duration in days from a start to an end date.
            Both start and end day will be respected, e.g.:
            Start date = 2011-01-01
            End date   = 2011-01-02
            => Duration = 2 days.
         */
        static int GetDuration(const MyDate & d1, const MyDate & d2);

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
        static int GetDays(int startMonth, int startYear, int endMonth, int endYear);
        /**
                Returns the number of concerned months.
                All effected months will be counted.
                e.g. startDate = 2011-01-31
                         endDate   = 2011-02-01
                ==> method will return 2.
         */
        static int GetMonths(const MyDate & startDate, const MyDate & endDate);
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
        static MyDate & Compute(MyDate & mydate, Unit unit, int delta);

        /**
                Adds or subtracts an amount of days from 
                a given date.
                @ mydate: Date to compute with			
                @ delta: number of days to add/subtract from mydate. 
                                Values > 0 will be added, values < 0 will be subtracted
                                from mydate
                @ returns: mydate
         */
        static MyDate & ComputeDays(MyDate & mydate, int delta);

        /**
                Adds or subtracts an amount of months from 
                a given date.
                @ mydate: Date to compute with			
                @ delta: number of months to add/subtract from mydate. 
                                Values > 0 will be added, values < 0 will be subtracted
                                from mydate
                @ returns: mydate
         */
        static MyDate & ComputeMonths(MyDate & mydate, int delta);

        /**
                Adds or subtracts an amount of years from 
                a given date.
                @ mydate: Date to compute with			
                @ delta: number of years to add/subtract from mydate. 
                                Values > 0 will be added, values < 0 will be subtracted
                                from mydate
                @ returns: mydate
         */
        static MyDate & ComputeYears(MyDate & mydate, int delta);


    private:

        /** Default constructor not accessible */
        DateTimeCalculator() { }

        static void ProvideTm(const MyDate & date, tm & rtm);
    };

}


#endif	/* DATETIMECALCULATOR_H */

