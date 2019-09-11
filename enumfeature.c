#include <stdio.h>
#include <sensors/sensors.h>

int main(void)
{
	FILE *conffile;
    sensors_chip_name const * cn;
    int c = 0;

	conffile = fopen("/etc/sensors3.conf", "r");
	sensors_init(conffile);

    while ((cn = sensors_get_detected_chips(0, &c)) != 0) {
        printf("Chip: %s/%s\n", cn->prefix, cn->path);

        sensors_feature const *feat;
        int f = 0;

        while ((feat = sensors_get_features(cn, &f)) != 0) {
            printf("%d: %s\n", f, feat->name);

            sensors_subfeature const *subf;
            int s = 0;

            while ((subf = sensors_get_all_subfeatures(cn, feat, &s)) != 0) {
                printf("%d:%d:%s/%d = ", f, s, subf->name, subf->number);

                double val;
                if (subf->flags & SENSORS_MODE_R) {
                    int rc = sensors_get_value(cn, subf->number, &val);
                    if (rc < 0) {
                        printf("err: %d", rc);
                    } else {
                        printf("%f", val);
                    }
                }
                printf("\n");
            }
        }
    }

    return 0;
}