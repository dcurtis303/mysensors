#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <sensors/sensors.h>
#include <pthread.h>
#include <ncurses.h>

#include "mysensors.h"

struct Temps temps[MAX_SENSORS];
struct CPU cpu[1];
struct MEM mem[MAX_MEM_ITEMS];

struct timeval start_timeval;
struct timeval cur_timeval;
long long mstime, mstime_last;

int delays[] = {250000, 500000, 1000000, 5000000};
int num_delays = sizeof(delays) / sizeof(int);
int cur_delay = 0;
int delay;

void *PollKbd(void *info)
{
	struct Common *cptr = (struct Common *)info;
	int ch;

	do
	{
		ch = getchar();
		if (ch == 'r')
			cptr->mReset = true;
		if (ch == 'q')
			cptr->mRunning = false;
		if (ch == 'd')
			if (++cur_delay >= num_delays)
				cur_delay = 0;
		if (ch == 'D')
			if (--cur_delay < 0)
				cur_delay = num_delays - 1;
	} while (ch != 'q');

	return (void *)0;
}

void *Worker(void *info)
{
	struct Common *cptr = (struct Common *)info;
	int num_sensors;

	do
	{

		num_sensors = do_read_temps(cptr->mReset);
		do_read_cpu();
		do_read_mem();
		do_print(num_sensors);

		if (cptr->mReset != 0)
		{
			gettimeofday(&start_timeval, NULL);
			cptr->mReset = 0;
		}

		usleep(delays[cur_delay]);
	} while (cptr->mRunning != false);

	return (void *)0;
}

int do_read_temps(char reset)
{
	const sensors_chip_name *chip;
	sensors_feature const *feat;
	sensors_subfeature const *subf;
	int nr = 0, i;
	int f = 0;
	int s = 0;
	int rc;
	double val;

	while ((chip = sensors_get_detected_chips(NULL, &nr)) != NULL)
	{
		i = nr - 1;

		//printf("Chip: %s/%s\n", chip->prefix, chip->path);
		strncpy(temps[i].name, chip->prefix, CHIP_NAME_MAXLENGTH);
		//if (temps[i].name[0] == 0)
		//	strncpy(temps[i].name, chip->path, CHIP_NAME_MAXLENGTH);

		while ((feat = sensors_get_features(chip, &f)) != 0)
		{
			//printf("%d: %s\n", f, feat->name);

			while ((subf = sensors_get_all_subfeatures(chip, feat, &s)) != 0)
			{
				//if (strcmp(subf->name, "temp1_input") == 0)
				{
					if (subf->flags & SENSORS_MODE_R)
					{
						rc = sensors_get_value(chip, subf->number, &val);
						if (rc < 0) {
							temps[i].val = -1.0f;
						} else {
							temps[i].val = val;
						}

						if (temps[i].val < temps[i].low || temps[i].low == 0 || reset != 0)
							temps[i].low = temps[i].val;
						if (temps[i].val > temps[i].high || reset != 0)
							temps[i].high = temps[i].val;
					}
				}
			}
		}
	}

	return nr;
}

void do_read_cpu(void)
{
	FILE *fstat;
	long tuser, tnice, tsystem, tidle;
	long tiowait, tirq, tsoftirq, tsteal;
	long tguest, tguest_nice;

	fstat = fopen("/proc/stat", "r");

	mstime_last = mstime;
	gettimeofday(&cur_timeval, NULL);
	mstime = (((long long)cur_timeval.tv_sec) * 1000) + (cur_timeval.tv_usec / 1000);

	fscanf(fstat, "%s %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld",
		   cpu[0].name, &tuser, &tnice, &tsystem, &tidle,
		   &tiowait, &tirq, &tsoftirq, &tsteal,
		   &tguest, &tguest_nice);

	cpu[0].luser = cpu[0].user;
	cpu[0].lnice = cpu[0].nice;
	cpu[0].lsystem = cpu[0].system;
	cpu[0].lidle = cpu[0].idle;
	cpu[0].liowait = cpu[0].iowait;
	cpu[0].lirq = cpu[0].irq;
	cpu[0].lsoftirq = cpu[0].softirq;
	cpu[0].lsteal = cpu[0].steal;
	cpu[0].lguest = cpu[0].guest;
	cpu[0].lguest_nice = cpu[0].guest_nice;

	cpu[0].user = tuser;
	cpu[0].nice = tnice;
	cpu[0].system = tsystem;
	cpu[0].idle = tidle;
	cpu[0].iowait = tiowait;
	cpu[0].irq = tirq;
	cpu[0].softirq = tsoftirq;
	cpu[0].steal = tsteal;
	cpu[0].guest = tguest;
	cpu[0].guest_nice = tguest_nice;

	fclose(fstat);
}

void do_read_mem(void)
{
	FILE *fmem = fopen("/proc/meminfo", "r");
	for (int i = 0; i < MAX_MEM_ITEMS; i++)
		fscanf(fmem, "%s %ld kB\n", mem[i].key, &mem[i].value);
	fclose(fmem);
}

