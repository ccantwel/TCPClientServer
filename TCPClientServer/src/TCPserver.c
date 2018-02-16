/*************************TCPserver.c****************************************
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
	 /* variable declaration section */
    int portNumber = 0; int numFlag = 0;
    char * tempPort;
    int len; 
    char buffer[MAXRCVLEN + 1]; // +1 so we can add null terminator
	/*check to make sure user has the right number of arguments before proceeding*/
    if(argc != 2)
    {
        fprintf(stderr, "Sorry, but you did not specify port number.  Please try again\n");
        exit(1);
    }
    /*before converting, check supposed port number to ensure that it is a number*/
    numFlag = checkPortNumber(argv[1]);
    /* only if entire string is an number, convert to integer and store
     * as port number */
    if(numFlag == 0){
        portNumber = (int)strtol(argv[1], &tempPort, 10);
        printf("My port number is %d\n", portNumber);
    }
    
	struct sockaddr_in dest; // socket info about the machine connecting to us
	struct sockaddr_in serv; // socket info about our server
	int mysocket;            // socket used to listen for incoming connections
	socklen_t socksize = sizeof(struct sockaddr_in);

	memset(&serv, 0, sizeof(serv));           // zero the struct before filling the fields
	
	serv.sin_family = AF_INET;                // Use the IPv4 address family
	serv.sin_addr.s_addr = htonl(INADDR_ANY); // Set our address to any interface 
	serv.sin_port = htons(portNumber);        // Set the server port number 

	/* Create a socket.
   	   The arguments indicate that this is an IPv4, TCP socket
	*/
	mysocket = socket(AF_INET, SOCK_STREAM, 0);

	// bind serv information to mysocket
	int y = bind(mysocket, (struct sockaddr *)&serv, sizeof(struct sockaddr));
    if(y != 0){
        fprintf(stderr, "Cannot bind to port %d\n", portNumber);
        exit(1);
    }
    
	// start listening, allowing a queue of up to 10 pending connection
	listen(mysocket, 10);
	
	// Create a socket to communicate with the client that just connected
	int consocket = accept(mysocket, (struct sockaddr *)&dest, &socksize);
	bool data;
	while(consocket)
	{
		printf("Incoming connection!  Receiving from %s \n", inet_ntoa(dest.sin_addr));
		data = true;
		while(data == true){
		// Receive data from the client
		len = recv(consocket, buffer, MAXRCVLEN, 0);
		if(len == 0){
		  data = false;
          printf("Finished reading, closing this connection!\n");
		}
		buffer[len] = '\0';
		printf("%s\n", buffer);
				
		//Send data to client
		send(consocket, buffer, strlen(buffer), 0); 
		}
		
		//Close current connection
		close(consocket);
		
		//Continue listening for incoming connections
		consocket = accept(mysocket, (struct sockaddr *)&dest, &socksize);
	}

	close(mysocket);
	return EXIT_SUCCESS;
}
