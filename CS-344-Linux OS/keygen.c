/* 
 * AUTHOR: MORANSP
 * CLASS: CS344 OPERATING SYSTEMS I
 * DATE: 3/14/2017
 * DESCRIPTION: Key generation script. Writes a random key value to STDOUT of n characters.
 * USAGE: keygen [int]  > mykey
 * NOTE: OUTPUTS TO STDOUT
 * */


#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<time.h>

void error(const char *msg) { perror(msg); exit(1); } //Error function used for reporting issues

int main(int argc, char *argv[])
{
	if (argc < 2) { error("Not enough parameters for keygen operation"); }

		int userint = atoi(argv[1]);				//Convert to integer
		srand(time(0));						//seed rand()
		int i;		
		int randVal = 0;
		char targetstring[74096];
		char catme[10];
		memset(targetstring, '\0', sizeof(targetstring));	//clear out targetstring/catme
		memset(catme, '\0', sizeof(catme));
		for (i = 0; i < userint; i++) {				//while < the usernumber
			randVal = rand() % 27;				//grab a value from 0 to 26
			if (randVal == 0) {randVal = 32;}		//if zero, set to ASCII 32
			else {randVal += 64;}				//else add 64
			sprintf(catme, "%c", randVal);			//add the char to the cat file
			strcat(targetstring, catme);			//cat into target
		}
		write(1, targetstring, strlen(targetstring));		//write to stdout
		printf("\n"); 						//add a newline to the console
		return 0;
}
