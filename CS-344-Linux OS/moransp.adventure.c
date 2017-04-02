/* Filename: adventure.c 
 * Author: MORANSP
 * Class: CS344 OPERATING SYSTEMS
 * Description: Project 2 Adventure game part 2
 * Dependencies: buildrooms.c must be compiled and run prior to this program.
 * Summary: This file contains the bulk of the adventure game
 * 	described in the project specifications for project 2
 * Notes: You may notice some legacy mentions of strtok() lying around in the comments.
 * 	I initially attempted to use the function to break up a string. That was a
 * 	stupid idea. Don't do that. Ever. I managed find a workaround that worked...
 * 	well, around it. It was safer too. I even spared the victim!
 * MUTEXES DO NOT FUNCTION. I couldn't get the code figured out in time for the deadline.
 * 	And the lecture left me with a lot of unanswered questions. I'll have to do some
 * 	research on my own time, but between my job and this... I'm toast. 	
 */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <dirent.h>

#define NUM_THREADS	2



/* Function: getTime()
 * This was supposed to be a mutex thread, but I could not get the code working in time.
 * it still generates a call to write files and such, but I couldn't finalize the mutex or
 * thread functionality in time for the deadline. 
 */

void * getTime(){
//	pthread_mutex_trylock(lock);					//Locking the mutex
	int file_desc;
	ssize_t nwritten;
	ssize_t nread;
	char readBuffer[50];
	char payload[50];
	memset(payload, '\0', sizeof(payload));
	time_t currTime;						//var for time
	time(&currTime);						//gets time
	sprintf(payload, "\n%s\n", ctime(&currTime)); 			//makes time readable

	file_desc = open("./currentTime.txt", O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);

	if (file_desc == -1)
	{
		printf("Could not open file -- open() failed on currentTime.txt\n");
		perror("In getTime()");
		exit(1);
	}
	nwritten = write(file_desc, payload, strlen(payload) * sizeof(char));
	close(file_desc);
	

	//File written. Now open it back up and print out the results
	file_desc = open("./currentTime.txt", O_RDONLY);	
			if (file_desc == -1)			//Generic Error checking
			{
				printf("--open() failed on \"%s\"\n", payload);
				perror("In main()");
				exit(1);
			}
			memset(readBuffer, '\0', sizeof(readBuffer)); 	// Clearing the receptacle
			lseek(file_desc, 0, SEEK_SET); 		
			nread = read(file_desc, readBuffer, sizeof(readBuffer));	
			close(file_desc);				// house keeping
//	printf("\n%s\n",readBuffer);
//	pthread_mutex_unlock(lock);					//Unlocking the mutex
	return NULL;
}




