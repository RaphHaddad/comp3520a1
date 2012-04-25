/***************************************************************************************
	Author: Chris Miceli
	sid: 309 198 097
	unikey: cmic8146


Please note: the use of a character in some of the returns/arguments is the fact that they
are 1 byte, where int is 4
***************************************************************************************/
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<stdio.h>
#define MAX_BUFFER 1024
extern char **environ;

//predefine the methods to be used
int processLine (char **command);
int executeLine(char **command, char background, int input, int output, char *inputFile, char *outputfile);
int checkInput(char **command,char **infile);
int checkOutput(char **command, char **outfile);
int internalCommand(char **command);
char checkBground(char **command);
void changeDir(char ** directory);
void printEnviron();

//global variables so that we can always access what we need
char *clCmd[20];
char *readmeFile;
char *shellLoc;
char *tempStr;


int main(int argc, char **args){

	//variables and allocations for the setting of the environment and locating back to the shell directory
	char homeDir[MAX_BUFFER];
	char cwd[MAX_BUFFER];
	char shellPlace[MAX_BUFFER];
	getcwd(homeDir,MAX_BUFFER-8);
	getcwd(shellPlace,MAX_BUFFER-9);
	readmeFile = strcat(homeDir,"/readme");
	shellLoc = strcat(shellPlace,"/myshell");
	tempStr = (char *)malloc(sizeof(char)*MAX_BUFFER);
	strcpy(tempStr,"SHELL=");
	strcat(tempStr,shellLoc);
	putenv(tempStr);

	//begin variables for input
	char *tempStr2;
	char *splitStr;
	int counter;
	//end input stream variables

	//initialise command array which can handle 20 commands with a total length of 1024 char!!
	for(counter = 0;counter < 20; counter++){
		clCmd[counter] = NULL;
	}

	//check for arguments passed to the myshell application
	if (argc > 1){
		if (argc == 2){	//assumes that if the correct number of arguments, user supplied a batch
			counter = 0;
			//open the file
			FILE *input;
			input = fopen(args[1], "r");
			//error checking for the file
			if (!input){
				printf("error in file");
				return -1;
			}
			//Same reading for a file as from the command line
			tempStr2 = malloc(sizeof(char)*MAX_BUFFER);
			while (fgets(tempStr2,MAX_BUFFER,input) != NULL){
				counter = 0;
				splitStr = strtok(tempStr2," \t\n"); //tokenise the input
	                        
				//continually reads in tokens until null token
				while (splitStr != NULL){
	                                clCmd[counter] = splitStr;
	                                counter++;
	                                splitStr = strtok(NULL," \n");  
	                        }

				//create an endpoint for the data
	                        clCmd[counter] = (char*) 0; 
	
				//check something has been entered
	                        if (clCmd[0] == NULL){
	                                continue;
	                        }
				
				processLine(clCmd); //run the parsed command
				
			}
			//clean up afterwards
			free(clCmd[0]);
		} else {
			// this is achieved if incorrect number of args parsed
			printf("error in input");
			return 0;
		}
	} else {
		while(1){
			//get the current working directory and print at the shell
			counter = 0;
			tempStr2 = malloc(sizeof(char)*MAX_BUFFER);
			getcwd(cwd,MAX_BUFFER-1);
			printf("%s -->", cwd);

			//get a line of input from the user
			fgets(tempStr2,MAX_BUFFER,stdin);

			//tokenise input and create an array of pointers
			splitStr = strtok(tempStr2," \t\n");
			while (splitStr != NULL){
				clCmd[counter] = splitStr;
				counter++;
				splitStr = strtok(NULL," \n");	
			}

			//terminating pointer
			clCmd[counter] = (char*) 0;

			//check the existence of command otherwise prints line again
			if (clCmd[0] == NULL){
				continue;
			}
			processLine(clCmd);
			
			//clean up!!
			free(clCmd[0]);
		}
	}	
	
	return 0;
}

int processLine (char **command){
	int input, output; //numerical value > 0 if found.
	char background; //y if ampersand found, n if not
	char *inputFile, *outputFile; //location of the file which is to be used as stdin or stdout
	
	//initialise
	inputFile = NULL;
	outputFile = NULL;

	//if the command is internally processed, stop the creation of a child process
	if (internalCommand(command) == 1){
		return 0;	
	}

	//check options of redirection and background processing
	input = checkInput(command,&inputFile);
	output = checkOutput(command,&outputFile);
	background = checkBground(command);

	//execute the command
	executeLine(command,background,input,output,inputFile,outputFile);
	return 0;
}

