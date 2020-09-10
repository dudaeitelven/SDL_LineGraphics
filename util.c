#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

int randomNumber(int nr_min, int nr_max)
{
    static bool initialized = false;
    if(!initialized)
    {
        initialized = true;
        srand(time(NULL));
    }

    return rand() % nr_max + nr_min;
}

int randomColor()
{
    return randomNumber(0,255);
}
