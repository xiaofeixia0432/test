#define FILEPATH_LEN 512
#define FILE_LEN 256
#define CONFIG_FILE "cfg.ini"
#define CFG_SECTION_LOG "LOG"
#define CFG_KEY_LOG_PATH "logpath"
#define CFG_SECTION_PIDFILE "PIDFILE"
#define CFG_KEY_PIDFILE "pidfile"

enum {
	INFO=1,
	DEBUG=2,
	WARN=3,
	ERROR=4,
	OTHER=5
};

extern char g_log_path[FILEPATH_LEN];
extern char program_name[128];

extern void log_write(int level,char *format,...);