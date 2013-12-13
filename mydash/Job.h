

#ifndef __JOB_H
#define __JOB_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "common.h"

#define MAXPID_DIGITS 20


typedef struct job  Job;
typedef struct job * JobPtr;

struct job {
	pid_t key;
	int jobNumber;
	char *command;
	unsigned int status;
	int reportted;		//0, been reported; 1, needs reporting.
	int jobsReportted;	//0, been reported by jobs command; 1, not.
	int background;		//0, is not a background job; 1, is.
};

JobPtr createJob (pid_t, char *, int, int);

char *toString(void *);
void freeJob(void *);
unsigned long int getJobPid(void *);
char *getCommand(void *);
unsigned int getExitStatus(void *);
char *getState(void *);
int compareJobs(void *, void *);


#endif /* __JOB_H */
