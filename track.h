#ifndef _TRACK_h_
#define _TRACK_h_

#include <vector>

#include <sensors/sensors.h>

struct item
{
    const sensors_chip_name *chip;
    const sensors_subfeature *subf;
    int number;
    double val;
    double low;
    double high;
};

class Track
{
    std::vector<item> items;

public:
    void initchips(void);
    void addtrack(const sensors_chip_name *, const sensors_subfeature *);

    int getcount() { return items.size(); };
    item &operator[](int index) { return items[index]; };
};

#endif /*_TRACK_h_ */
