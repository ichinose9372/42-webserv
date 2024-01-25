#ifndef TIMER_HPP
#define TIMER_HPP

#include <ctime>

class Timer 
{
    public: 
        static clock_t startTimer();
        static double calculateTime(clock_t start);
};

#endif