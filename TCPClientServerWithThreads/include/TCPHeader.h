/*************************TCPHeader.h****************************************
Student Name: CHANCE CANTWELL                Student Number: 0792138
Date:   OCTOBER 31 2017                         Course Name: CIS3210
I have exclusive control over this submission via my password.
By including this statement in this header comment, I certify that:
     1) I have read and understood the University policy on academic integrity;
     2) I have completed the Computing with Integrity Tutorial on Moodle; and
     3) I have achieved at least 80% in the Computing with Integrity Self Test.
I assert that this work is my own. I have appropriately acknowledged any and all material
(data, images, ideas or words) that I have used, whether directly quoted or paraphrased.
Furthermore, I certify that this assignment was prepared by me specifically for this course.
****************************************************************************/

#ifndef TCPHeader
#define TCPHeader

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <time.h>
#include <pthread.h>
#define MAXCLIENTS 10
#define LOCAL_TEST_FILE "blabla.txt"
#define LOCAL_TEST_FILE2 "blabla2.txt"
#define LOCAL_TEST_FILE3 "blabla3.txt"
#define LOCAL_TEST_FILE4 "blabla4.txt"
#define LOCAL_TEST_FILE5 "blabla5.txt"
#define LOCAL_TEST_FILE6 "blabla6.txt"
#define LOCAL_TEST_FILE7 "blabla7.txt"
#define LOCAL_TEST_FILE8 "blabla8.txt"
#define LOCAL_TEST_FILE9 "blabla9.txt"
#define LOCAL_TEST_FILE10 "blabla10.txt"

void * workerThreadFunction(void * socketInfo);
//void * uiThreadFunction();

typedef enum { false, true } bool;
/* ExitControl struct - contains exit flag and counter for clients */
   typedef struct {
    int clients; // counts number of clients
    bool exitFlag;
    pthread_mutex_t mutex;
} ExitControl;
/*Worker socket struct */ 
typedef struct clisock {
    int id;
    int socket;
 } ClientSocket;
 
//Transfer Node
typedef struct transfer_node {
    int id;
    char filename[21]; 
    long long int fileSize;
    long long int chunkSize;
    struct transfer_node* next;
} TransferNode;
//Transfer list - a singly linked list
//Remove from head, add to tail
typedef struct {
    TransferNode* head;
    TransferNode* tail;
    pthread_mutex_t mutex;
    
    //Add a condition variable
    pthread_cond_t cond;
} TransferList;

int checkPortNumber(char * test); /* function to test whether a given string is a number (used for port number) */
ExitControl* createExitControl(); //Create an ExitControl and initilize its mutex
TransferList* createTransferList(); //Create a transfer list and initilize its mutex and condition variable
void addTransfer(TransferList* q, int id, char file[], long long int size, long long int chunks);
//Add transfer to transfer list
void removeTransfer(TransferList* q); //Remove transfer from transfer list
//void displayTransferList(TransferList* q); //function that iterates through list and displays active transfers
ClientSocket* createClientSocket(int id, int socket);
#endif
