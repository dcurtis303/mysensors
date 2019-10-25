#ifndef _TRACK_h_
#define _TRACK_h_

#include <vector>

#include <sensors/sensors.h>

struct item
{
    const sensors_chip_name *chip;
    int number;
};

class Track
{
    std::vector<item> items;

public:
    void getchip(void);
    void addtrack(const sensors_chip_name *cn, int number);
    int getcount() { return items.size(); };
    item &operator[](int index) { return items[index]; };
};

#endif /*_TRACK_h_ */
