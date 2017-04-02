/* 
 * Filename: otp_enc
 * Author: MORANSP
 * Class: CS344
 * Date: 03/15/2017
 * Description: client process for encoding. Opens a file and checks the input for bad characters. 
 * */


// filename: client.c
// NOTE: pairs with server.c
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<fcntl.h>

void error(const char *msg) { perror(msg); exit(1); } //Error function used for reporting issues

int main(int argc, char *argv[])	//otp_enc plaintext key port
{
	int socketFD, portNumber, charsWritten, charsRead;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;
	char * id = "otp_enc";
	char plainbuffer[4096];
	char keybuffer[74096];
	char buffer [4096];	
	int plaintextFD, keyFD;
	//check usage & args
	if (argc < 4) { fprintf(stderr,"USAGE: %s textfile keyfile port\n", argv[0]); exit(1); } 

	//open the input plaintext file
	memset(plainbuffer, '\0', sizeof(plainbuffer));
	plaintextFD = open(argv[1], O_RDONLY);
	read(plaintextFD, plainbuffer, 4096);
//	printf("the file contains:%slength:%d\n", plainbuffer, strlen(plainbuffer));
	//check for bad characters
	if (strstr(plainbuffer, "$")){ fprintf(stderr, "BAD CHAR FOUND, ABORTING\n"); exit(1); }
	close(plaintextFD);

	//plainbuffer now contains the contents of of the file. Now do the same for the key.

	//open the input key file
	memset(keybuffer, '\0', sizeof(keybuffer));
	keyFD = open(argv[2], O_RDONLY);
	read(keyFD, keybuffer, 74096);
//	printf("the key file contains:%slength:%d\n", keybuffer, strlen(keybuffer));
	//Compare buffer size to determine if key is too small for input file.
	if(strlen(keybuffer) < strlen(plainbuffer)){ fprintf(stderr, "KEY TOO SMALL, ABORTING\n"); exit(1); }


	/***********************************************************************************
 	* If the program reaches this point, both the plainfile and the keyfile are valid. *
 	*     Therefore, we can begin communicating with the encryption program.           *
 	***********************************************************************************/

	//Set up the server addrses struct
	memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[3]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverHostInfo = gethostbyname("localhost"); // Convert the machine name into a special form of address
	if (serverHostInfo == NULL) {fprintf(stderr, "CLIENT: ERROR, no such host\n"); exit(0); }
	memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length);
	//copy in the address
	
	//Set up the socket
	socketFD = socket(AF_INET, SOCK_STREAM, 0); //create the socket
	if (socketFD < 0) error("CLENT: ERROR opening socket");
	
	//connect to server
	if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0)
		error("CLIENT: ERROR connectiong");
	
	// Get input message from user
//	printf("CLIENT: sending process name \"%s\" to daemon\n", id);
//	memset(buffer, '\0', sizeof(buffer));
//	fgets(buffer, sizeof(buffer) -1, stdin);
//	buffer[strcspn(buffer, "\n")] = '\0'; //Remove the trailing \n that fgets adds <- remember this.
	
	// Send message to server
	charsWritten = send(socketFD, id, strlen(id), 0);
	if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
	if (charsWritten < strlen(buffer)) printf("CLIENT: WARNING: Not all data written to socket!\n");

	// Get return message from server
	memset(buffer, '\0', sizeof(buffer));
	charsRead = recv(socketFD, buffer, sizeof(buffer) -1, 0); // Read data from the socket, leaving \0
	if (charsRead < 0) error("CLIENT: ERROR reading from socket");
//	printf("CLIENT: I received this from the server: \"%s\"\n", buffer);
	if(!strstr(buffer, "GO")){
		close(socketFD); return 0;
//		printf("CLIENT: Sending plaintext contents to daemon.");
	}

	// Send the contents of the plaintext file
	charsWritten = send(socketFD, plainbuffer, strlen(plainbuffer), 0);
	if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
	if (charsWritten < strlen(buffer)) printf("CLIENT: WARNING: Not all data written to socket!\n");

	// Get return message from server
	memset(buffer, '\0', sizeof(buffer));
	charsRead = recv(socketFD, buffer, sizeof(buffer) -1, 0); // Read data from the socket, leaving \0
	if (charsRead < 0) error("CLIENT: ERROR reading from socket");
//	printf("CLIENT: I received this from the server: \"%s\"\n", buffer);
	if(strstr(buffer, "GO")){
//		printf("CLIENT: Sending key file contents to daemon.");
	}
	// Send the contents of the plaintext file
	charsWritten = send(socketFD, keybuffer, strlen(keybuffer), 0);
	if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
	if (charsWritten < strlen(buffer)) printf("CLIENT: WARNING: Not all data written to socket!\n");

	/***********************************************************************
 	* At this point, the daemon has everything it needs to cipher the text.*
 	*     Now, we wait for the server to respond with the ciphered text,   *
 	*     and then push it to stdout.                                      *
	***********************************************************************/
	/**/
	memset(buffer, '\0', sizeof(buffer));
	charsRead = recv(socketFD, buffer, sizeof(buffer) -1, 0); // Read data from the socket, leaving \0
	if (charsRead < 0) error("CLIENT: ERROR reading from socket");
//	printf("CLIENT: The ciphered text is: \"%s\"\n", buffer);
	/**/	
	fprintf(stdout, "%s\n", buffer);

	close(socketFD); // Close the socket
	return 0;		
}
