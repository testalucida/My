#ifndef _TIMER_HPP
#define _TIMER_HPP

#include <time.h>
#include <cstdio>
#include "my/timer.h"

#ifndef CLOCKS_PER_SEC
#  define CLOCKS_PER_SEC  1000000l
#endif

namespace my {

    Timer::Timer() : _duration(0.000), _pBuf(  (char*) 0) {
    }

    void Timer::start() {
        _duration = 0.000;
        _start = clock();
    }

    void Timer::stop() {
        _stop = clock();
        _duration = (double) (_stop - _start) / CLOCKS_PER_SEC;
    }

    double Timer::duration() {
        return _duration;
    }

    const char *Timer::durationToString() {
        if (_pBuf) {
            delete[] _pBuf;
        }
        _pBuf = new char[15];
        sprintf(_pBuf, "%4.4f", _duration);
        return _pBuf;
    }

    Timer::~Timer() {
        if (_pBuf) {
            delete[] _pBuf;
        }
    }

} //namespace my

#endif 
