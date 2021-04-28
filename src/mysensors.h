
#define MAX_MEM_ITEMS 6 // only need first six values

#define SENSOR_LINES 12 // lines excluded for sensor display

#define	COLORPAIR_WHITE_BLACK	1
#define	COLORPAIR_GREEN_BLACK	2
#define	COLORPAIR_RED_BLACK		3
#define	COLORPAIR_YELLOW_BLACK	4
#define	COLORPAIR_CYAN_BLACK	5
#define	COLORPAIR_ALERT			6

struct CPU {
	char name[8];

	long user;
	long nice;
	long system;
	long idle;
	long iowait;
	long irq;
	long softirq;
	long steal;
	long guest;
	long guest_nice;

	long luser;
	long lnice;
	long lsystem;
	long lidle;
	long liowait;
	long lirq;
	long lsoftirq;
	long lsteal;
	long lguest;
	long lguest_nice;
};

struct MEM {
	char key[32];
	long value;
};

struct Common {
	bool mRunning;
	bool mReset;
};

void *PollKBD(void*);
void *Worker(void*);

void do_read_temps(char);
void do_read_cpu(void);
void do_read_cpufreq(char);
void do_read_mem(void);
void do_print(void);
void do_print_to_log(void);
