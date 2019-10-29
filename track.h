#ifndef _TRACK_h_
#define _TRACK_h_

#include <vector>
#include <string>

#include <sensors/sensors.h>

struct item
{
    const sensors_chip_name *chip;
    const sensors_subfeature *subf;
    double val;
    double low;
    double high;
};

class Track
{
    std::vector<item> items;

public:
    void initchips(void);
    int readcfg(void);
    int writecfg(const char *);
    void load(void);
    void addtrack(std::string a, std::string b);
    int getcount() { return items.size(); };
    item &operator[](int index) { return items[index]; };
};

#endif /*_TRACK_h_ */
