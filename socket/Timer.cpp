
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

void Timer::busyWaitMilliseconds(int waitMilliseconds) {
    clock_t startClock = clock();
    clock_t waitClocks = waitMilliseconds * (CLOCKS_PER_SEC / 1000);
    while ((clock() - startClock) < waitClocks) {}
}