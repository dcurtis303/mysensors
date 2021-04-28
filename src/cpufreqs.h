#ifndef _CPUFREQS_h_
#define _CPUFREQS_h_

#include <vector>

class Cpufreqs
{
    std::vector<double> freqs;

    double low;
    double high;
    double elow;
    double ehigh;
    double mean;

public:
    void init(void);
    void read(void);
    void getdata(char reset);
    int getcount(void);
    double getlow() { return low; };
    double gethigh() { return high; };
    double getelow() { return elow; };
    double getehigh() { return ehigh; };
    double getmean() { return mean; };

    double &operator[](int index) { return freqs[index]; };
};

#endif /*_CPUFREQS_h_ */