int executeLine(char **command, char background, int input, int output, char *inputFile, char *outputFile){
	
	//forking variables
	pid_t childPid;
	int status;
	
	//creation of the string for seting a parent variable for child processes
	char *temp = (char *)malloc(sizeof(char)*MAX_BUFFER);
	strcat(temp,"parent=");
	strcat(temp,shellLoc);
	
	//begin fork
	childPid = fork();
	if (childPid == 0){
		if (input > 0){ // this value will be 1, therefore setting input from file
				freopen(inputFile,"r",stdin);
		}
		if (output > 0){
			if (output == 2){ //appended files return 2 to the checkOutput method
				freopen(outputFile,"a+",stdout);
			}else { // assume other value is a 1 indicating to write a file
				freopen(outputFile,"w+",stdout);
			}
		}
		//set environment
		putenv(temp);
		//execute the command now that redirection has been set/not set
		execvp(command[0],command);
		//end the process
		exit(0);
	}		

	//if the execution is to be in the foreground then we will not wait and go back to the shell
	if (background == 'n'){
		waitpid(childPid, &status,WUNTRACED);
	}
	// free the string holding the environment variable for the child process
	free(temp);
	return 0;
}

int checkInput(char **command, char **infile){
	int i,j;
	//if we find input coming in, we adjust the array and return the file and whether there was input
	//we adjust the array here and in checkOutput so that the values are not parsed to the execvp function
	//later on
	for (i = 0; command[i] != NULL; i++){
		if (command[i][0] == '<'){
			if(command[i+1] != NULL){
				*infile = command[i+1];
			}else{
				return -1;
			}
			for(j=i;command[j-1] != NULL; j++){
				command[j] = command[j+2];
			}	
			return 1;
		}
	}
	return 0;
}

int checkOutput(char **command, char **outfile){
	int i,j,k;
	//similar to checkInput but with a simple change. the k variable is set according to whether
	//append(2) or writing an output(1). if nothing found return 0. -1 on error, but is not
	//handled, simply ignored.
	for (i = 0; command[i] != NULL; i++){
		if (command[i][0] == '>'){
			k = 1;
			if (command[i][1] == '>'){
				k = 2;
			}	
			if (command[i+1] != NULL){
				*outfile = command[i+1];
			}else{
				return -1;
			}
			for (j = i; command[j-1] != NULL;j++){
				command[j] = command[j+2];
			}
			return k;
		}
	}
	return 0;
}

int internalCommand(char **command){
	if(strcmp(command[0], "quit") == 0){

		//free up some of the mallocs before closing
		free(command[0]);
		free(tempStr);
		//exit the shell
		exit(0);
	}else if(strcmp(command[0], "environ") == 0){
		printEnviron();
		return 1;
	}else if(strcmp(command[0], "clr") == 0){
		//set the correct alias
		strcpy(command[0], "/usr/bin/clear");
		return 0;
	}else if(strcmp(command[0],"dir") == 0){
		strcpy(command[0],"/bin/ls");
		return 0;
	}else if(strcmp(command[0],"echo") == 0){
		//echo is a bash command, so has been set to do nothing different in our
		//shell.
		return 0;
	}else if(strcmp(command[0],"help") == 0){
		//need to set the more alias and then create a pointer to the file to open
		strcpy(command[0],"/bin/more");
		command[1] = (char*)malloc(sizeof(char)*MAX_BUFFER);
		strcpy(command[1],readmeFile);
		return 0;
	}else if(strcmp(command[0],"cd") == 0){
		//check for null input
		if (command[1] == NULL){
			printf("THERE IS NO DIRECTORY TO CHANGE TO!!!");
			return 1;
		}
		changeDir(&command[1]);	
		return 1;
	}else if(strcmp(command[0],"pause") == 0){
		//cheaters way of forcing the pressing of enter. could create low level 
		//remaping of the keyboard to /dev/NULL but getpass() is much lower risk of
		//doing it wrong and being unable to get keyboard functionality upon
		//testing.
		getpass("Press Enter to continue....");
		return 1;
	}
	return 0;
}

char checkBground(char **command){
	int i;
	//iterates to find the & and if found removes the pointer to it so that it is not 
	//executed by execvp above
	for(i = 0; command[i] != NULL;i++){
		if (command[i][0] == '&'){
			command[i] = NULL;
			return 'y';
		}
	}
	return 'n';
}

void changeDir(char ** directory){
		//only changes directory if it exists else does nothing, returns nothing to user
		chdir(*directory);

		//set environment variables
		char * temp = (char*)malloc(sizeof(char)*MAX_BUFFER);
		strcpy(temp,"PWD=");
		char loc[MAX_BUFFER];
		getcwd(loc,MAX_BUFFER-5);
		strcat(temp,loc);
		putenv(temp);

		//clean up!!!
		free(temp);
}

void printEnviron(){
	int i;
	//simply cycle the imported array and print to console
	for(i=1;environ[i] != NULL; i++){
		printf("%s\n",environ[i-1]);
	}
}


