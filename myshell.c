
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_BUFFER 1024                        /* max line buffer */
#define MAX_ARGS 64                            /* max # args */
#define SEPARATORS " \t\n"                     /* token sparators */

pid_t waitpid(pid_t pid, int *stat_loc, int options);
extern char **environ;
void syserr(const char *msg); 
void forking_new_command(char **args);



void syserr(const char * msg)   /* report error code and abort */
{
   fprintf(stderr,"%s: %s", strerror(errno), msg);
   abort();
}


void forking_new_command(char *args[]) {
	pid_t pid;
	switch (pid = fork()) {
		case -1:
			syserr("fork");
		case 0:
			execvp(args[0],args);
			syserr("exec");
	}
}

int main (int argc, char ** argv)
{
    char buf[MAX_BUFFER];                      /* line buffer */
    char * args[MAX_ARGS];                     /* pointers to arg strings */
    char ** arg;                               /* working pointer thru args */
    char path_prompt[MAX_BUFFER]; 
    char *prompt;/* shell prompt */
/* keep reading input until "quit" command or eof of redirected input */

    while (!feof(stdin)) { 
/* get command line from input */
		getcwd(path_prompt,MAX_BUFFER);
		char *end_prompt = " ==> ";
		prompt = malloc( strlen(path_prompt) + strlen(end_prompt) + 1 );
		strcat(prompt,path_prompt);
		strcat(prompt,end_prompt);
        fputs (prompt, stdout);                /* write prompt*/
        /*free(prompt);*/
        if (fgets (buf, MAX_BUFFER, stdin )) { /* read a line*/
/* tokenize the input into args array */
            arg = args;
            *arg++ = strtok(buf,SEPARATORS);   /* tokenize input */
            while ((*arg++ = strtok(NULL,SEPARATORS)));
			/* last entry will be NULL */									
			if (args[0]) {                     /* if there's anything there */
				/* check for internal/external command */
			
				/*************pause************/
				if(!strcmp(args[0],"pause")){
					printf("Press Enter to continue...");
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
                    break;                     /*  break out of 'while' loop */
               /*************quit****************/


               	/*********DIR ***************/
                if (!strcmp(args[0],"dir")){/*TODO: implment dir */
						if (args[1]) {/*if something after dir*/
							char *str;
							char *ls = "ls -al ";
							str = malloc(strlen(ls) + strlen(args[1])+1);
							strcat(str,ls);
							strcat(str,args[1]);
	
							/*char * args_to_pass[] = {"ls","-al",args[1],NULL};
							forking_new_command(args_to_pass);
							continue;*/
							system(str);
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
						chdir(args[1]);
						char *copy;
						char working_dir[MAX_BUFFER];
						getcwd(working_dir,MAX_BUFFER);
						copy = strdup(working_dir);
						setenv("PWD",copy,1);

					} else {

						printf("-->PATH IS:%s<-- \n", getenv("PWD"));
					}
					continue;
				}

				/**********CD ***********/

				/*********environ*********/
				if (!strcmp(args[0],"environ")){
					char ** env = environ;
					while (*env) printf("%s\n",*env++);
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
							/*printf("%s\n",args[i]);*/
						}
						str = malloc(lengthStr);
						i = 1; /* reseting counter to actually concat to variable str */
						while (args[i]){/*the reason the new counter has to be less than i-1 is because the last arg is all args contatinated */
							strcat(str,args[i]);
							strcat(str," ");
							i = i +1;
						}
					printf("%s\n",str);
					/*free(str);*/
					continue;
				}
				/**********echo ************/

			
				/* else pass command onto OS (or in this instance, print them out) */
                arg = args;
                while (*arg) fprintf(stdout,"%s ",*arg++);
				fputs ("\n", stdout);

            }

        }

    }
    return 0; 
}


