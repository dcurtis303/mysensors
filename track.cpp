#include <iostream>
#include <cstdlib>
#include <cstring>
#include <libconfig.h++>

#include "track.h"

using namespace std;
using namespace libconfig;


int Track::readcfg(void)
{
    Config cfg;

    try
    {
        string const HOME = std::getenv("HOME") ? std::getenv("HOME") : ".";
        string cfgfilepath = HOME + "/.config/mysensors.cfg";
        cfg.readFile(cfgfilepath.c_str());
    }
    catch (const FileIOException &fioex)
    {
        std::cerr << "I/O error while reading file." << std::endl;
        return (EXIT_FAILURE);
    }
    catch (const ParseException &pex)
    {
        std::cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine()
                  << " - " << pex.getError() << std::endl;
        return (EXIT_FAILURE);
    }

    const Setting &root = cfg.getRoot();

    try
    {
        const Setting &books = root["sensors"];
        int count = books.getLength();

        cout << "count of sensors: " << count << endl;

        for (int i = 0; i < count; ++i)
        {
            const Setting &book = books[i];

            string chip, subf;
            int subfnum;

            if (!(book.lookupValue("chip", chip)))
                continue;

            if (!(book.lookupValue("subf", subf)))
                continue;

            if (!(book.lookupValue("subfnum", subfnum)))
                continue;

            cout << "chip: " << chip;
            cout << "\tsubf: " << subf;
            cout << "\tsubfnum: " << subfnum << endl;

            addtrack(chip, subf);
        }
    }
    catch (const SettingNotFoundException &nfex)
    {
        // Ignore.
    }

    return (EXIT_SUCCESS);
}

int test_write(void)
{
    string const HOME = std::getenv("HOME") ? std::getenv("HOME") : ".";
    string cfgfilepath = HOME + "/.config/mysensors.cfg";
    static const char *output_file = cfgfilepath.c_str();

    Config cfg;

    Setting &root = cfg.getRoot();

    // Add some settings to the configuration.
    Setting &sensors = root.add("sensors", Setting::TypeList);

    sensors_chip_name const *cn;
    int c = 0;

    while ((cn = sensors_get_detected_chips(0, &c)) != 0)
    {
        sensors_feature const *feat;
        int f = 0;

        while ((feat = sensors_get_features(cn, &f)) != 0)
        {
            sensors_subfeature const *subf;
            int s = 0;

            while ((subf = sensors_get_all_subfeatures(cn, feat, &s)) != 0)
            {
                Setting &g1 = sensors.add(Setting::TypeGroup);
                g1.add("chip", Setting::TypeString) = cn->prefix;
                g1.add("subf", Setting::TypeString) = subf->name;
                g1.add("subfnum", Setting::TypeInt) = subf->number;
            }
        }
    }

    // Write out the new configuration.
    try
    {
        cfg.writeFile(output_file);
        cerr << "New configuration successfully written to: " << output_file
             << endl;
    }
    catch (const FileIOException &fioex)
    {
        cerr << "I/O error while writing file: " << output_file << endl;
        return (EXIT_FAILURE);
    }

    return (EXIT_SUCCESS);
}

void enumfeature(void)
{
    sensors_chip_name const *cn;
    int c = 0;

    while ((cn = sensors_get_detected_chips(0, &c)) != 0)
    {
        printf("Chip: %s:%s\n", cn->prefix, cn->path);

        sensors_feature const *feat;
        int f = 0;

        while ((feat = sensors_get_features(cn, &f)) != 0)
        {
            printf("%d:%s\n", f, feat->name);

            sensors_subfeature const *subf;
            int s = 0;

            while ((subf = sensors_get_all_subfeatures(cn, feat, &s)) != 0)
            {
                printf("%d:%d:%s:%d = ", f, s, subf->name, subf->number);

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

void Track::initchips(void)
{
    readcfg();
    printf("press <ENTER> to continue...");
    getchar();
}

void Track::addtrack(string chip, string subf)
{
    sensors_chip_name const *cn;
    int c = 0;

    while ((cn = sensors_get_detected_chips(0, &c)) != 0)
    {
        if (strcmp(chip.c_str(), cn->prefix) != 0)
            continue;

        sensors_feature const *feat;
        int f = 0;

        while ((feat = sensors_get_features(cn, &f)) != 0)
        {
            sensors_subfeature const *sf;
            int s = 0;

            while ((sf = sensors_get_all_subfeatures(cn, feat, &s)) != 0)
            {
                if (strcmp(subf.c_str(), sf->name) != 0)
                    continue;
         
                item *i = new item;
                i->chip = cn;
                i->subf = sf;
                i->val = 0.0f;
                i->low = 0.0f;
                i->high = 0.0f;
                items.push_back(*i);
            }
        }
    }
}
