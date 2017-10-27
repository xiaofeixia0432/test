#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<sys/types.h>
#include<stdarg.h>
#include<sys/stat.h>
#include<time.h>
#include<sys/timeb.h>
#include<pthread.h>
#include<unistd.h>
#include "common.h"
#include "log.h"

static pthread_mutex_t			log_mutex = PTHREAD_MUTEX_INITIALIZER;
extern char g_log_path[FILEPATH_LEN];
extern char program_name[128];

int CreateDir(const char *sPathName)
{  
  char DirName[256] = {'\0'};  
  strcpy(DirName,sPathName);  
  int i=0;
  int len = strlen(DirName);  
  if(DirName[len-1]!='/')  
  strcat(DirName,"/");     
  len = strlen(DirName);    
  for(i=1; i<len; i++)  
  {  
	  if(DirName[i]=='/')  
	  {  
	  	
		  DirName[i] = 0;  
		  if(access(DirName,F_OK)!=0)  
		  {
		    if(mkdir(DirName,0755)==-1)  
		    {   
		      printf("mkdir error!");   
		      return -1;   
		    }  
		  }
		    
		  DirName[i]  = '/';  
	  
	  }  
  }  
    
  return   0;  
}


void log_write(int level,char *format,...)
{
	FILE *fp = NULL;
	time_t t;
	struct tm *ptm = NULL;
	struct timeb tp;
	char file[FILEPATH_LEN] = {'\0'};
	va_list ap;
	t = time(NULL);
	ptm = localtime(&t);
	ftime(&tp);
	sprintf(file,"%s/%s_log[%04d-%02d-%02d]",g_log_path,program_name,1900+ptm->tm_year,ptm->tm_mon+1,ptm->tm_mday);
	if(0!=CreateDir(g_log_path)) return;
	pthread_mutex_lock(&(log_mutex));
	if((fp = fopen(file,"a+"))== NULL) return ;
	va_start(ap,format);
	switch(level)
	{
		case INFO:
			fprintf(fp,"¡¾%02d-%02d-%02d.%03d¡¿INFO:",ptm->tm_hour,ptm->tm_min,ptm->tm_sec,tp.millitm);
			break;
		case DEBUG:
			fprintf(fp,"¡¾%02d-%02d-%02d.%03d¡¿DEBUG:",ptm->tm_hour,ptm->tm_min,ptm->tm_sec,tp.millitm);
			break;
		case WARN:
			fprintf(fp,"¡¾%02d-%02d-%02d.%03d¡¿WARN:",ptm->tm_hour,ptm->tm_min,ptm->tm_sec,tp.millitm);
			break;
		case ERROR:
			fprintf(fp,"¡¾%02d-%02d-%02d.%03d¡¿ERROR:",ptm->tm_hour,ptm->tm_min,ptm->tm_sec,tp.millitm);
			break;
		default:
			fprintf(fp,"¡¾%02d-%02d-%02d.%03d¡¿OTHER:",ptm->tm_hour,ptm->tm_min,ptm->tm_sec,tp.millitm);
			break;					
	}
	
	vfprintf(fp, format, ap);
	va_end(ap);
  if (strlen(format) > 0 && '\n' != format[strlen(format) - 1]) fprintf(fp, "\n");
  fclose(fp);
  pthread_mutex_unlock(&(log_mutex));
	return;	
	
}