int main(int argc, char *argv[])
{

//pthread_t threads[NUM_THREADS];
//int thread_args[NUM_THREADS];
//int result_code, index;
//pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
//pthread_mutex_lock(&lock);
//int pthread_create(&threads[0], NULL, getTime, (void *) &thread_args[0]);



//FIND THE MOST RECENT ROOMS DIRECTORY
char *fd = "moransp.rooms.";			//Don't wear it out.
char currentDir[100];				//Intermediate filepath holder.
char targetDir[100];				//To contain final filepath.
DIR *d;						//d is for Directory.
struct dirent *dp;				
struct stat *buffer;
buffer = malloc(sizeof(struct stat));		//initialize buffer
dp = malloc(sizeof(struct dirent));		//initialize dir object
time_t lastModified=0;				//need this for finding the most recent file.
int largest=0;					//logic helper.
char tmp[50];					//more strings
char mostRecent[50];
memset(tmp, '\0', strlen(tmp));			//This is to ward off the weird bugs.
memset(mostRecent, '\0', strlen(mostRecent));	//...
memset(targetDir, '\0', sizeof(targetDir));	//...
memset(currentDir, '\0', sizeof(currentDir));	//...
getcwd(currentDir, sizeof(currentDir));		//gets the current directory.

d = opendir(currentDir);				//Open directory
if (d != NULL) {					//If it worked,
	while (dp= readdir(d)) {			//keep reading until NULL.
		if (strstr(dp->d_name,fd) != NULL){	
		    stat(dp->d_name, buffer);		//fill the stat buffer.
		    lastModified = buffer->st_mtime;	
		    if (lastModified > largest){	//get the last modified file.
			largest = lastModified;
			sprintf(tmp,"%s",dp->d_name);	//past it to a tmp file.
			strcpy(mostRecent,tmp);		//so you can past it to the working file.
		    }
		}
	}
}
sprintf(targetDir, "%s/%s",currentDir,mostRecent);	//mash it all together like mother's potato salad.
closedir(d);						//and close the directory.


//MOST RECENT ROOMS DIRECTORY NOW IN "targetDir". PRAISE GOD.
//NOW, FIND THE STARTING FILE. FIRST, INITIALIZE THE LIST OF ROOM NAMES.
char **roomfiles = malloc(9 * sizeof(char *));
int j;
int i;
for (i = 0; i<9; i++){
	roomfiles[i] = malloc(20 * sizeof(char));
}
for (j =0; j<9; j++) {
	for (i = 0; i< 20; i++){
		roomfiles[j][i] = '\0';
	}
}
//THIS SEGMENT FETCHES THE FILENAMES FROM THE TARGET DIRECTORY.
i=0;							//counter for array position.
d = opendir(targetDir);					//here we go again.
if (d != NULL) {
	while ((dp = readdir(d)) != NULL) {
//		printf("%s\n", dp->d_name);		//Debug
		sprintf(roomfiles[i],"%s",dp->d_name);	//We'll do work with these.
		i++;
	}	
	closedir(d);					//I didn't like that directory anyway.
}

for (i=2; i<9;i++){
	strcpy(roomfiles[i-2],roomfiles[i]);		//Tidying up the array.
}
roomfiles[7] = '\0';		//Putting a null character here to be safe.
roomfiles[8] = '\0';		//And here.

/*
for (i=0; i<7; i++){
	printf("%s\n", roomfiles[i]);
}
*/

//WE NOW HAVE OUR FILES. NEXT WE FIND THE STARTING FILE. SO NOW WE HAVE TO OPEN THEM. YAY US.

int file_descriptor;					//File descriptor. For files.
int counter = 0;					//Needed for the murder loop.
int stepCount = 0;					//Do I need to say it?
char *currentLocation = malloc(100* sizeof(char));	//This string holds the current location.
char *roomName = malloc(100 * sizeof(char));		//This string holds the connection names.
char userInput[255];					//It's evil. All of it.
char victim[128];					//Murder isn't pretty. Neither is strtok().
char **reachable=malloc(6 * sizeof(char *));		//All the rooms you can go to.
char possibleConnections[128];				//UI string.
char roomType[20];					//UI string.
char *midRoom = "MID_ROOM";				//These literals are here for easy access.
char *startRoom = "START_ROOM";				// ^
char *endRoom = "END_ROOM";				// ^
char readBuffer[128];					//For reading files.
char * pch;						//Accessory to murder.
char payload[100];					//For getting files.
ssize_t nread;						//For reading files.
char path[1000];					//For tracking the user path.

memset(currentLocation, '\0', sizeof(currentLocation));		//This section initializes strings
memset(userInput, '\0', sizeof(userInput));
memset(roomName, '\0', sizeof(roomName));
memset(victim, '\0', sizeof(victim));
memset(possibleConnections, '\0', sizeof(possibleConnections));
memset(roomType, '\0', sizeof(roomType));
memset(path, '\0', sizeof(path));

for (i=0;i<6;i++)
{
	reachable[i] = malloc(6 * sizeof(char));	//Free this too.
}

//Start the loop to find the start room.
int k;
for (k=0;k<7;k++)
{
	memset(payload, '\0', sizeof(payload));		//Clean the barrel
	strcpy(payload, targetDir);			//Load the charge
	strcat(payload,"/");				//Pack it down
	strcat(payload, roomfiles[k]);			//Take aim!

	file_descriptor = open(payload, O_RDONLY);	//FIRE!
	if (file_descriptor == -1)			//Generic Error checking
	{
		printf("--open() failed on \"%s\"\n", payload);
		perror("In main()");
		exit(1);
	}
	memset(readBuffer, '\0', sizeof(readBuffer)); 	// Clearing the receptacle
	lseek(file_descriptor, 0, SEEK_SET); 		// Reset the file pointer to the beginning of the file
	nread = read(file_descriptor, readBuffer, sizeof(readBuffer));	//Get that sweet, sweet knowledge.
	close(file_descriptor);				// house keeping

	if (strstr(readBuffer, "START_ROOM")!=NULL) 	//If it's the right file
	{
		 break;					//stop.
	}
}

//Now to populate the values.

//INITIALIZATE_ROOM
pch = 0;
pch = strstr(readBuffer, "ROOM NAME: ");			//find the room name
if (pch != 0){
	pch = &pch[11];						//Advance to content
	strncpy(currentLocation, pch, strcspn(pch,"\n"));	//copy the content into the location.
	memset(victim, '\0', sizeof(victim));
	strcat(path, currentLocation);
	strcat(path, "\n");
}
i=0;
while (pch != 0){
	pch = strstr(pch, "CONNECTION");					//Get Room name
	if (pch!=0) {
		pch = &pch[14];					//advance forward two spaces in the c-string
		strncpy(victim, pch, strcspn(pch,"\n"));
		strcat(possibleConnections, victim);
		strcat(possibleConnections, ", ");
		strcpy(reachable[i], victim);
		i++;
		memset(victim, '\0', sizeof(victim));
	}
}
pch = strstr(readBuffer, "ROOM TYPE");
if (pch!=0){
	pch = &pch[11];
	strncpy(roomType, pch, strcspn(pch,"\n"));
}
/*for (i=0;i<6;i++){
	printf("%s\n",reachable[i]);
}*/
//Remove the last comma of possible connections and replace it with a period.!!!
possibleConnections[strlen(possibleConnections)-2] = '.';


int found = 0;
int run = 1;		//Conditional for Game loop.




//GAME LOOP STARTS HERE____________________________________
while (run) 		
{

if (strcmp(roomType, endRoom)==0){
	run = 0;		//terminate loop.
	printf("\nYOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
	printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n%s",stepCount, path);
}else{

printf("\nCURRENT LOCATION: %s\n", currentLocation);
printf("POSSIBLE CONNECTIONS: %s\n", possibleConnections);
printf("WHERE TO? >");
memset(userInput,'\0', sizeof(userInput));			//clear input buffer.
fgets(userInput, 25, stdin);					//get some input.
userInput[strlen(userInput)-1] = '\0';				//remove the newline character.
if (userInput != NULL)
{

	if (strcmp(userInput, "time")){ 		//TIME mutex. GOES HERE.
//		pthread_mutex_unlock(&lock);
//		getTime();
//		pthread_mutex_lock(&lock);
	//	pthread_
	//	memset(userInput,'\0', sizeof(userInput));
	//	printf("\nWHERE TO? >");
	//	fgets(userInput, 25, stdin);
	//	userInput[strlen(userInput)-1] = '\0';
	}
	for (i=0;i<6;i++)
	{
		if (strcmp(reachable[i],userInput)== 0)		//if the user string is in the array.
		{
			found = 1;				//Set the flag and break.
			break;
		}
	}
	if (found == 1){		//CASE: Found the room. Time to mosey.
		found = 0;
		stepCount++;
		strcat(path, userInput);
		strcat(path, "\n");
		//Clean the Reachable array
		
			memset(payload, '\0', sizeof(payload));		//Clean the barrel
			strcpy(payload, targetDir);			//Load the charge
			strcat(payload,"/");				//Pack it down
			strcat(payload, userInput);			//Take aim!

			file_descriptor = open(payload, O_RDONLY);	//FIRE!
			if (file_descriptor == -1)			//Generic Error checking
			{
				printf("--open() failed on \"%s\"\n", payload);
				perror("In main()");
				exit(1);
			}
			memset(readBuffer, '\0', sizeof(readBuffer)); 	// Clearing the receptacle
			lseek(file_descriptor, 0, SEEK_SET); 		
			nread = read(file_descriptor, readBuffer, sizeof(readBuffer));	
			close(file_descriptor);				// house keeping

		//CHANGE OF FILE GOES FIRST	
		for (k=0;k<6;k++){				//Reset the reachable array
			memset(reachable[k], '\0', sizeof(reachable[k]));
		}						//Reset connections and location
		memset(possibleConnections, '\0', sizeof(possibleConnections));
		memset(currentLocation,'\0', sizeof(currentLocation));
	pch = 0;
	pch = strstr(readBuffer, "ROOM NAME: ");			//find the room name
	if (pch != 0){
		pch = &pch[11];						//Advance to content
		strncpy(currentLocation, pch, strcspn(pch,"\n"));	//copy the content into the location.
		memset(victim, '\0', sizeof(victim));
	}
	i=0;
	while (pch != 0){
		pch = strstr(pch, "CONNECTION");			//Find Connection name
		if (pch!=0) {
			pch = &pch[14];					//advance forward to content
			strncpy(victim, pch, strcspn(pch,"\n"));
			strcat(possibleConnections, victim);
			strcat(possibleConnections, ", ");
			strcpy(reachable[i], victim);
			i++;
			memset(victim, '\0', sizeof(victim));
		}
	}
	pch = strstr(readBuffer, "ROOM TYPE");
	if (pch!=0){
		pch = &pch[11];
		strncpy(victim, pch, strcspn(pch,"\n"));
		strcpy(roomType, victim);
	}
//Remove the last comma of possible connections and replace it with a period.
	possibleConnections[strlen(possibleConnections)-2] = '.';
	

	}
	else{				//CASE: Not reachable. Try again.
		printf("\nHUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n");
	}
}



}
}

//pthread_mutex_destroy(&lock);
for (i=0;i<6;i++){
	free(reachable[i]);}
free(reachable);
for (i=0;i<9;i++){		//Free all the memory we allocated.
	free(roomfiles[i]);}
free(roomfiles);
free(dp);
free(currentLocation);
free(roomName);
free(buffer);
return 0;
}