void do_print(int num)
{
	static bool first_print = true;

	if (first_print)
	{
		first_print = false;
		return;
	}
	clear();

	for (int i = 0; i < num; i++)
	{
		attroff(A_BOLD);
		attron(COLOR_PAIR(COLORPAIR_WHITE_BLACK));
		printw("%s\t: ", temps[i].name);
		if (i > MAX_SENSORS)
			printw("MAX_SENSORS exceeded\n");
		else
		{
			attron(COLOR_PAIR(COLORPAIR_WHITE_BLACK) | A_BOLD);
			printw("\t%*.1f", 4, temps[i].val);
			attron(COLOR_PAIR(COLORPAIR_GREEN_BLACK));
			printw("\t%*.1f", 4, temps[i].low);
			attron(COLOR_PAIR(COLORPAIR_RED_BLACK));
			printw("\t%*.1f\n", 4, temps[i].high);
		}
	}

	attroff(A_BOLD);
	attron(COLOR_PAIR(COLORPAIR_WHITE_BLACK));

	printw("time\t: ");
	attron(A_BOLD);
	printw("%ldms\n", mstime - mstime_last);

	attroff(A_BOLD);
	printw("up\t: ");
	long diff = cur_timeval.tv_sec - start_timeval.tv_sec;
	int hr = diff / 3600;
	int min = diff % 3600 / 60;
	int sec = diff % 60;
	attron(A_BOLD);
	printw("%02d:%02d:%02d\n", hr, min, sec);

	attroff(A_BOLD);
	printw("%s\t: ", cpu[0].name);

	attroff(A_BOLD);
	attron(COLOR_PAIR(COLORPAIR_YELLOW_BLACK));
	printw("%4ld %4ld %4ld %4ld %4ld %4ld %4ld %4ld %4ld %4ld\n",
		   cpu[0].user - cpu[0].luser,
		   cpu[0].nice - cpu[0].lnice,
		   cpu[0].system - cpu[0].lsystem,
		   cpu[0].idle - cpu[0].lidle,
		   cpu[0].iowait - cpu[0].liowait,
		   cpu[0].irq - cpu[0].lirq,
		   cpu[0].softirq - cpu[0].lsoftirq,
		   cpu[0].steal - cpu[0].lsteal,
		   cpu[0].guest - cpu[0].lguest,
		   cpu[0].guest_nice - cpu[0].lguest_nice);

	long t1 = cpu[0].luser + cpu[0].lnice + cpu[0].lsystem + cpu[0].lidle +
			  cpu[0].liowait + cpu[0].lirq + cpu[0].lsoftirq + cpu[0].lsteal +
			  cpu[0].lguest + cpu[0].lguest_nice;
	long t2 = cpu[0].user + cpu[0].nice + cpu[0].system + cpu[0].idle +
			  cpu[0].iowait + cpu[0].irq + cpu[0].softirq + cpu[0].steal +
			  cpu[0].guest + cpu[0].guest_nice;
	long w1 = cpu[0].luser + cpu[0].lnice + cpu[0].lsystem;
	long w2 = cpu[0].user + cpu[0].nice + cpu[0].system;
	float wpd = (float)w2 - w1;
	float tpd = (float)t2 - t1;
	float usage = wpd / tpd * 100.0f;
	attroff(A_BOLD);
	attron(COLOR_PAIR(COLORPAIR_WHITE_BLACK));
	printw("cpu\t: ");
	attron(A_BOLD);
	printw("%*.1f%\n", 3, usage);
	attroff(A_BOLD);

	printw("mem\t: ");
	attron(A_BOLD);
	usage = (float)(mem[0].value - mem[2].value) / mem[0].value * 100.0f;
	printw("%*.1f%\n", 3, usage);

	attroff(A_BOLD);
	printw("-----------------------------------------------------------\n"
		   "press 'q' to quit\n      'r' to reset\n      'd' to change delay\n");

	refresh();
}

void *LogThread(void *info)
{
	return (void *)0;
}

int enum_features(void)
{
    sensors_chip_name const * cn;
	sensors_feature const *feat;
	sensors_subfeature const *subf;
    int c = 0;
	int f = 0;
	int s = 0;


    while ((cn = sensors_get_detected_chips(0, &c)) != 0) {
        while ((feat = sensors_get_features(cn, &f)) != 0) {
			if (strcmp("temp1", feat->name) == 0) {
				printf("found temp1 feature on %s\n", cn->prefix);
			} else {
				continue;
			}

            while ((subf = sensors_get_all_subfeatures(cn, feat, &s)) != 0) {
				if (strcmp("temp1_input", subf->name) == 0) {
					printf("found temp1_input subfeature on %s\n", cn->prefix);
				} else {
					continue;
				}
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

int main(void)
{
	pthread_t workThread, pollThread;
	struct Common common = {true, false};
	FILE *conffile;

	conffile = fopen("/etc/sensors3.conf", "r");
	sensors_init(conffile);
	fclose(conffile);

	enum_features();
	printf("\nPress <enter> to continue...");
	getchar();

	initscr();
	//curs_set(0);
	start_color();
	init_pair(COLORPAIR_WHITE_BLACK, COLOR_WHITE, COLOR_BLACK);
	init_pair(COLORPAIR_GREEN_BLACK, COLOR_GREEN, COLOR_BLACK);
	init_pair(COLORPAIR_RED_BLACK, COLOR_RED, COLOR_BLACK);
	init_pair(COLORPAIR_YELLOW_BLACK, COLOR_YELLOW, COLOR_BLACK);

	delay = delays[cur_delay];

	gettimeofday(&start_timeval, NULL);

	pthread_create(&pollThread, 0, PollKbd, &common);
	pthread_create(&workThread, 0, Worker, &common);
	pthread_join(pollThread, 0);
	pthread_join(workThread, 0);

	endwin();

	printf("done\n");

	return 0;
}