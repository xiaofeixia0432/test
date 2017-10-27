#include<stdio.h>
#include<string.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<fcntl.h>
#include<signal.h>
#include<unistd.h>
#include "common.h"
#include "log.h"
#include "pidfile.h"

/*create running daemon pidfile*/
int create_pidfile(const char *pid_file,int pid)
{
	FILE *pidfile = NULL;
	int pidfd = 0;
	int ret = 0;
	char *p = NULL;
	int len = 0;
	char path[FILEPATH_LEN] = {'\0'};
	p = strrchr(pid_file,'/');
	len = p - pid_file + 1;
	snprintf(path,len,"%s",pid_file);
	ret = CreateDir(path);
	if(0 != ret)
	{
		log_write(ERROR,"create directory error!");
		return -1;
	}
	
	pidfd = creat(pid_file, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
  if (pidfd != -1)
  {
    pidfile = fdopen(pidfd, "w");
  }
  
  if(NULL == pidfile)
  {
  	log_write(ERROR,"create pidfile is error!");
  	return -1;
  }
  
  fprintf(pidfile,"%d\n",pid);
  fclose(pidfile);
  return 0;
  
}

/*delete pidfile*/
void delete_pidfile(const char *pid_file)
{
	unlink(pid_file);
}

int check_process_stat(const char *pid_file)
{
	FILE *pidfile = fopen(pid_file,"r");
	int pidfd = 0;
	int pid = 0;
	int ret = 0;
	/*NO FIND PIDFILE*/
	if(NULL==pidfile)
	{
		return 0;
	}
	
	ret = fscanf(pidfile,"%d",&pid);
	if(1!=ret)
	{
		log_write(ERROR,"read pidfile is error!");
		pid = 0;
	}
	
	fclose(pidfile);
	if(!pid)
	{
		return 1;
	}
	/*check process exist return 0:exist other:fail*/
	if(kill(pid,0))
	{
		delete_pidfile(pid_file);
		return 0;
	}
	
	return 1;
	
}


#if 0
int main(int argc,char *argv[]) 
{
	int ret = 0;
	if(argc !=2 )
	{
		printf("parameter counts is error!\n");
		exit(-1);
	}
	ret = CreateDir(argv[1]);
	if(0!=ret)
	{
		printf("mkdir is error!\n");
		exit(-1);
	}
	 
	printf("mdir is finish!\n");
	return 0;
}
#endif


