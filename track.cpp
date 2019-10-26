#include <cstring>

#include "track.h"

void Track::initchips(void)
{
    sensors_chip_name const *cn;
    int c = 0;

    while ((cn = sensors_get_detected_chips(0, &c)) != 0)
    {
        sensors_feature const *feat;
        int f = 0;

        while ((feat = sensors_get_features(cn, &f)) != 0)
        {
            // if (feat->type != SENSORS_FEATURE_TEMP)
            //     continue;

            // if (strncmp("temp1", feat->name, 5) != 0)
            //     continue;

            sensors_subfeature const *subf;
            int s = 0;

            while ((subf = sensors_get_all_subfeatures(cn, feat, &s)) != 0)
            {
                // if (subf->type != SENSORS_SUBFEATURE_TEMP_INPUT)
                //     continue;

                addtrack(cn, subf);
            }
        }
    }
}

void Track::addtrack(const sensors_chip_name *cn, const sensors_subfeature *subf)
{
    item *i = new item;
    i->chip = cn;
    i->subf = subf;
    i->number = subf->number;
    i->val = 0.0f;
    i->low = 0.0f;
    i->high = 0.0f;
    items.push_back(*i);
}