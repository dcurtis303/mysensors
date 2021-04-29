#include <iostream>
#include <cstdlib>
#include <cstring>

#include <libconfig.h++>
#include <sensors/sensors.h>

#include "track.h"

using namespace std;
using namespace libconfig;

#include "logger.h"

extern Logger logger;

int Track::readcfg(void)
{
    Config cfg;

    string const HOME = std::getenv("HOME") ? std::getenv("HOME") : ".";
    string cfgfilepath = HOME + "/.config/mysensors.cfg";

    char dbgmsg[1024];
    sprintf(dbgmsg, "using: %s\n", cfgfilepath.c_str());
    logger.debug(dbgmsg);

    try
    {
        cfg.readFile(cfgfilepath.c_str());
    }
    catch (const FileIOException &fioex)
    {
        writecfg(cfgfilepath.c_str());

        try
        {
            cfg.readFile(cfgfilepath.c_str());
        }
        catch (const exception &ex)
        {
            std::cerr << "FATAL error" << std::endl;
            return (EXIT_FAILURE);
        }
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

        for (int i = 0; i < count; ++i)
        {
            const Setting &book = books[i];

            string chip, subf;

            if (!(book.lookupValue("chip", chip)))
                continue;

            if (!(book.lookupValue("subf", subf)))
                continue;

            addtrack(chip, subf);
        }
    }
    catch (const SettingNotFoundException &nfex)
    {
        // Ignore.
    }

    return (EXIT_SUCCESS);
}

int Track::writecfg(const char *output_file)
{
    Config cfg;

    Setting &root = cfg.getRoot();

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
            }
        }
    }

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

void Track::initchips(void)
{
    readcfg();
}
