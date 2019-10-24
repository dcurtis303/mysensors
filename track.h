#ifndef _TRACK_h_
#define _TRACK_h_

#include <sensors/sensors.h>

struct item
{
    const sensors_chip_name *chip;
    int number;
};

class Track
{
    item items[10];
    int itemcount;

public:
    Track() { itemcount = 0; };
    void getchip(void);
    void addtrack(const sensors_chip_name *cn, int number);
    int getcount() { return itemcount; };
    item &operator[](int index) { return items[index]; };
};

#endif /*_TRACK_h_ */
