﻿#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <pthread.h>
#include <wait.h>
#include <errno.h>
#include <syslog.h>
#include <sys/mman.h>
#include <sys/reboot.h>
#include <bits/types.h>

#include "cjdeal.h"
#include "read485.h"
#include "readplc.h"
#include "guictrl.h"
#include "acs.h"
#include "event.h"

/*********************************************************
 *程序入口函数-----------------------------------------------------------------------------------------------------------
 *程序退出前处理，杀死其他所有进程 清楚共享内存
 **********************************************************/
void QuitProcess(ProjectInfo *proinfo)
{
	proinfo->ProjectID=0;
    fprintf(stderr,"\n退出：%s %d",proinfo->ProjectName,proinfo->ProjectID);
	exit(0);
}
/*********************************************************
 * 进程初始化
 *********************************************************/
int InitPro(ProgramInfo** prginfo, int argc, char *argv[])
{
	if (argc >= 2)
	{
		*prginfo = OpenShMem("ProgramInfo",sizeof(ProgramInfo),NULL);
		ProIndex = atoi(argv[1]);
		fprintf(stderr,"\n%s start",(*prginfo)->Projects[ProIndex].ProjectName);
		(*prginfo)->Projects[ProIndex].ProjectID=getpid();//保存当前进程的进程号
		return 1;
	}
	return 0;
}


/********************************************************
 * 载入档案、参数
 ********************************************************/
int InitPara()
{
	InitACSPara();
	Event_Init();
	return 0;
}
/*********************************************************
 * 主进程
 *********************************************************/
int main(int argc, char *argv[])
{
//    struct sigaction sa = {};
//    Setsig(&sa, QuitProcess);

	fprintf(stderr,"\n[cjdeal]:cjdeal run!");
	if(InitPro(&JProgramInfo,argc,argv)==0){
		fprintf(stderr,"进程 %s 参数错误",argv[0]);
		return EXIT_FAILURE;
	}

	//载入档案、参数
	InitPara();
	//485、四表合一
	//read485_proccess();
	//载波
	//readplc_proccess();
	//液晶、控制
	//guictrl_proccess();

	while(1)
   	{
	    struct timeval start={}, end={};
	    long  interval=0;
		gettimeofday(&start, NULL);
		//交采、状态、统计处理
		DealACS();
		//DealState();  //TODO：时间要求可能不满足
		gettimeofday(&end, NULL);
		interval = 1000000*(end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec);
	    if(interval>=1000000)
	    	fprintf(stderr,"deal main interval = %f(ms)\n", interval/1000.0);
		usleep(10 * 1000);
   	}
	return EXIT_SUCCESS;//退出
}