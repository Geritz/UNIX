/* Filename: buildrooms.c
 * Author: MORANSP
 * Class: CS344 OPERATING SYSTEMS
 * Description: Project 2 Adventure game part 1
 * Summary: this file generates a series of seven room files
 * 	named after seven of ten defined colors with each
 * 	filename being unique. Arrays are used to assist with
 * 	organizing the data before creating the files.
 *Known Bugs: Number of connections is not always at least 3. Reason unknown.
 *	moving on due to time constraints.
 */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int main () {

const char *colors[10];
colors[0] = "black";
colors[1] = "white";
colors[2] = "red";
colors[3] = "yellow";
colors[4] = "blue";
colors[5] = "green";
colors[6] = "cyan";
colors[7] = "magenta";
colors[8] = "pink";
colors[9] = "purple";

const char *roomtype[3];
roomtype[0] = "START_ROOM";
roomtype[1] = "MID_ROOM";
roomtype[2] = "END_ROOM";

const char *roomname = "ROOM NAME: ";
const char *connection = "CONNECTION ";
int **rooms = malloc (7 * sizeof(int *)); 	//free this
int *used = malloc(7 * sizeof(int));		//and this
int *typeTracker = malloc(7 * sizeof(int));	//also this
int i = 0;
int j = 0;
int k = 0;
int l = 0;
for (i = 0; i < 7; i++)
{
	rooms[i] = malloc(6 * sizeof(int));	//and all of this
}
for (j = 0; j < 7; j++)
{
	for (i = 0; i < 6; i++)
	{
		rooms[j][i] = -7;		//integer must be non-zero for detection
	}
}

time_t t;
srand((unsigned) time(&t));

int nconns = 0;		//tracks connections
int uflag = 1;		//unique flag for while loop
int randVal = 0;	//holds output from rand()
int dupflag = 0;	//duplicate flag
int hasempty = 1;	//flag for array capacity, needed for while loop.
int count = 0;		//basic counter
for (j = 0; j < 7; j++)
{
	nconns = (rand() % 7);			//grab a random number between 0 and 6
	if (nconns < 3)
		nconns = 3;			//if too low, set it to 3
	for (i = 0; i < nconns; i++)
	{
		uflag =1;			//prep for the while loop.
		hasempty = 1;
		while (uflag == 1 && hasempty != 0)
		{
			dupflag = 0;
			randVal = rand() % 7;	//roll a number between 0 and 6.
			for(k = 0; k < 6; k++)
			{
				if (randVal == j)
				{
					dupflag = 1; 		//No self references
				}
				if (randVal == rooms[j][k] )
				{ 
					dupflag = 1; 		//No duplicates allowed.
				}	
				if (rooms[j][k] == -7)
				{
					hasempty = 1;		//free space found.
				}	
				else
				{ hasempty = 0; }
			}
			if (dupflag == 0)	//If no circular reference and no duplicate.
			{ 	
				for(k = 0; k < 6; k++)		//cycle through the entire array
				{
					if (rooms[j][k] == -7)	//If there's an empty spot 
					{
						uflag = 0;	//set the flag to terminate loop
						rooms[j][k] = randVal;	//fill the position
						for (l=0;l<6;l++)	//loop through the referenced array
						{			//  to check for duplicates.
							if (rooms[randVal][l] == j)
							{
								dupflag = 1;
								break; 	//already connected
							}
						}
						for (l=0;l<6;l++)	//loop through the reference array 
						{			// and if not already connected...
							if (rooms[randVal][l] == -7 && dupflag == 0)
							{
								rooms[randVal][l] = j; 	//add the connection
								dupflag = 1;		//flag for safety.
								break;			//stop adding.
							}
						}
						break;		//all done. Break out of loop.
					}				
					else 	//The position was full, try another.
					{ 
					uflag = 0; 
					}
			}

		}
	if (hasempty == 1) {uflag = 0; break;}
	}
}
}
/*
//PRINT THE CONNECTIONS TO THE SCREEN ---DEBUG
	printf("Current connections:\n");
	for (j = 0; j < 7; j++)
	{
		for (i = 0; i < 6; i++)
		{
			if (rooms[j][i] == -7)
				break;
			printf("%d ", rooms[j][i]);
		}
		printf("\n");
	}
*/
//TYPE ASSIGNMENT BLOCK
/* This block was made following the logic that since the room
 *    connection structure is already randomized, any assignment
 *    of a start or end will appear random to the user. So really,
 *    just how random does it have to be? Assigning the start and end
 *    to the first and second parts of this array have no real effect
 *    on where they end up in the randomized structure. Other than making
 *    my life a little easier, anyway.
 */
for (i=0;i<7;i++)
{
    typeTracker[i] = -1;	//initialize the tracking array.
}

typeTracker[0] = 0; 		//assigns start
typeTracker[1] = 2;		//assigns end
for (i=2;i<7;i++)
{ typeTracker[i] = 1; }		//assigns mids
/*
printf("room types: %d %d %d %d %d %d %d", 
			typeTracker[0], 
			typeTracker[1],
			typeTracker[2],
			typeTracker[3],
			typeTracker[4],
			typeTracker[5],
			typeTracker[6]);
*/

//COLOR ASSIGNMENT BLOCK
int colorpicker = 0;
int restart = 1;
for (i=0;i<7;i++)
{
    used[i] = -1;	//initialize the tracking array.
}

while (restart == 1)
{
    colorpicker = rand() % 10;
    for (i=0;i<7;i++)
    {
    	if (used[i] == colorpicker) 	//value already in array
    	{
		break;			//stop and start over.
    	}
	else if (used[i] == -1)		//if there is an empty slot.
	{
		used[i] = colorpicker;	//fill the empty slot.
		break;
	}
    }
    if (used[6] != -1) 			//the array is full.
    {
	restart = 0;			//kill the while loop.	
    }
}
/*
//DISPLAY COLORS --DEBUG
printf("\n");
for (i=0;i<7;i++)
{
	printf("%d ", used[i]);	
}
printf("\n");
*/
/* At this point, the used array contains the color mappings for the filenames
 *     and the rooms array contains all of the connections between rooms.
 * The value in used[0] represents the filename/color for rooms[0][x], where
 *     the index of the used array corresponds to the row containing the
 *     array of values for the first room.
 * The value of used[x] also corresponds to the index of the colors array which
 *     is what the filename of the room will be.
 * So, if used[0] == 7, then the filename of rooms[0][x] will be colors[7].
 */

//FILE STUFF__________________
//Make Directory moransp.rooms.<pid>
int pid = getpid();
int file_directory;
char pidBuffer[32];
memset(pidBuffer, '\0', sizeof(pidBuffer));
//printf("pid is:%d\n", pid);
sprintf(pidBuffer,"./moransp.rooms.%d", pid);
//printf("%s\n",pidBuffer);
file_directory = mkdir(pidBuffer, S_IRWXU | S_IRWXG | S_IRWXO);  //make a directory with 770 permissions

if (file_directory == -1)
{
	printf("Directory Creation failure - mkdir() failed on \"%s\"\n", pidBuffer);
	perror("In main()");
	exit(1);
}
//Directory Created. Build room files.
int file_descriptor;			//filepath for output files. 
ssize_t nwritten;

char room1[50], room2[50], room3[50], room4[50], room5[50], room6[50], room7[50];
memset(room1, '\0', sizeof(room1));	//initialization for arrays.
memset(room2, '\0', sizeof(room2));
memset(room3, '\0', sizeof(room3));
memset(room4, '\0', sizeof(room4));
memset(room5, '\0', sizeof(room5));
memset(room6, '\0', sizeof(room6));
memset(room7, '\0', sizeof(room7));

sprintf(room1, "%s/%s", pidBuffer,colors[used[0]]);	//Essentially: filepath/filename
sprintf(room2, "%s/%s", pidBuffer,colors[used[1]]);	//posting to strings.
sprintf(room3, "%s/%s", pidBuffer,colors[used[2]]);
sprintf(room4, "%s/%s", pidBuffer,colors[used[3]]);
sprintf(room5, "%s/%s", pidBuffer,colors[used[4]]);
sprintf(room6, "%s/%s", pidBuffer,colors[used[5]]);
sprintf(room7, "%s/%s", pidBuffer,colors[used[6]]);

//The following seven blocks are pretty much identical. The only thing that
//  changes are the room numbers.
//
//  THERE IS A BUG IN THE CODE INSIDE THIS SECTION
//  REMEMBER: CONNECTIONS IN THE 'used' ARRAY ARE SYMBOLIC 
//
char payload[500];						//contains final filepath
char tmp[50];
//__ROOM 1
memset(payload, '\0', sizeof(payload));				//prep the memory
memset(tmp, '\0', sizeof(tmp));					//
sprintf(payload, "%s%s\n",roomname,colors[used[0]]);		//post to memory
for (i=0;i<6;i++)
{
	if (rooms[0][i] != -7)
	{
		sprintf(tmp,"%s%d: %s\n",connection, (i+1), colors[used[rooms[0][i]]]);
		strcat(payload,tmp);
	}
}
sprintf(tmp,"ROOM TYPE: %s\n", roomtype[typeTracker[0]]);
strcat(payload,tmp);
//printf("%s",payload);
//PAYLOAD COMPLETE. WRITE TO FILE.

file_descriptor = open(room1, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR); 	//opens the file

if (file_descriptor == -1)							//error handler
{
	printf("Could not open file -- open() failed on \"%s\"\n", room1);
	perror("In main()");
	exit(1);
}

nwritten = write(file_descriptor, payload, strlen(payload) * sizeof(char));	//write to file.
//__END ROOM 1	//And that's it. the rest of these just repeat for seven iterations with slight differences.
//__ROOM 2
memset(payload, '\0', sizeof(payload));
memset(tmp, '\0', sizeof(tmp));
sprintf(payload, "%s%s\n",roomname,colors[used[1]]);
for (i=0;i<6;i++)
{
	if (rooms[1][i] != -7)
	{
		sprintf(tmp,"%s%d: %s\n",connection, (i+1), colors[used[rooms[1][i]]]);
		strcat(payload,tmp);
	}
}
sprintf(tmp,"ROOM TYPE: %s\n", roomtype[typeTracker[1]]);
strcat(payload,tmp);
//printf("%s",payload);
//PAYLOAD COMPLETE. WRITE TO FILE.

file_descriptor = open(room2, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);

if (file_descriptor == -1)
{
	printf("Could not open file -- open() failed on \"%s\"\n", room1);
	perror("In main()");
	exit(1);
}

nwritten = write(file_descriptor, payload, strlen(payload) * sizeof(char));
//__END ROOM 2
//__ROOM 3
memset(payload, '\0', sizeof(payload));
memset(tmp, '\0', sizeof(tmp));
sprintf(payload, "%s%s\n",roomname,colors[used[2]]);
for (i=0;i<6;i++)
{
	if (rooms[2][i] != -7)
	{
		sprintf(tmp,"%s%d: %s\n",connection, (i+1), colors[used[rooms[2][i]]]);
		strcat(payload,tmp);
	}
}
sprintf(tmp,"ROOM TYPE: %s\n", roomtype[typeTracker[2]]);
strcat(payload,tmp);
//printf("%s",payload);
//PAYLOAD COMPLETE. WRITE TO FILE.

file_descriptor = open(room3, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);

if (file_descriptor == -1)
{
	printf("Could not open file -- open() failed on \"%s\"\n", room1);
	perror("In main()");
	exit(1);
}

nwritten = write(file_descriptor, payload, strlen(payload) * sizeof(char));
//__END ROOM 3
//__ROOM 4
memset(payload, '\0', sizeof(payload));
memset(tmp, '\0', sizeof(tmp));
sprintf(payload, "%s%s\n",roomname,colors[used[3]]);
for (i=0;i<6;i++)
{
	if (rooms[3][i] != -7)
	{
		sprintf(tmp,"%s%d: %s\n",connection, (i+1), colors[used[rooms[3][i]]]);
		strcat(payload,tmp);
	}
}
sprintf(tmp,"ROOM TYPE: %s\n", roomtype[typeTracker[3]]);
strcat(payload,tmp);
//printf("%s",payload);
//PAYLOAD COMPLETE. WRITE TO FILE.

file_descriptor = open(room4, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);

if (file_descriptor == -1)
{
	printf("Could not open file -- open() failed on \"%s\"\n", room1);
	perror("In main()");
	exit(1);
}

nwritten = write(file_descriptor, payload, strlen(payload) * sizeof(char));
//__END ROOM 4
//__ROOM 5
memset(payload, '\0', sizeof(payload));
memset(tmp, '\0', sizeof(tmp));
sprintf(payload, "%s%s\n",roomname,colors[used[4]]);
for (i=0;i<6;i++)
{
	if (rooms[4][i] != -7)
	{
		sprintf(tmp,"%s%d: %s\n",connection, (i+1), colors[used[rooms[4][i]]]);
		strcat(payload,tmp);
	}
}
sprintf(tmp,"ROOM TYPE: %s\n", roomtype[typeTracker[4]]);
strcat(payload,tmp);
//printf("%s",payload);
//IF YOU ARE READING THIS, YOU MUST BE VERY BORED.

file_descriptor = open(room5, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);

if (file_descriptor == -1)
{
	printf("Could not open file -- open() failed on \"%s\"\n", room1);
	perror("In main()");
	exit(1);
}

nwritten = write(file_descriptor, payload, strlen(payload) * sizeof(char));
//__END ROOM 5
//__ROOM 6
memset(payload, '\0', sizeof(payload));
memset(tmp, '\0', sizeof(tmp));
sprintf(payload, "%s%s\n",roomname,colors[used[5]]);
for (i=0;i<6;i++)
{
	if (rooms[5][i] != -7)
	{
		sprintf(tmp,"%s%d: %s\n",connection, (i+1), colors[used[rooms[5][i]]]);
		strcat(payload,tmp);
	}
}
sprintf(tmp,"ROOM TYPE: %s\n", roomtype[typeTracker[5]]);
strcat(payload,tmp);
//printf("%s",payload);
//PAYLOAD COMPLETE. WRITE TO FILE.

file_descriptor = open(room6, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);

if (file_descriptor == -1)
{
	printf("Could not open file -- open() failed on \"%s\"\n", room1);
	perror("In main()");
	exit(1);
}

nwritten = write(file_descriptor, payload, strlen(payload) * sizeof(char));
//__END ROOM6
//__ROOM 7
memset(payload, '\0', sizeof(payload));
memset(tmp, '\0', sizeof(tmp));
sprintf(payload, "%s%s\n",roomname,colors[used[6]]);
for (i=0;i<6;i++)
{
	if (rooms[6][i] != -7)
	{
		sprintf(tmp,"%s%d: %s\n",connection, (i+1), colors[used[rooms[6][i]]]);
		strcat(payload,tmp);
	}
}
sprintf(tmp,"ROOM TYPE: %s\n", roomtype[typeTracker[6]]);
strcat(payload,tmp);
//printf("%s",payload);
//PAYLOAD COMPLETE. WRITE TO FILE.

file_descriptor = open(room7, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);

if (file_descriptor == -1)
{
	printf("Could not open file -- open() failed on \"%s\"\n", room1);
	perror("In main()");
	exit(1);
}

nwritten = write(file_descriptor, payload, strlen(payload) * sizeof(char));
//__END ROOM7

//This block frees the memory allocated during runtime.
	for (i=0;i<7; i++) 			
	{
		free(rooms[i]);
	}
	free(rooms);
	free(used);
	free(typeTracker);
return 0;
}
