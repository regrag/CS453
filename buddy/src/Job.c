#include "Job.h"
#include "ourhdr.h"


JobPtr createJob(pid_t key, char *command, int jobNumber, int back)
{
	JobPtr newJob = (JobPtr) malloc (sizeof(Job));
	newJob->key = key;
	newJob->jobNumber = jobNumber;
	newJob->status = -1;
	newJob->reportted = 0;
	newJob->jobsReportted = 1;
	newJob->background = back;
	newJob->command = (char *) malloc(sizeof(char)*(strlen(command)+1));
	strcpy(newJob->command, command); 
	return newJob;
}

unsigned long int getJobPid(void *job)
{
	JobPtr myjob = (JobPtr) job;
	return (unsigned long int) myjob->key;
}

char *getCommand(void *job) {
	JobPtr myjob = (JobPtr) job;
	return myjob->command;

}

unsigned int getStatus(void *job) {
	JobPtr myjob = (JobPtr) job;
	return myjob->status;

}

char *toString(void *job)
{
	JobPtr myjob = (JobPtr) job;
	char *temp;
	temp = (char *)malloc(sizeof(char)*(strlen(myjob->command)+MAXLINE));
	sprintf(temp, "[%d] Job PID:[%ld], orignal cmd: %s, status: %d",myjob->jobNumber, (unsigned long int)myjob->key, myjob->command, myjob->status);
	return temp;
}

void freeJob(void *job)
{
	JobPtr myjob = (JobPtr) job;
	if (myjob == NULL) return;
	free(myjob->command);
	free(myjob);
}

int compareJobs(void *a, void *b) {
	printf("In compareJobs method.\n");
	if (((JobPtr)a)->key == ((JobPtr)b)->key) return 1;
	else return 0;
}

