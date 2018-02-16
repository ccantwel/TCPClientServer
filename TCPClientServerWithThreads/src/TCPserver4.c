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
#define LOCAL_TEST_FILE "blabla.txt"
#define MAXCLIENTS 10
void * workerThreadFunction(void * socketDescription);
//void * workerThreadFunction(void * clientSocketInfo);
void * uiThreadFunction();
ExitControl* q;
TransferList* translist;

int main(int argc, char *argv[])
{
    //exitFlag = false;
    //ExitControl* q = createExitControl();
    q = createExitControl();
    translist = createTransferList();
    ClientSocket* clientSockets[10];
    /* variable declaration section */
    int portNumber = 0; int numFlag = 0; //bool uiCreate = false;
    char * tempPort; int x = 0;
    //int * uiPtr;
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
        return 1;
    }
    
	// start listening, allowing a queue of up to 10 pending connection
	listen(mysocket, MAXCLIENTS);
	
/* Create the UI thread */
	pthread_t uiThread; 	
	//if((pthread_create(&uiThread, NULL, uiThreadFunction, &q))<0){
    if((pthread_create(&uiThread, NULL, uiThreadFunction, NULL))<0){
			fprintf(stderr, "Cannot create UI thread\n");
			return 1;
	}
	//pthread_join(uiThread, NULL);
	pthread_t workerThread[MAXCLIENTS];
	// Create a socket to communicate with the client that just connected
	int consocket;
	int * workerSocket;
 
   	while((consocket = accept(mysocket, (struct sockaddr *)&dest, &socksize))&& x < MAXCLIENTS)
	{   
        //fprintf(stderr, "%d\n", x);
        if((pthread_mutex_trylock(&q->mutex))==0){
            if(q->exitFlag == true){
                pthread_mutex_unlock(&q->mutex);            
                break;
            }
            pthread_mutex_unlock(&q->mutex);            
        }
        clientSockets[x] = createClientSocket(x, consocket);
        //printf("Incoming connection!\n");
		workerSocket = malloc(sizeof(int));
		*workerSocket = consocket;
		if((pthread_create(&workerThread[x], NULL, workerThreadFunction, (void*)clientSockets[x]))<0){
			fprintf(stderr, "Cannot create worker thread\n");
			return 1;
		}
		pthread_join(workerThread[x], NULL);
        x = x + 1;
	}

	fprintf(stderr, "Received exit flag!\n");
	fprintf(stderr, "Closing socket and exiting\n");
    pthread_mutex_destroy(&q->mutex);
    free(q);
    close(mysocket);
    return 0;
}	
/* Transfer thread: this function takes the file information, writes the contents to disk, and sends the data back to client */
//void * workerThreadFunction(void * clientSocketInfo){
void * workerThreadFunction(void * socketDescription){
	bool data; int len = 0; char * next;
    long long int fileSize; long long int chunkSize; char filename[21];
	char buffer1[MAXRCVLEN + 1]; // +1 to add null terminator	
	char * buffer2;
    FILE * sendtoFile; //file to write the received bytes to 
	//ClientSocket* mysock = (ClientSocket*)clientSocketInfo;
	int socket = *(int*)socketDescription;
    /*increment client counter when client, decrement counter when job is done and thread terminates */ 
    if((pthread_mutex_trylock(&q->mutex))==0){
            q->clients = (q->clients) + 1;
            pthread_mutex_unlock(&q->mutex);            
        }
	//first receive - get file size, file name and chunk size
    len = recv(socket, buffer1, MAXRCVLEN, 0);
    if(len <= 0){ 
        fprintf(stderr, "Nothing received!\n"); 
        exit(1);         
    }
    next = strtok(buffer1, " ");
    strcpy(filename, next);
    next = strtok(NULL, " ");
    fileSize = strtoll(next, NULL, 10);
    next = strtok(NULL, " ");
    chunkSize = strtoll(next, NULL, 10);
    
    /*open sendtoFile for writing */
    sendtoFile = fopen(LOCAL_TEST_FILE,"w");
    //sendtoFile = fopen(filename,"w");
    if (sendtoFile == NULL) {
        printf ("error opening sendtoFile\n"); 
        exit(0);        
    }
    /*add transfer to transferList */
    //addTransfer(translist, mysock->id, filename, fileSize, chunkSize);
    /* set up buffer for file contents */
    buffer2 = malloc(sizeof(char)*(chunkSize + 1));
    data = true;
	while(data == true){
		//Receive file contents from the client, write to disk and send back
		len = recv(socket, buffer2, chunkSize, 0);
		if(len == 0){
            //remove from transfer list
            int success = 0; 
            //success = removeTransfer(translist);
            if((pthread_mutex_trylock(&q->mutex))==0){
            q->clients = (q->clients) - 1;
            pthread_mutex_unlock(&q->mutex);            
          } 
		  data = false;
          //fprintf(stderr, "Finished reading, closing this connection!\n");
		}
		buffer2[len] = '\0';
		// write file to disk
		//fprintf(stderr, "buffer: %s\n", buffer2);  //for testing purposes
		fputs(buffer2, sendtoFile);
		//printf("%s\n", buffer2);
				
		//Send copy of file contents to client
		send(socket, buffer2, strlen(buffer2), 0); 
		}
		
		//Close current connection and free socket and file contents buffer
		fclose(sendtoFile);
		close(socket);
        free(socketDescription);
		//free(mysock);
        free(buffer2);
		return 0;

}
/* UI thread: this function provides user with menu that allows them to either view transfers
 * or exit the server */
void * uiThreadFunction(){//void * quit){
    //ExitControl* q = (ExitControl*)quit;
    //int exitvar;
	int choice;
	while(1){	
		printf("Threaded TCP Server Application\n Choices are as follows:\n1. Display Active Transfers (files being downloaded)\n2. Exit the server\n");
		printf("Enter number of choice here: ");
		scanf("%d", &choice);
		switch(choice){
			case 2 : 
                pthread_mutex_lock(&q->mutex);
                /* if a client is connected, set exit flag, otherwise just terminate whole 
                 * program */
                if(q->clients > 0){
                    q->exitFlag = true;
                    pthread_mutex_unlock(&q->mutex);
                    pthread_exit(NULL);
                }
                else{
                    pthread_mutex_unlock(&q->mutex);
                    exit(0);
                }
                break;
			case 1 : 
				//display transfer list
                //displayTransferList(translist);
                //displayTransferList(translist);
				break;
			default :
				printf("Incorrect choice, please try again!\n");			
				break;
		}
	}
}

	
