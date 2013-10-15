/* 
  mydash.c
 */

#include	"execute.h"
#include	"readline/readline.h"
#include	"readline/history.h"

/**
 * This is the main driver function for the mydash program.
 */

int main(void)
{
	char    *line;
	char	lineCopy[MAXLINE];
	char	*args[MAXLINE];
	struct	passwd  *pwd;
	int	i = 1;
	char    *prompt;
	int	jobCount = 1;
	prompt = getenv("DASH_PROMPT");
	ListPtr list;

	list = createList(getJobPid,toString, freeJob, compareJobs);
	using_history();

	while ((line=readline(prompt))) {
		add_history(line);
		i = 1;
		strcpy(lineCopy, line);
		if(strcmp(line,"exit") == 0) {     //exit command
			printf("Exitting...\n");
			freeList(list);
			free(line);
			sleep(1);
			exit(0);
		}
		else if(strcmp(line,"cd") == 0) {
			pwd = getpwuid(getuid());	//cd home command
			chdir(pwd->pw_dir);
		}
		else if(strcmp(line,"jobs") == 0) {	//jobs command
			updateBackgroundJobs(list);
			customPrintList(list->head, list->toString);
		}
		else if(strcmp(line,"") == 0); //do nothing for blank line
		else {
			args[0] = strtok(line, " ");		//file to exec
			while((args[i] = strtok(NULL, " ")) != NULL) {
				i++;		//assign each arg into an array
			}
			if(strcmp(args[0], "cd") == 0) {
				chdir(args[1]);	//parent must handle cd command.
			}
			else {
				if (args[i-1][(strlen(args[i-1])-1)] != '&') {
					jobCount = runJob(args, lineCopy, jobCount, list);
				}
				else {
					jobCount = runBackgroundJob(args, lineCopy, i, jobCount, list);
				}
			}
		}
	}
	exit(EXIT_SUCCESS);
}
