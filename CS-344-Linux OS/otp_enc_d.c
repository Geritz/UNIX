/*
 * Filename: otp_enc_d.c
 * Author: MORANSP
 * Class: CS344
 * Date: 03/15/2017
 * Description: pseudo-daemon for encoding.
 */

//server.c
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>

void error(const char *msg) { perror(msg); exit(1); } // Error function used for reporting issues

int main(int argc, char *argv[])
{
	int listenSocketFD, establishedConnectionFD, portNumber, charsRead;
	socklen_t sizeOfClientInfo;
	char buffer[4096];
	char plainbuffer[4096];
	char keybuffer[74096];
	struct sockaddr_in serverAddress, clientAddress;

	if (argc < 2) { fprintf(stderr, "USAGE: %s port\n", argv[0]); exit(1); } // Check usage & args

	//Set up the address struct for this process (the server)
	memset((char *)&serverAddress, '\0', sizeof(serverAddress));
	portNumber = atoi(argv[1]); 		//Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET;	//Create a network-capable socket
	serverAddress.sin_port = htons(portNumber);	//Store the port number
	serverAddress.sin_addr.s_addr = INADDR_ANY; 	//Any address is allowed for connection to this procss
	
	//Set up the socket
	listenSocketFD = socket(AF_INET, SOCK_STREAM, 0);//Create the socket
	if (listenSocketFD < 0) error("ERROR opening socket");

	//Enable the socket to begin listening
	if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
		error("ERROR on binding");
	listen(listenSocketFD, 5); 	//This call flips the socket on - it can now receive up to 5 conns.

	//Accept a connection, blocking if one is not available until one connects
	while(1) {
	sizeOfClientInfo = sizeof(clientAddress); //Get the size of the address for the client
       establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo);
	if (establishedConnectionFD < 0) error("ERROR on accept");

	//Get the message from the client and display it
	memset(buffer, '\0', 4096);
	charsRead = recv(establishedConnectionFD, buffer, 4095, 0); //read message from socket
	if (charsRead < 0) error("ERROR reading from socket");
	if (!strstr(buffer, "otp_enc")){ 
		charsRead = send(establishedConnectionFD, "otp_enc_d", 9, 0);
		if (charsRead < 0) error("ERROR writing to socket");
		fprintf(stderr, "OTP_ENC_D: invalid connection, rejecting!\n"); 
		close(establishedConnectionFD); 
	} //validate
//	printf("ENC DAEMON: received connection from: \"%s\"\n", buffer);
	else {
	//Send a success message back to the client
	charsRead = send(establishedConnectionFD, "GO", 2, 0);
	if (charsRead < 0) error("ERROR writing to socket");

	//At this point, if the program has not exited, the daemon is ready for the plaintext.
	
	//Get the message from the client and display it
	memset(plainbuffer, '\0', sizeof(plainbuffer));
	charsRead = recv(establishedConnectionFD, plainbuffer, sizeof(plainbuffer)-1, 0); //read message socket
	if (charsRead < 0) error("ERROR reading from socket");
//	printf("ENC DAEMON: contents of plaintext: \"%s\"\n", plainbuffer);

	//Send a success message back to the client
	charsRead = send(establishedConnectionFD, "GO", 2, 0);
	if (charsRead < 0) error("ERROR writing to socket");

	//Get the message from the client and display it
	memset(keybuffer, '\0', sizeof(keybuffer));
	charsRead = recv(establishedConnectionFD, keybuffer, sizeof(keybuffer)-1, 0); //read message socket
	if (charsRead < 0) error("ERROR reading from socket");
//	printf("ENC DAEMON: contents of key file: \"%s\"\n", keybuffer);


	/************************************************************************************************* 
 	* At this point in execution. The text file and key have been passed into the encryption program.*
 	*     What needs to happen now, is the program will cipher the plaintext, and then return it to  *
 	*     The calling program. This will happen in a for loop, bound by the size of the input string.*
 	*************************************************************************************************/
	char char1;
	char char2;
	char convertedResult;
	int convert1;
	int convert2;
	int result;
	int ptlen = strlen(plainbuffer)-1;
	int current;
	memset(buffer,'\0', sizeof(buffer));
	for (current = 0; current < ptlen; current++){
		char1 = plainbuffer[current];
		char2 = keybuffer[current];
		convert1 = (int)char1;
		convert2 = (int)char2;
//		printf("%d char1, %d char2\n", convert1, convert2);
		//the conversion keys now contain the workable chars.
		//Now, do the math.
		if (convert1 == 32) { convert1 = 0; }
		else { convert1 -= 64; }
		if (convert2 == 32) { convert2 = 0; }
		else { convert2 -= 64; }
//		printf("%d adjusted char1, %d adjusted char2\n", convert1, convert2);
		//the two numbers have been reduced to their mathematical equivalents.
		//Now, add them together. and adjust as necessary.
		result = convert1 + convert2;
		if (result > 26) { result -= 26; }
		else if (result < 0) { result += 26; }
		//the char has been ciphered. if the result is not zero, add 64, and convert.
		//else, if it is zero, add 32 instead, and convert
		if (result == 0) { result += 32; }
		else { result += 64; }
		convertedResult = (char)result;
//		printf("the ciphered char is: %c\n", convertedResult);
		buffer[current] = convertedResult;
	}
//	printf("ENC DAEMON: Sending response to CLIENT\n");
	//Send the completed cipher to the client
	charsRead = send(establishedConnectionFD, buffer, strlen(buffer), 0);
	if (charsRead < 0) error("ERROR writing to socket");

	//And done! Close the connection.
	close(establishedConnectionFD);		//Close the existing client socket
	}
	}
	close(listenSocketFD);			//Close the listening socket
	return 0;
}
