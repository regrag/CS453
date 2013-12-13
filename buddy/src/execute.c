
#include "execute.h"

/**
 * runJob will run a program based on the passed in arguments.
 * @param args contains the program name and arguments respectively.
 * @param lineCopy contains the original line read from the prompt, used to add
 * the command to the job list.
 * @param jobCount is the count of jobs.
 * @param list is a pointer to the jobs list.
 */
int runJob(char **args, char *lineCopy, int jobCount, ListPtr list) {
	//run process normally
	pid_t pid;
	int status;
	NodePtr node;
	JobPtr job;
	if ( (pid = fork()) < 0)
		err_sys("fork error");	//did not fork correctly.

	if (pid == 0) {		/* child */
		execvp(*args, args);
		err_ret("couldn't execute: %s", lineCopy);
		exit(1);
	}
	
	/* parent */
	addAtRear(list, createNode(createJob(pid, lineCopy, jobCount, 0)));
	jobCount++;
	if ( (pid = waitpid(pid, &status, 0)) < 0)
		err_sys("waitpid error");
	else if(pid > 0) {
		node = search(list, pid);
		job = (JobPtr)node->obj;
		job->status = status;
		job->reportted = 0;
	}
	updateBackgroundJobs(list);
	customPrintList(list->head,list->toString);
	return jobCount;
}

/**
 * runJob will run a program in the background based on the passed in arguments.
 * @param args contains the program name and arguments respectively.
 * @param lineCopy contains the original line read from the prompt, used to add
 * the command to the job list.
 * @param i is the counter for position in args.
 * @param jobCount is the count of jobs.
 * @param list is a pointer to the jobs list.
 */
int runBackgroundJob(char **args, char *lineCopy, int i, int jobCount, ListPtr list) {
	//run process in background
	pid_t pid;
	NodePtr node;
	if(strcmp(args[i-1],"&") == 0) args[i-1] = 0;
	else args[i-1][(strlen(args[i-1])-1)] = 0;

	if ( (pid = fork()) < 0)
		err_sys("fork error");	//did not fork correctly.

	if (pid == 0) {		/* child */
		execvp(*args, args);
		err_ret("couldn't execute: %s", lineCopy);
		exit(1);
	}

	/* parent */
	node = createNode(createJob(pid, lineCopy, jobCount, 1));
	addAtRear(list, node);
	jobCount++;
	updateBackgroundJobs(list);
	printBackgroundJob(node, list->toString);
	return jobCount;
}

/**
 * updateBackgroundJobs will update the report status of all jobs in the job list.
 * @param list is a pointer to the jobs list.
 */
void updateBackgroundJobs(ListPtr list) {
	pid_t pid;
	int status;
	NodePtr node;
	JobPtr job;
	while((pid = waitpid(-1, &status, WNOHANG)) != 0) {
		node = search(list,pid);
		if(node == NULL || pid < 0) break;
		job = (JobPtr)node->obj;
		job->status = status;
		job->reportted = 1;
	}
}

/**
 * customPrintList prints the list to the specified format required by the assingment doc.
 * @param node is a pointer to the node you wish to begin printing at. (generally head)
 * @param toString is a function pointer to the toString method of the data inside the nodes.
 */ 
void customPrintList(NodePtr node, char * (*toString)(void *)) {
	char *output;
	JobPtr job;
	while (node) {
		job = (JobPtr)node->obj;
		if(job->reportted == 1) {
			output = (*toString)(node->obj);
			printf(" %s\n",output);
			free(output);
			job->reportted = 0;
		}
		node = node->next;
	}
}

/**
 * printJobs prints the list to the specified format required by the assingment doc.
 * @param node is a pointer to the node you wish to begin printing at. (generally head)
 * @param toString is a function pointer to the toString method of the data inside the nodes.
 */ 
void printJobs(NodePtr node, char * (*toString)(void *)) {
	char *output;
	JobPtr job;
	while (node) {
		job = (JobPtr)node->obj;
		if(job->jobsReportted == 1 && job->background == 1) {
			output = (*toString)(node->obj);
			printf(" %s\n",output);
			free(output);
			job->reportted = 0;
			job->jobsReportted = 0;
		}
		node = node->next;
	}
}

/**
 * printBackgroundJob prints the list to the specified format required by the assingment doc.
 * @param node is a pointer to the node you wish to begin printing at. (generally head)
 * @param toString is a function pointer to the toString method of the data inside the nodes.
 */ 
void printBackgroundJob(NodePtr node, char * (*toString)(void *)) {
	char *output;
	output = (*toString)(node->obj);
	printf(" %s\n",output);
	free(output);
}
