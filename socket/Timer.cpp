
#include "Timer.hpp"

clock_t Timer::startTimer()
{
    return clock();
}

double Timer::calculateTime(clock_t start)
{
    clock_t end = clock();
    return static_cast<double>(end - start) / CLOCKS_PER_SEC;
}