/*************************TCPclient.c****************************************
Student Name: CHANCE CANTWELL                Student Number: 0792138
Date:   OCTOBER 312017                          Course Name: CIS3210
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
  long long int chunkSize = 0; //size of buffer for reading/writing
  long long int totalBytesReceived = 0; //number of total bytes received
  double transferTimeInSeconds; //amount of seconds for file transfer to take place
  long long int sizeOfFile = 0; //size of file to send to server (in bytes)
  char fileData[50]; //string to send file name, file data, and chunk size
  char filename[21]; //name of file to send to server

  /*check to make sure user has the right number of arguments before proceeding*/
    if(argc < 3)
    {
        fprintf(stderr, "Sorry, but you did not specify server IP address and port number in form\nserver-IP-address:port-number, and/or a file name.  Please try again\n");
        exit(1);
    }
    	
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
            chunkSize = strtoll(argv[3], &temp, 10);
            printf("My chunk size is %lld\n", chunkSize);
        }
    } 
    else chunkSize = MAXRCVLEN;
	
    char buffer[chunkSize + 1]; /* +1 so we can add null terminator */
    char fileBuf[chunkSize + 1];
    
	/* open file to send to server for reading, ensure that it isn't NULL and file name is right
       length */
    if(((strlen(argv[2]))+1) > 21){
        printf("File name must be 20 characters or less long! Please restart and try again\n");
        exit(1);
    }
    inputFile = fopen(argv[2], "r");//argv[2], "r");
    if(inputFile == NULL){
        fprintf(stderr, "Sorry, the file you specified does not exist or right path\nhas not been specified.  Please restart and try again!\n");
	    exit(1);
	}
	strcpy(filename, argv[2]);
	/* find size of file */
    fseek(inputFile, 0L, SEEK_END);
    //int size = ftell(inputFile);
    sizeOfFile = ftell(inputFile);
    fseek(inputFile, 0L, SEEK_SET);
    printf("Size of file is %lld\n", sizeOfFile);
    if((strcasecmp(host, "localhost"))==0){
        sprintf(fileData, "%s %lld %lld ", LOCAL_TEST_FILE, sizeOfFile, chunkSize);
    }
    else sprintf(fileData, "%s %lld %lld ", filename, sizeOfFile, chunkSize);
    
    /* Consulted "How to use timer in C? - Stack Overflow" on October 5, 2017 for assistance with
     * implementing a timer for file transfer
     * URL: https://stackoverflow.com/questions/17167949/how-to-use-timer-in-c */
    
    // Connect to the server
	connect(mysocket, (struct sockaddr *)&dest, sizeof(struct sockaddr_in));
    clock_t start = clock();
	// first send - send file name, file size and chunk size
    send(mysocket, fileData, strlen(fileData), 0); 
    sleep(1);
    //send file contents to server and receive them back
    while((fgets(fileBuf, chunkSize, inputFile)) != NULL){
        send(mysocket, fileBuf, strlen(fileBuf), 0); 
        len = recv(mysocket, buffer, MAXRCVLEN, 0);
 
        /* We have to null terminate the received data ourselves */
        buffer[len] = '\0';
 
        //printf("Received %s (%d bytes).\n", buffer, len);
        totalBytesReceived = totalBytesReceived + len;
	}
    close(mysocket);
    clock_t finished = clock() - start;
    transferTimeInSeconds = ((double)((finished * 1000 )/ CLOCKS_PER_SEC)/1000);
    printf("I received %lld bytes total in %lf seconds!\n", totalBytesReceived, transferTimeInSeconds);
    fclose(inputFile);
    free(host);
    free(port);
	return EXIT_SUCCESS;
}
