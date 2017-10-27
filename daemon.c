#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<unistd.h>
#include<sys/param.h>
#include "log.h"
#include "common.h"
#include "daemon.h"

int create_daemon()
{
  int  fd;
  int pid;
  int i;
  pid = fork();
  if(pid < 0)
  {
  	log_write(ERROR,"fork error!");
  	exit(-1);
  }
  else if (pid > 0)
  {
  	exit(0);
  }
  setsid();
  /*chang work directory*/
	chdir("/");
	umask(0);
  for(i=0; i< NOFILE; ++i)
  {  
    close(i);  
  }  	
	log_write(INFO,"start daemon success!");
	return 0;	
}


