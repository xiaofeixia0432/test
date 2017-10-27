#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<fcntl.h>
#include<string.h>
#include "common.h"
#include "list.h"
#include "conf_reader.h"
#include "pidfile.h"
#include "log.h"
#include "daemon.h"

SCONIFG(g_config);
char g_log_path[FILEPATH_LEN];
char program_name[128];
char g_pid_file[FILE_LEN];
int get_config_content()
{
	int ret = 0;
	char tmp[1024] = {'\0'};
	ret = load_config_file(CONFIG_FILE,&g_config);
	if(ret!=0)
	{
		printf("load config file is error!\n");
		return -1;
	}
	
	sprintf(g_log_path,"%s",get_str_value(&g_config,CFG_SECTION_LOG,CFG_KEY_LOG_PATH,NULL));
	sprintf(g_pid_file,"%s",get_str_value(&g_config,CFG_SECTION_PIDFILE,CFG_KEY_PIDFILE,NULL));
	return 0;
}

int main(int argc,char *argv[])
{
	int ret = 0;
	int cpunum = 0;
	int i = 0;
	alarm(1);
	for(i=0;;i++)
	{
		printf("I:%d\n",i);
	}
	sprintf(program_name,"%s",argv[0]);
	if(0!=get_config_content())
	{
		exit(-1);
	}
	/*create daemon process*/
	if(0!=create_daemon())
	{
		log_write(ERROR,"start daemon error!");
		exit(-1);
	}
	/*check process state*/
	if(check_process_stat(g_pid_file))
	{
		log_write(ERROR,"%s already start!",argv[0]);
	}
	
	if(0!=create_pidfile(g_pid_file,getpid()))
	{
		log_write(ERROR,"create pidfile error!");
		exit(-1);
	}
	cpunum = sysconf(_SC_NPROCESSORS_CONF);
	log_write(INFO,"_SC_NPROCESSORS_CONF:%d",cpunum);
	sleep(10);
	unlink(g_pid_file);
	return 0;
	
}