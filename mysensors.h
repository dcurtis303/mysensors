
#define MAX_SENSORS 10
#define MAX_MEM_ITEMS 6 // only need first six values
#define CHIP_NAME_MAXLENGTH 32

#define	COLORPAIR_WHITE_BLACK	1
#define	COLORPAIR_GREEN_BLACK	2
#define	COLORPAIR_RED_BLACK		3
#define	COLORPAIR_YELLOW_BLACK	4

struct Temps {
	char name[CHIP_NAME_MAXLENGTH];
	double val;
	double low;
	double high;
};

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
void *LogThread(void*);

int do_read_temps(char);
void do_read_cpu(void);
void do_read_mem(void);
void do_print(int);
