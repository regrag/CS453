
#include "ourhdr.h"
#include "List.h"
#include "Job.h"

int runJob(char **args, char *lineCopy, int jobCount, ListPtr list);
int runBackgroundJob(char **args, char *lineCopy, int i, int jobCount, ListPtr list);
void updateBackgroundJobs(ListPtr list);
void customPrintList(NodePtr node, char * (*toString)(void *));
void printJobs(NodePtr node, char * (*toString)(void *));
void printBackgroundJob(NodePtr node, char * (*toString)(void *));
