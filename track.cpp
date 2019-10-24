#include "track.h"

void Track::getchip(void)
{
    sensors_chip_name const *cn;
    int c = 0;

    while ((cn = sensors_get_detected_chips(0, &c)) != 0)
    {
        sensors_feature const *feat;
        int f = 0;

        while ((feat = sensors_get_features(cn, &f)) != 0)
        {
            if (feat->type != SENSORS_FEATURE_TEMP)
                continue;

            sensors_subfeature const *subf;
            int s = 0;

            while ((subf = sensors_get_all_subfeatures(cn, feat, &s)) != 0)
            {
                if (subf->type != SENSORS_SUBFEATURE_TEMP_INPUT)
                    continue;

                printf("%s:%s:%s:%d = ", 
                    cn->prefix, feat->name, subf->name, subf->number);

                addtrack(cn, subf->number);

                double val;
                if (subf->flags & SENSORS_MODE_R)
                {
                    int rc = sensors_get_value(cn, subf->number, &val);
                    if (rc < 0)
                    {
                        printf("err: %d", rc);
                    }
                    else
                    {
                        printf("%f", val);
                    }
                }
                printf("\n");
            }
        }
    }
}

void Track::addtrack(const sensors_chip_name *cn, int number)
{
    items[itemcount].chip = cn;
    items[itemcount].number = number;
    itemcount++;
}