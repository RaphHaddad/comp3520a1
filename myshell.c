
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>


#define MAX_BUFFER 1024                        /* max line buffer */
#define MAX_ARGS 64                            /* max # args */
#define SEPARATORS " \t\n"                     /* token sparators */

pid_t waitpid(pid_t pid, int *stat_loc, int options);
extern char **environ;
void syserr(const char *msg); 


void syserr(const char * msg)   /* report error code and abort */
{
   fprintf(stderr,"%s: %s", strerror(errno), msg);
   abort();
}

void forkExec(char * args[],char *shellVar) {
	int status;
	pid_t pid;
	switch (pid = fork()) {
	case -1:
		syserr("fork");
	case 0:
		setenv("parent",shellVar,1);	
		execvp(args[0],args);
		syserr("exec");
		exit(0);
	default:
		if (1==1)/*** if  not in the background **/
			waitpid(pid, &status, WUNTRACED);
	}
}


int main (int argc, char ** argv)
{
    char buf[MAX_BUFFER];                      /* line buffer */
    char * args[MAX_ARGS];                     /* pointers to arg strings */
    char ** arg;                               /* working pointer thru args */
    char path_prompt[MAX_BUFFER]; 
    char original_path[MAX_BUFFER];				/*store in different directory incase user changes which directory he/she is on */
    char *prompt;/* shell prompt */
/* keep reading input until "quit" command or eof of redirected input */
	getcwd(original_path,MAX_BUFFER);
	

	/******setting environment for shell variable *******/
	
	char *shellVar;
	shellVar = malloc(strlen(original_path) + strlen("/myshell") + 1);
	strcat(shellVar,original_path);
	strcat(shellVar,"/myshell");
	setenv("shell",shellVar,1);
	

	/*****setting environment for shell variable *******/


    while (!feof(stdin)) { 
/* get command line from input */
		sigset(SIGINT,SIG_IGN);



		getcwd(path_prompt,MAX_BUFFER);
		char *end_prompt = " ==> ";
		prompt = malloc( strlen(path_prompt) + strlen(end_prompt) + 1 );
		strcat(prompt,path_prompt);
		strcat(prompt,end_prompt);
		fprintf(stdout,"%s",prompt); 
        
        
        /*free(prompt);*/
        if (fgets (buf, MAX_BUFFER, stdin )) { /* read a line*/
/* tokenize the input into args array */
            arg = args;
            *arg++ = strtok(buf,SEPARATORS);   /* tokenize input */
            while ((*arg++ = strtok(NULL,SEPARATORS)));
			/* last entry will be NULL */									
			if (args[0]) {                     /* if there's anything there */
				/* check for internal/external command */
			
				/**********help ****************/
				if (!strcmp(args[0],"help")) {
					char *command = malloc (strlen("more ") + strlen(original_path) + strlen("/readme") + 1);
					strcat(command,"more ");
					strcat(command,original_path);
					strcat(command,"/readme");
					system(command);
				}

				/**********help****************/


				/*************pause************/
				if(!strcmp(args[0],"pause")){
					getpass("Press Enter to continue...");/*function actually collects input and waits for user to press enter, exactly what we need */
					sigset(SIGINT,SIG_IGN);
					continue;
				}
				/*************pause************/
				
				/**************clr*****************/
				if (!strcmp(args[0],"clr")) { /*  "clear" command */
                    system("clear");
                    continue;
                }
                /***************clr**************/

                /***************quit***************/
                if (!strcmp(args[0],"quit"))   /*  "quit" command */
                	break;
               /*************quit****************/


               	/*********DIR ***************/
                if (!strcmp(args[0],"dir")){/*TODO: implment dir */
						if (args[1]) {/*if something after dir*/
							/*char *str;
							char *ls = "ls -al ";
							str = malloc(strlen(ls) + strlen(args[1])+1);
							strcat(str,ls);
							strcat(str,args[1]);*/
	
							char * args_to_pass[] = {"ls","-al",args[1],NULL};
							forkExec(args_to_pass,shellVar);
							
							/*system(str);*/
							/*free(str);*/
						} else {/* if no directory is selected assume it is current directory */
							system("ls -al .");
						}
						continue;
				}
				/**************DIR ***********/

				/*********CD ***************/
				if (!strcmp(args[0],"cd")) {
					if (args[1]){/*** if user has supplied directory */
						if (chdir(args[1])==0) {
							char *copy;
							char working_dir[MAX_BUFFER];
							getcwd(working_dir,MAX_BUFFER);
							copy = strdup(working_dir);
							setenv("PWD",copy,1);
						} else {
							fprintf(stdout,"No such directory\n");
						}

					} else {

						fprintf(stdout,"No directory selected the path is: %s \n", getenv("PWD"));
					}
					continue;
				}

				/**********CD ***********/

				/*********environ*********/
				if (!strcmp(args[0],"environ")){
					char ** env = environ;
					while (*env) fprintf(stdout,"%s\n",*env++);
					continue;
				}
				/********environ********/

				/*******echo************/
				if (!strcmp(args[0],"echo")) {
						int i= 0;/*start count at zero as the command echo doesn't need to be outputed*/
						char * str;
						int lengthStr = 1;/* starting at one because of trailing \n */
						while (args[i]){/*loop to get the length of string to malloc */
							lengthStr = strlen(args[i])+ lengthStr + 1;/*plus 1 for the spaces */
							i = i + 1;
						}
						str = malloc(lengthStr);
						i = 1; /* reseting counter to actually concat to variable str */
						while (args[i]){/*the reason the new counter has to be less than i-1 is because the last arg is all args contatinated */
							strcat(str,args[i]);
							strcat(str," ");
							i = i +1;
						}
					fprintf(stdout,"%s\n",str);
					continue;
				}
				/**********echo ************/

			
				/* else pass command onto OS (or in this instance, print them out) */
                arg = args;
				/*while (*arg) fprintf(stdout,"%s ",*arg++);
				fputs ("\n", stdout);*/
				{
					int status;
					pid_t pid;
					switch (pid = fork()) {
					case -1:
						syserr("fork");
					case 0:
						setenv("parent",shellVar,1);	
						execvp(args[0],args);
						syserr("exec");
						exit(0);
					default:
						if (1==1)/*** if  not in the background **/
							waitpid(pid, &status, WUNTRACED);
					}
				}

					
            }

        }

    }
    return 0; 
}


