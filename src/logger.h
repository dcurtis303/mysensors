#ifndef _LOGGER_H_
#define _LOGGER_H_

class Logger
{
    const char *logname = "/tmp/mysensors.log";

public:
    void init(void);
    void clear(void) { init(); };
    void debug(const char *message);
};

#endif /* _LOGGER_H_ */