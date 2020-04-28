/* 
 * File:   timer.h
 * Author: testalucida
 *
 * Created on 21. November 2011, 11:03
 */

#ifndef TIMER_H
#define	TIMER_H
#include <cstdlib>
#include <ctime>

namespace my {

    class Timer {
    public:
        Timer();

        void start();
        void stop();
        double duration();
        const char *durationToString();

        ~Timer();
    private:
        clock_t _start, _stop;
        double _duration;
        char *_pBuf;
    };

}

#endif	/* TIMER_H */

