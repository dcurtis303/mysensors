#include <stdio.h>
#include <string.h>

#include "cpufreqs.h"
#include "logger.h"

extern Logger logger;

void Cpufreqs::init(void)
{
    low = 10000.00;
    high = 0.00;
    elow = 10000.00;
    ehigh = 0.00;

    char buffer[1025];
    FILE *fcpu = fopen("/proc/cpuinfo", "r");
    do
    {
        fgets(buffer, 1024, fcpu);
        if (strstr(buffer, "MHz") != NULL)
            freqs.push_back(0.0f);
    } while (!feof(fcpu));
    fclose(fcpu);

    char logbuf[128];
    sprintf(logbuf, "number of CPUs: %d\n", getcount());
    logger.debug(logbuf);
}

void Cpufreqs::read(void) 
{
    int i = 0;    
    char buffer[1025];
    double val;

    FILE *fcpu = fopen("/proc/cpuinfo", "r");
    do
    {
        fgets(buffer, 1024, fcpu);
        if (strstr(buffer, "MHz") != NULL)
        {
            sscanf(buffer, "cpu MHz		: %lf\n", &val);
            freqs[i] = val;
            i++;
        }
    } while (!feof(fcpu));
    fclose(fcpu);
}

void Cpufreqs::getdata(char reset)
{
    unsigned int i;

    low = 10000.00;
    high = 0.00;
    mean = 0.00;
    if (reset)
    {
        elow = 10000.00;
        ehigh = 0.00;
    }

    for (i = 0; i < freqs.size(); i++)
    {
        if (freqs[i] < low)
            low = freqs[i];

        if (freqs[i] > high)
            high = freqs[i];

        if (freqs[i] < elow)
            elow = freqs[i];

        if (freqs[i] > ehigh)
            ehigh = freqs[i];

        mean += freqs[i];
    }

    mean /= freqs.size();
}

int Cpufreqs::getcount(void)
{
    return freqs.size();
}