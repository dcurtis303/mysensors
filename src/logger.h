#ifndef _LOGGER_H_
#define _LOGGER_H_

class Logger
{
    const char *logname = "mysensors.log";

public:
    void init(void);
    void debug(const char *message);
};

#endif /* _LOGGER_H_ */