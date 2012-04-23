
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_BUFFER 1024                        /* max line buffer */
#define MAX_ARGS 64                            /* max # args */
#define SEPARATORS " \t\n"                     /* token sparators */

extern char **environ;


int main (int argc, char ** argv)
{
    char buf[MAX_BUFFER];                      /* line buffer */
    char * args[MAX_ARGS];                     /* pointers to arg strings */
    char ** arg;                               /* working pointer thru args */
    char * prompt = "==>" ;                    /* shell prompt */
/* keep reading input until "quit" command or eof of redirected input */

    while (!feof(stdin)) { 
/* get command line from input */
        fputs (prompt, stdout);                /* write prompt*/
        if (fgets (buf, MAX_BUFFER, stdin )) { /* read a line*/
/* tokenize the input into args array */
            arg = args;
            *arg++ = strtok(buf,SEPARATORS);   /* tokenize input */
            while ((*arg++ = strtok(NULL,SEPARATORS)));
			/* last entry will be NULL */									
			if (args[0]) {                     /* if there's anything there */
				/* check for internal/external command */
				if (!strcmp(args[0],"clr")) { /*  "clear" command */
                    system("clear");
                    continue;
                }
                if (!strcmp(args[0],"quit"))   /*  "quit" command */
                    break;                     /*  break out of 'while' loop */
                if (!strcmp(args[0],"dir")){/*TODO: implment dir */
                	if (args[1]) {/*if something after dir*/
						char *ls = "ls -al ";
						char *str;
						str = malloc(strlen(ls) + strlen(args[1])+1);
						strcat(str,ls);
						strcat(str,args[1]);
						system(str);
						free(str);
					} else {/* if no directory is selected assume it is current directory */
						system("ls -al .");
					}
				}
				if (!strcmp(args[0],"environ")){
					char ** env = environ;
					while (*env) printf("%s\n",*env++);
				}
				if (!strcmp(args[0],"echo")) {
						int i= 1;
						int lengthStr = 5;/* the word 'echo' is 4 characters with a space it is 5 */
						while (args[i]){
							lengthStr = strlen(args[i])+ lengthStr;
							i = i + 1;
						}
						printf("%i",lengthStr);
				}
			
			
				/* else pass command onto OS (or in this instance, print them out) */
                arg = args;
                while (*arg) fprintf(stdout,"%s ",*arg++);
				fputs ("\n", stdout);

            }

        }

    }

    return 0; 
}
