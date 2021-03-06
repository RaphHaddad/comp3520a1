# NOTE:
this is an OOOLD uni assignment. Please forgive the code.

# STARTING THE SHELL
if you start the shell this way, you will be welcomed with a prompt that will allow you to enter commands:

->		./myshell

alternatively if you have a batchfile filled with commands you can do this:

->		./myshell batchfile


The shell supports the following internal commands:
	cd <directory> - change the current default directory to <directory>. If the <directory> argument is not present, report the current directory. If the directory does not exist an appropriate error should be reported. This command should also change the PWD environment variable.
	clr - clear the screen.
	dir <directory> - list the contents of directory <directory>
	environ - list all the environment strings
	echo <comment> - display <comment> on the display followed by a new line (multiple spaces/tabs may be reduced to a single space)
	help - display the user manual using the more filter
	pause - pause operation of the shell until 'Enter' is pressed
	quit - quit the shell


# HOW TO USE
to use the program just type a command and press enter.

# IO REDIRECTION
IO redirection is a technque built into a shell that allows raw data on a file to be used as input or output. For example executing a program such as 

->		ls < input

will instruct the ls command to read input from the file 'input' and use that data to finish execution. In this case it will list all directories in input.

Output can be done in a similar way for example

->		ls . > output

the outputed result of this command will be stored in the file output. 

One can also truncate a file like this

->		ls . >> output

Finally, inputing and outputting redirection can be used together

->		ls < input > output

# ENVIRONMENT
The environment contains variables that are required by an operating system. This includes data about users, the working directory and the system itself. These are important as they can define the state of a system at any point in time.

Some common environment variables include: TERM (the terminal being used), PWD (current location), HOME (home directory of user), SHELL (the shell being used) and LANG (  the language being used).

# BACKGROUND EXECUTION
Background execution differs from normal execution as it does not wait for a previous process to finish. When the '&' flag is used for a command for example

->		ls . &

it tells the shell that this program can be executed in the background. That is, it doesn't wait for the previous program to terminate instead it starts its own process.
