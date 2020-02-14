

/* not sure how this code is really c++ */

#include <unistd.h>
#include <sys/time.h>

#include <sensors/sensors.h>
#include <pthread.h>
#include <ncurses.h>

#include "mysensors.h"
#include "track.h"

Track track;

struct CPU cpu[1];
struct MEM mem[MAX_MEM_ITEMS];

bool first_print = true;

struct timeval start_timeval;
struct timeval cur_timeval;
long long mstime, mstime_last;

WINDOW *mainWindow;
int sensorListIndex = 0;

int delays[] = {50000, 125000, 250000, 500000, 1000000, 3000000};
const char *dlyprt[] = {"50ms", "125ms", "250ms", "500ms", "1s", "3s"};
int num_delays = sizeof(delays) / sizeof(int);
int cur_delay = 5;

void *PollKbd(void *info)
{
	struct Common *cptr = (struct Common *)info;
	int ch;

	do
	{
		do_print();
		
		ch = getchar();
		if (ch == 'k') // up arrow
			sensorListIndex = sensorListIndex > 0 ? sensorListIndex - 1 : sensorListIndex;
		if (ch == 'l') // down arrow
			sensorListIndex = sensorListIndex < track.getcount() ? sensorListIndex + 1 : sensorListIndex;
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
	} while (cptr->mRunning != false);

	return (void *)0;
}

void *Worker(void *info)
{
	struct Common *cptr = (struct Common *)info;

	while (cptr->mRunning != false)
	{
		do_read_temps(cptr->mReset);
		do_read_cpu();
		do_read_mem();
		do_print();

		if (cptr->mReset != 0)
		{
			gettimeofday(&start_timeval, NULL);
			cptr->mReset = 0;
		}

		if (!first_print)
			usleep(delays[cur_delay]);
	};

	return (void *)0;
}

void do_read_temps(char reset)
{
	int i;
	int rc;
	double val;

	for (i = 0; i < track.getcount(); i++)
	{
		rc = sensors_get_value(track[i].chip, track[i].subf->number, &val);
		if (rc < 0)
			track[i].val = -1.0f;
		else
			track[i].val = val;

		if (track[i].val < track[i].low || track[i].low == 0 || reset != 0)
			track[i].low = track[i].val;

		if (track[i].val > track[i].high || reset != 0)
			track[i].high = track[i].val;
	}
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

void do_print(void)
{
	if (first_print)
	{
		first_print = false;
		//return;
	}
	clear();

	attroff(A_BOLD);
	printw("up\t: ");
	long diff = cur_timeval.tv_sec - start_timeval.tv_sec;
	int day = diff / 3600 / 24;
	int hr = diff / 3600;
	int hrd = hr % 24;
	int min = diff % 3600 / 60;
	int sec = diff % 60;
	attron(A_BOLD);
	printw(" %02d:%02d:%02d  ", hr, min, sec);
	printw("(%d %s, %d %s)\n", 
		day, (day == 1 ? "day" : "days"), hrd, (hrd == 1 ? "hr" : "hrs"));

	attroff(A_BOLD);
	printw("interval: ");
	attron(A_BOLD);
	printw("%s\n", dlyprt[cur_delay]);

	attroff(A_BOLD);
	printw("\t    Us   Ni   Sy   Id   Iw   Ih   Is    S    G   Gn\n");

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
	float tpd = (float)t2 - t1;
	float uu = (cpu[0].user - cpu[0].luser) / tpd * 100.0f;
	float us = (cpu[0].system - cpu[0].lsystem) / tpd * 100.0f;
	attroff(A_BOLD);
	attron(COLOR_PAIR(COLORPAIR_WHITE_BLACK));
	printw("cpu\t: ");
	attron(A_BOLD);
	printw("%*.1f/%*.1f\n", 3, uu, us);
	attroff(A_BOLD);

	printw("mem\t: ");
	attron(A_BOLD);
	float usage = (float)(mem[0].value - mem[2].value) / mem[0].value * 100.0f;
	printw("%*.1f%\n", 3, usage);

	attroff(A_BOLD);
	printw("-----------------------------------------------------------\n");

	int dy = getmaxy(mainWindow) - SENSOR_LINES + sensorListIndex;
	for (int i = sensorListIndex; (i < dy) && (i < track.getcount()); i++)
	{
		attroff(A_BOLD);
		attron(COLOR_PAIR(COLORPAIR_WHITE_BLACK));
		printw("%s\t ", track[i].chip->prefix);
		move(getcury(mainWindow), 10);
		printw("%s\t:", track[i].subf->name);
		move(getcury(mainWindow), 24);
		attron(COLOR_PAIR(COLORPAIR_WHITE_BLACK) | A_BOLD);
		printw("\t%6.1f", 4, track[i].val);
		attron(COLOR_PAIR(COLORPAIR_CYAN_BLACK));
		printw("\t%6.1f", 4, track[i].low);
		attron(COLOR_PAIR(COLORPAIR_RED_BLACK));
		printw("\t%6.1f\n", 4, track[i].high);
	}

	attroff(A_BOLD);
	attron(COLOR_PAIR(COLORPAIR_WHITE_BLACK));

	printw("-----------------------------------------------------------\n"
		   "press  'q' to quit            'k' to scroll up\n"
		   "       'r' to reset           'l' to scroll down\n"
		   "     'd/D' to change delay\n");

	refresh();
}

int main(void)
{
	pthread_t workThread, pollThread;
	struct Common common = {true, false};

	FILE *conffile = fopen("/etc/sensors3.conf", "r");
	sensors_init(conffile);
	fclose(conffile);

	track.initchips();

	mainWindow = initscr();
	start_color();
	init_pair(COLORPAIR_WHITE_BLACK, COLOR_WHITE, COLOR_BLACK);
	init_pair(COLORPAIR_GREEN_BLACK, COLOR_GREEN, COLOR_BLACK);
	init_pair(COLORPAIR_RED_BLACK, COLOR_RED, COLOR_BLACK);
	init_pair(COLORPAIR_YELLOW_BLACK, COLOR_YELLOW, COLOR_BLACK);
	init_pair(COLORPAIR_CYAN_BLACK, COLOR_CYAN, COLOR_BLACK);

	gettimeofday(&start_timeval, NULL);

	pthread_create(&workThread, 0, Worker, &common);
	pthread_create(&pollThread, 0, PollKbd, &common);

	// only wait for polling to return
	pthread_join(pollThread, 0);
	//pthread_join(workThread, 0);

	endwin();

	return 0;
}
