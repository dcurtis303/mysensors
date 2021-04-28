#include <stdio.h>
#include <string.h>

#include "logger.h"

Logger logger;

void Logger::init(void)
{
    const char *message = "mysensors log: \n";
    FILE *fp = fopen(logname, "w");
    fwrite(message, 1, strlen(message), fp);
    fclose(fp);
}

void Logger::debug(const char *message)
{
    FILE *fp = fopen(logname, "a+");
    fwrite(message, 1, strlen(message), fp);
    fclose(fp);
}