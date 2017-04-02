/****************************************************
 * Filename: smallsh.c
 * Author: Spencer Moran
 * Class: CS344
 * Description: Core program for the smallsh shell.
 * Notes: This program is unfinished. See comments
 * 	for details.
 ****************************************************/

#include<stdlib.h>
#include<stdio.h>
#include<sys/types.h>
#include<string.h>
#include<fcntl.h>
#include<unistd.h>
#include<signal.h>

int fgmode = 0;		//I know what you're thinking. WTF global variable? Sadly, I couldn't think of a more
			//    efficient means of getting the signals to behave.

void catchSIGINT(int signo){	//Ctrl-C signal handler. SIGINT. To kill the shell, send -KILL signal OR
}				// -SIGUSR2.

void catchSIGUSR2(int signo){	//Custom signal handler. SIGUSR2. Kill program.
	exit(0);
}

void catchSIGTSTP(int signo){	//Ctrl-Z signal handler. SIGTSTP
	char* messageOn_39 = "Caught SIGTSTP. Foreground mode is ON\n";		//Not fully implemented.
	char* messageOff_40 = "Caught SIGTSTP. Foreground mode is OFF\n";	//Catch works and flips the
	if (fgmode == 0){		//le on.				//bit, but does not affect
		write(STDOUT_FILENO, messageOn_39, 39); fflush(stdout);		//background mode yet.
		fgmode = 1;
	}else {				//le off.
		write(STDOUT_FILENO,messageOff_40, 40); fflush(stdout);
		fgmode = 0;
	}
}

void catchSIGUSR1(int signo){	//Custom signal handler. SIGUSR1
	
}


