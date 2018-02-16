/*************************TCPclient.c****************************************
Student Name: CHANCE CANTWELL                Student Number: 0792138
Date:   OCTOBER 6 2017                          Course Name: CIS3210
I have exclusive control over this submission via my password.
By including this statement in this header comment, I certify that:
     1) I have read and understood the University policy on academic integrity;
     2) I have completed the Computing with Integrity Tutorial on Moodle; and
     3) I have achieved at least 80% in the Computing with Integrity Self Test.
I assert that this work is my own. I have appropriately acknowledged any and all material
(data, images, ideas or words) that I have used, whether directly quoted or paraphrased.
Furthermore, I certify that this assignment was prepared by me specifically for this course.
****************************************************************************/

/* Code was used/modified with permission, source: "Lecture3Exaple.zip" (https://moodle.socs.uoguelph.ca/mod/resource/view.php?id=5020)
 * announced in CIS*3210 lecture on September 22, 2017 */

#include "TCPHeader.h"
#define MAXRCVLEN 500
#define PORTNUM 2300
 
int main(int argc, char *argv[])
{
  /*variable declarations */
  uint16_t portNumber = 0; int numFlag = 0; int x;
  char * next; char * temp; char * port;
  char * host; 
  int len, mysocket;
  struct sockaddr_in * destination;
  struct addrinfo hints, *result; //address information structs
  FILE * inputFile; //this is file to send to server
  int bufferSize = 0; //size of buffer for reading/writing
  int totalBytesReceived = 0; //number of total bytes received
  double transferTimeInSeconds; //amount of seconds for file transfer to take place

  char fileBuf[500];
		
  /*check to make sure user has the right number of arguments before proceeding*/
    if(argc < 3)
    {
        fprintf(stderr, "Sorry, but you did not specify server IP address and port number in form\nserver-IP-address:port-number, and/or a file name.  Please try again\n");
        exit(1);
    }
    printf("Entered: %s\n", argv[1]);
	
	/*set up host and port number*/
    /*tokenize string from argv[1] to obtain components, store host in correct
     location*/
    next = strtok(argv[1], ":");
    host = malloc(sizeof(strlen(next)+1));
    strcpy(host, next);
    next = strtok(NULL, "\n");
    /*before converting, check supposed port number to ensure that it is a number*/
    numFlag = checkPortNumber(next);
    /* only if entire string is an number, convert to integer and store
     * as port number */
    if(numFlag == 0){
        port = malloc(sizeof(strlen(next)+1));
		if(port == NULL){
		  fprintf(stderr, "Sorry, out of memory!  Exiting...\n");
		  exit(1);
		}
		strcpy(port, next);
	    portNumber = (uint16_t )strtol(next, &temp, 10);
        printf("My host is %s\n", host);
		printf("My port number is %d\n", portNumber);
    }
	struct sockaddr_in dest; // socket info about the machine connecting to us
 
	/* Create a socket.
	   The arguments indicate that this is an IPv4, TCP socket
	*/
	mysocket = socket(AF_INET, SOCK_STREAM, 0);
  
	memset(&dest, 0, sizeof(dest));                // zero the struct
	/* Consulted "Get IP address from hostname in C using Linux sockets" on 2017-10-01 for help with getaddrinfo and setting hints struct.  
     *URL: http://www.binarytides.com/hostname-to-ip-address-c-sockets-linux/ */
  	/* set hints struct */
	memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; 
	hints.ai_socktype = SOCK_STREAM;
  	
	// resolve host information using getaddrinfo 
	x = getaddrinfo(host, port, &hints, &result);
	if(x != 0){
	  fprintf(stderr, "Sorry, unable to get host information!\n");
	  exit(1);
	}
	    /*This line causes runLocalhost.sh to give Segmentation Fault error */
	    destination = (struct sockaddr_in *) result->ai_addr; 
        
	    
		
	//Initialize the destination socket information
	dest.sin_family = AF_INET;					   // Use the IPv4 address family
	dest.sin_addr.s_addr = destination->sin_addr.s_addr; 
	dest.sin_port = htons(portNumber);                // Set destination port number
 	
	/* if buffer size provided, convert and store, otherwise set default */
    if(argc > 3){
        /*before converting, check supposed buffer size value to ensure that it is a number*/
        numFlag = 0;
        numFlag = checkPortNumber(argv[3]);
        /* only if entire string is an number, convert to integer and store
        * as port number */
        if(numFlag == 0){
            bufferSize = (int)strtol(argv[3], &temp, 10);
            printf("My buffer size is %d\n", bufferSize);
        }
    } 
    else bufferSize = MAXRCVLEN;
	char buffer[bufferSize + 1]; /* +1 so we can add null terminator */
	//open file to send to server for reading, ensure that it isn't NULL
    inputFile = fopen(argv[2], "r");
	if(inputFile == NULL){
        fprintf(stderr, "Sorry, the file you specified does not exist or right path\nhas not been specified.  Please restart and try again!\n");
	    exit(1);
	}	
    /* Consulted "How to use timer in C? - Stack Overflow" on October 5, 2017 for assistance with
     * implementing a timer for file transfer
     * URL: https://stackoverflow.com/questions/17167949/how-to-use-timer-in-c */
    
    // Connect to the server
	connect(mysocket, (struct sockaddr *)&dest, sizeof(struct sockaddr_in));
    clock_t start = clock();
	while((fgets(fileBuf, 500, inputFile)) != NULL){
        send(mysocket, fileBuf, strlen(fileBuf), 0); 
        len = recv(mysocket, buffer, MAXRCVLEN, 0);
 
        /* We have to null terminate the received data ourselves */
        buffer[len] = '\0';
 
        printf("Received %s (%d bytes).\n", buffer, len);
        totalBytesReceived = totalBytesReceived + len;
	}
    close(mysocket);
    clock_t finished = clock() - start;
    transferTimeInSeconds = ((double)((finished * 1000 )/ CLOCKS_PER_SEC)/1000);
    printf("I received %d bytes total in %lf seconds!\n", totalBytesReceived, transferTimeInSeconds);
    fclose(inputFile);
    free(host);
    free(port);
	return EXIT_SUCCESS;
}
