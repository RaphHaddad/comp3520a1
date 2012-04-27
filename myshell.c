
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

void forkExec(char *args[],char *shellVar,int inputType,int outputType,char * inputFileStr, char *outputFileStr) {
	int status;
	pid_t pid;
	switch (pid = fork()) {
	case -1:
		syserr("fork");
	case 0:
		if (outputType == 1) {
			freopen(outputFileStr,"w",stdout); 
		} else if (outputType ==2 ) {
			freopen(outputFileStr,"a+",stdout);
		}
		setenv("parent",shellVar,1);	
		execvp(*args,args);
		syserr("exec");
		exit(0);
	default:
		if (1==1)/*** if  not in the background **/
			waitpid(pid, &status, WUNTRACED);
	}
}


int main (int argc, char ** argv)
{
	int outputType = 0; /** trigger used to tell which output type. 0 for no redirection, > is 1, >> is 2*/
	int inputType = 0; /** trigger used to tell which input type. 0 for no redirection, < is 1 */

	/*variables for input output files */
	char *inputFileStr = NULL;
	char *outputFileStr = NULL;
	FILE *outputFile = stdout;
	FILE *inputFile = stdin;

	/***variables for reading input file ***/
	char **inputFileArg;
	char * inputFileArgs[MAX_ARGS];
	char inputFileBuf[MAX_BUFFER];

	/***variables for reading input file **/

	FILE *commands;
    char buf[MAX_BUFFER];                      /* line buffer */
    char * args[MAX_ARGS];                     /* pointers to arg strings */
    char *args_temp[MAX_ARGS];					/* used as above to remove < >> and associated files */
    char ** arg;                               /* working pointer thru args */
    char path_prompt[MAX_BUFFER]; 
    char original_path[MAX_BUFFER];				/*store in different directory incase user changes which directory he/she is on */
    char *prompt;/* shell prompt */
	char *shellVar;/** shel location **/
	const char *end_prompt;/**variabel to store ==> at end of directly for prompt **/
	int i = 0;/** counter for input output setting */
	int j = 0;
	int k = 0;
	char * str; /** temp str **/
	int lengthStr; /** used for echo **/	
	char *args_to_pass[100];/** 100 arguments is more than enough **/
	
	/* keep reading input until "quit" command or eof of redirected input */
	getcwd(original_path,MAX_BUFFER);

	/************stdin or batchfile ***********************/
	commands = stdin;
	if (argc == 2)
		printf("---->%s<-----",*(argv+1));
	

	/************stdin or batchfile ***********************/


	/******setting environment for shell variable *******/
	
	shellVar = malloc(strlen(original_path) + strlen("/myshell") + 1);
	strcat(shellVar,original_path);
	strcat(shellVar,"/myshell");
	setenv("shell",shellVar,1);
	

	/*****setting environment for shell variable *******/


    while (!feof(commands)) { 
	/*****resetting vars for io*******/
	outputType = 0; /** trigger used to tell which output type. 0 for no redirection, > is 1, >> is 2*/
	inputType = 0; /** trigger used to tell which input type. 0 for no redirection, < is 1 */
	inputFileStr = NULL;
	outputFileStr = NULL;
	outputFile = stdout;
	inputFile = commands;

	/*** resetting var for io ****/




/* get command line from input */
		sigset(SIGINT,SIG_IGN);


		/**setting the prompt str **/
		getcwd(path_prompt,MAX_BUFFER);
		end_prompt = " ==> ";
		prompt = malloc( strlen(path_prompt) + strlen(end_prompt) + 1 );
		strcat(prompt,path_prompt);
		strcat(prompt,end_prompt);
		fprintf(stdout,"%s",prompt); 
		/**setting prompt str **/

        
        /*free(prompt);*/
        if (fgets (buf, MAX_BUFFER, commands )) { /* read a line*/
		/* tokenize the input into args array */
            arg = args_temp;
            *arg++ = strtok(buf,SEPARATORS);   /* tokenize input */
            while ((*arg++ = strtok(NULL,SEPARATORS)));
			
			/****detecting input/output flags etc */
			i = 0;
			j = 0;
			k = 0;
			while (args_temp[i]) {
				if (!strcmp(args_temp[i],"<")){
					inputType = 1;
					inputFileStr = args_temp[i + 1];
					inputFile = fopen(inputFileStr,"r");/**may have to check if exist **/
					if (fgets (inputFileBuf, MAX_BUFFER,inputFile)) {/**adding everything to the normal arg list as if it was a normal feed */
						inputFileArg = inputFileArgs;
						*inputFileArg++ = strtok(inputFileBuf,SEPARATORS);
						while ((*inputFileArg++ = strtok(NULL,SEPARATORS)));
						while (inputFileArgs[k]){
							args[j] = inputFileArgs[k];
							j = j + 1;
							k = k + 1;
						}
					}
					i = i + 2;
					continue;
				}

				if (!strcmp(args_temp[i],">")){
					outputType = 1;
					outputFileStr = args_temp[i + 1];
					outputFile = fopen(outputFileStr,"w");
					i = i + 2;
					continue;
				}

				if (!strcmp(args_temp[i],">>")){
					outputType = 2;
					outputFileStr = args_temp[i + 1];
					outputFile = fopen(outputFileStr,"a+");
					i = i + 2;
					continue;
				}
				args[j] = args_temp[i];
				j = j + 1;
				i = i + 1;
			}
				args[j] = NULL;/*last entry must be NULL */
			/***************************************/


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
							args_to_pass[0] = "ls";
							args_to_pass[1] = "-al";
							args_to_pass[2] = args[1];
							forkExec(args_to_pass,shellVar,inputType,outputType,inputFileStr,outputFileStr);
						} else {/* if no directory is selected assume it is current directory */
							args_to_pass[0]= "ls";
							args_to_pass[1] = "-al";
							args_to_pass[2] = ".";
							forkExec(args_to_pass,shellVar,inputType,outputType,inputFileStr,outputFileStr);
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
							fprintf(outputFile,"No such directory\n");
						}

					} else {

						fprintf(outputFile,"No directory selected the path is: %s \n", getenv("PWD"));
					}
					continue;
				}

				/**********CD ***********/

				/*********environ*********/
				if (!strcmp(args[0],"environ")){
					char ** env = environ;
					while (*env) fprintf(outputFile,"%s\n",*env++);
					continue;
				}
				/********environ********/

				/*******echo************/
				if (!strcmp(args[0],"echo")) {
						i= 0;/*start count at zero as the command echo doesn't need to be outputed*/
						lengthStr = 1;/* starting at one because of trailing \n */
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
					fprintf(outputFile,"%s\n",str);
					continue;
				}
				/**********echo ************/

			
				/* else pass command onto OS (or in this instance, print them out) */
                arg = args;
				/*while (*arg) fprintf(stdout,"%s ",*arg++);
				fputs ("\n", stdout);*/
				forkExec(args,shellVar,inputType,outputType,inputFileStr,outputFileStr);

					
            }

        }

    }
    return 0; 
}