int main(int argc, char* argv[])
{
//Signal initialization. LECTURE CODE
//Create 3 structs for the handlers
struct sigaction SIGINT_action = {0}
	,SIGUSR2_action={0} 
	,SIGTSTP_action={0} 
	,SIGUSR1_action={0}
	,ignore_action = {0};

SIGINT_action.sa_handler = catchSIGINT;		//sets the handler for Ctrl-C
sigfillset(&SIGINT_action.sa_mask);
SIGINT_action.sa_flags = 0;

SIGUSR1_action.sa_handler = catchSIGUSR1;	//sets handler for SIGUSR1
sigfillset(&SIGUSR1_action.sa_mask);
SIGUSR1_action.sa_flags = 0;

SIGUSR2_action.sa_handler = catchSIGUSR2;	//Kill code.
sigfillset(&SIGUSR2_action.sa_mask);
SIGUSR2_action.sa_flags = 0;

SIGTSTP_action.sa_handler = catchSIGTSTP;	//sets the handler for Ctrl-Z
sigfillset(&SIGTSTP_action.sa_mask);
SIGTSTP_action.sa_flags = 0;

ignore_action.sa_handler = SIG_IGN;

sigaction(SIGINT, &SIGINT_action, NULL);	//if receiving SIGINT, use handler.
sigaction(SIGUSR2, &SIGUSR2_action, NULL);	//if receiving SIGUSR2, use handler. 
sigaction(SIGTSTP, &SIGTSTP_action, NULL);	//if receiving SIGTSTP, use handler.
sigaction(SIGUSR1, &SIGUSR1_action, NULL);	//if receiving SIGUSR1, use handler.
sigaction(SIGTERM, &ignore_action, NULL);	//Ignore SIGTERM
sigaction(SIGHUP, &ignore_action, NULL);	//Ignore SIGHUP
sigaction(SIGQUIT, &ignore_action, NULL);	//Ignore SIGQUIT


//Initialization block
char* openingMessage = "Welcome to smallsh by Spencer Moran.\n";
char* commandline = ":";
char userinput[2049];
pid_t shellPID = getpid();
pid_t childPID;
char current_directory[150];		//self-explanatory
char* change_dir;
char **commands;			//This is an array of strings.
int process_exit_value = 0;
int process_exit_status;
//general setup
memset(current_directory, '\0', sizeof(current_directory));
memset(userinput, '\0', sizeof(userinput));

getcwd(current_directory, 150);

commands = malloc(512 * sizeof(char *));		//FREE THIS
int i;
for (i=0;i<512;i++){					//THIS TOO
	commands[i] = malloc(2049 * sizeof(char));
	memset(commands[i], '\0', sizeof(commands[i]));
}

//write(STDOUT_FILENO, openingMessage, 39); 	fflush(stdout);
//write(STDOUT_FILENO, "Shell PID is: ", 15); 	fflush(stdout);
//printf("%d\n", shellPID);			fflush(stdout);


//Signals
/* Signal control plan
 *	This assignment specifies two behaviors from signals
 *		CTRL-C sends SIGINT to parent and all processes.
 *			DOES NOT TERMINATE SHELL.
 *			ONLY terminates the forground command if running.
 *			Background processes should NOT be affected.
 *			parent prints signal number that kills the child.
 * 		CTRL-Z sends SIGTSTP to shell.
 * 			Toggles on/off running processes in background.
 * 			displays an informative message about ^^^^^.
 * 			the toggle ignores/unignores the & sign.
 */




/* Shell Implementation plan.
 * Getting commands: Set up an array to hold all of the commands the user will enter.
 * 	For the sake of planning, we will call this array commands[].
 * 		commands[0] will hold the primary command name.
 * 		commands[1-n] will hold the arguments.
 * 	Each condition will check commands[0] for the correct argument name.
 * 		If the name matches. It will proceed to check the command arguments.
 * 		if everything is supported, fork(), and then the parent process will continue,
 * 		while the child process will work in the background.
 * 			the child process will then call exec() once setup is complete.
 * 		
 */

//Shell process run loop
int run = 1;					//execution flag
int j = 0;		
int chdir_status;			
char* pch;
char cmdbuilder[2049];
pid_t Spawn_Children[512];			//Array of child processes holds up to 511 processes.
int spawnTracker = 0; 				//increments when a child is created.
int sourceFD, targetFD, result;			//file redirection flags. *Not fully implemented*
memset(cmdbuilder, '\0', sizeof(cmdbuilder));
while (run)
{
/****************************
 * INPUT STUFF
 ***************************/
	memset(userinput, '\0', sizeof(userinput));
	write(STDOUT_FILENO, commandline, 2);		fflush(stdout);
	fgets(userinput, 2049, stdin);		//Get input from user.

	for (i=0; i<512; i++){		//RESET THE COMMANDS ARRAY
		memset(commands[i], '\0', sizeof(commands[i]));
	}
	
	pch = userinput;
	i=0;
	while (pch !=0){		//PARSE THROUGH USER STRING AND ISOLATE COMMANDS
		if (pch[0] == '#'){			//Find comments and stop! 
			strncpy(commands[i], pch, strcspn(pch,"\n"));
			break;
		}
		else if (commands[0] != NULL && strcmp(commands[0],"echo")==0){ //If an echo,
			strncpy(commands[1], pch, strcspn(pch,"\n"));		//parse the rest
			break;						//as a single string.
		}
		else if (strstr(pch, " ")!=NULL){ 	//SPACE DELIMITER CASE
			strncpy(commands[i], pch, strcspn(pch," "));
//			printf("%s\n", commands[i]); fflush(stdout);
			pch = strstr(pch," ")+1;
			i++;	
		}	
		else{					//NEWLINE TERMINATION
			strncpy(commands[i], pch, strcspn(pch, "\n"));
//			printf("%s\n", commands[i]); fflush(stdout);
			pch = strstr(pch,"\n")+1;
			i++;				//Logic: if it hits a newline, the user pressed
			break;				// Return.
		}
	}
/*	
	for (i=0; i<10; i++){			//DEBUG VALIDATION
		if (commands[i] != '\0'){
			printf("commands[%d] is: %s length:%d\n", i, commands[i], strlen(commands[i]));
			fflush(stdout);
		}
		else {break;}
	}
*/
	
	for (i=0; i < 512; i++){		//EXPAND $$ INTO shell PID
		if (strstr(commands[i], "$$")!=0){
			pch = strstr(commands[i], "$$");
			pch[strcspn(pch,"$$")] = '\0';
			sprintf(pch, "%d", shellPID);
//			printf("%s\n", commands[i]); fflush(stdout);
		}
	}

/*******************************************************************************
 * END INPUT STUFF.
 * Now test the arguments vs the command conditions.
 *     and execute commands if applicable.
 * NOTE: the for loop at the beginning of this segment really bit me in the ass.
 * 	I set it up this way to account for the case where a user might run
 * 	multiple commands on one line, but the result was a complicated mess of
 * 	code checking for specific commands, instead of a good catchall. The
 * 	required built-in commands EXIT, CD, and STATUS, all work as intended,
 * 	but other commands such as file i/o redirection, external commands with more
 * 	than one parameter, and cat statements, do not work as they should, and suffer
 * 	from serious bugs. So far the confirmed commands that do work as intended are
 * 	LS, ECHO, MKDIR, and PS, which can run with or without parameters. I am uncertain if
 * 	the file redirection portion is working correctly. It is possible to run the
 * 	command ls > junk or what have you, but the resulting file does not contain the
 * 	right information.
 *******************************************************************************/
	for (j=0;j<512;j++){
	//EXIT BUILT-IN COMMAND
		if (commands[j][0]=='#'){}		//Comment, do nothing.
		else if (strcmp(commands[j],"exit") == 0){	//USER INPUTS EXIT COMMAND
			//exit;
			//NEED AN ARRAY TO TRACK CHILD PIDs
			//call waitpid() on all child processes and then terminate
			//kill all children before exiting!!!!!!!
			run = 0;
			break;
		}
	//CD BUILT-IN COMMAND
		else if (strcmp(commands[j], "cd")==0){	//CHANGES DIRECTORY ABSOLUTE AND RELATIVE
			//cd [PATH];
				
			if (j!=511 && strcmp(commands[j+1], "\0")!=0 && commands[j+1][0] != '-'){
				strcat(current_directory, "/");
				strcat(current_directory, commands[j+1]);
				chdir_status = chdir(current_directory);
				if (chdir_status == -1) {
					printf("No Directory! CHDIR()failed line 204.\n");
					process_exit_status = 1;
				}
				printf("%s\n", current_directory);		fflush(stdout);
//				printf("argument 1 is: %s\n", commands[1]);		fflush(stdout);
				j++;		//Don't read this as a command. Skip this.
			}else{			//BUGGY, additional parameters will cause problems.
				change_dir = getenv("HOME");
				chdir_status = chdir(change_dir);
				memset(current_directory, '\0', sizeof(current_directory));
				strcpy(current_directory, change_dir);
//				printf("%s\n", change_dir); fflush(stdout);
			}
		}
	//STATUS BUILT-IN COMMAND
		else if (strcmp(commands[j], "status") == 0){ //PRINT EXIT STATUS XOR -TERM LAST FG PROCESS
			printf("exit value %d\n", process_exit_status);				fflush(stdout);
		}
		else if(strcmp(commands[j], "")== 0 || strcmp(userinput, "\n")==0) {break;} 	//Do nothing.
	//EXTERNAL PROGRAM REFERENCE	
		else { 	//Not a built-in command try an external command.
			//Fork off a child. The child does redirection. and runs exec()
			fflush(stdout);	
			Spawn_Children[spawnTracker] = fork();
			switch(Spawn_Children[spawnTracker])
			{
				case -1:
					perror("HULL BREACH!\n SPAWN FAILED ON LINE 229\n"); fflush(stdout);
					exit(1);
					break;
				case 0:
					i = 0;
					if (commands[j+1][0] == '-') {		//check for parameter
						execlp(commands[j],commands[j],commands[j+1],NULL);
						perror("CHILD: exec() failure!\n");
						exit(1);
						break;
					}else if (commands[j+1][0] == '<' || commands[j+1][0] == '>'){
						//PERFORM REDIRECTION.
						if(commands[j+1][0] == '<'){
							sourceFD = open(commands[j+2], O_RDONLY);
							if(sourceFD ==-1){ perror("source open()"); exit(1);}
							printf("sourceFD == %d\n", sourceFD);

//							targetFD = open(commands[j+2], O_WRONLY 
//											| O_CREAT 
//											| O_TRUNC, 0644);
//							if(targetFD == -1){perror("target open()"); exit(1);}
//							printf("targetFD == %d\n",targetFD);
							result = dup2(sourceFD, 0);
							if(result==-1){perror("source dup2()"); exit(2);}
//							result = dup2(targetFD, 1);
//							if (result == -1){perror("target duip2()"; exit(2);}
						}
						else {	
							targetFD = open(commands[j+2], O_WRONLY 
											| O_CREAT 
											| O_TRUNC, 0644);
							if(targetFD == -1){perror("target open()"); exit(1);}
							printf("targetFD == %d\n",targetFD);
							result = dup2(targetFD, 1);
							if (result == -1){perror("target duip2()"); exit(2);}
						}
						execlp(commands[j], commands[j], commands[j+2],NULL);
						perror("CHILD:exec failure!\n");
						exit(1);
						break;
					}else if (strcmp(commands[j], "echo")==0 || 
							strcmp(commands[j], "mkdir")==0){		
						execlp(commands[j], commands[j], commands[j+1], NULL);
//						execlp(commands[j], commands[j], "-a", NULL);
						perror("CHILD:exec failure!\n");
						exit(1);
						break;
					}
					else{					
						if (commands[j+1][0] != '\0'){
							execlp(commands[j], commands[j], commands[j+1], NULL);
							perror("CHILD:exec failure!\n");
							exit(1);
							break;
						}
						execlp(commands[j], commands[j], NULL);
//						execlp(commands[j], commands[j], "-a", NULL);
						perror("CHILD:exec failure! line 316\n");
						exit(1);
						break;
					}
				default://Part of the for loop. Parent counter to avoid bad inputs. Not 100%.
					if (commands[j+1][0] == '-') { //Skip the parameter if next.
						j++;
					}
					else if (commands[j+1][0] == '<' || commands[j+1][0] == '>'){
						j = j+2;		//skip the redirection if next.
					}
					if (strcmp(commands[0],"echo")==0 || strcmp(commands[0],"mkdir")==0){
						j++;
					}
					if (commands[j][0] == '&') { j++; }	//checking for background ind.
					waitpid(Spawn_Children[spawnTracker], &process_exit_value,WNOHANG);
					spawnTracker++;		//This way we don't overwrite the kid.
					
										
			}
			if (WIFEXITED(process_exit_value)!=0){
//				printf("The process exited normally\n");
				process_exit_status = WEXITSTATUS(process_exit_value);
			}
//			printf("exit value %d\n", process_exit_value);
		}
	}
}
//END SHELL WHILE LOOP. 
//Shell Termination
	write(STDOUT_FILENO, "Exiting Shell...\n", 18);	fflush(stdout);
	for(i=0;i<512;i++){ waitpid(Spawn_Children[i], &process_exit_value, 0);}	
	for(i=0;i<512;i++){ free(commands[i]);}
	free(commands);
	return 0;
}
