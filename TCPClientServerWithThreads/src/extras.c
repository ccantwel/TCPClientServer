/*************************extras.c******************************************
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

#include "TCPHeader.h"
/* function to test whether a given string is a number (used for port number) */
int checkPortNumber(char * test){
    int x = 0; int flag = 0; //flag is set on first find of non-digit
    /*check incoming string to ensure that it is a number*/
    int len = strlen(test); //length of incoming string
    for(x = 0; x<len; x++){
        if(!isdigit(test[x])){
            flag = 1;
            break;
        }
    }
    return flag;
}
//Create an ExitControl and initilize its mutex
ExitControl* createExitControl()
{
    ExitControl* q = (ExitControl*)malloc(sizeof(ExitControl));
    q->clients = 0;
    q->exitFlag = false;
    pthread_mutex_init(&q->mutex, NULL);
    return q;
}
//Create a transfer list and initilize its mutex and condition variable
TransferList* createTransferList()
{
    TransferList* q = (TransferList*)malloc(sizeof(TransferList));
    q->head = q->tail = NULL;
    pthread_mutex_init(&q->mutex, NULL);
    
    //Initialize the condition variable
    pthread_cond_init(&q->cond, NULL);
    return q;
}

//Add transfer to transfer list
void addTransfer(TransferList* q, int id, char file[], long long int size, long long int chunks){
    TransferNode* node = (TransferNode*)malloc(sizeof(TransferNode));
    node->id = id;
    strcpy(node->filename, file);
    node->fileSize = size;
    node->chunkSize = chunks;
    node->next = NULL;

    // critical section
    pthread_mutex_lock(&q->mutex);
    if (q->tail != NULL) {
        q->tail->next = node;       // append after tail
        q->tail = node;
    } else {
        q->tail = q->head = node;   // first node
    }
    //Signal the consumer thread woiting on this condition variable
    pthread_cond_signal(&q->cond);
    pthread_mutex_unlock(&q->mutex);
    //fprintf(stderr, "Worker %d enqueues the message, signals cond variable, unlocks mutex, and goes to sleep\n", sender);
    sleep(2);

}
//Remove transfer from transfer list
void removeTransfer(TransferList* q)
{
    // critical section
    pthread_mutex_lock(&q->mutex);
    
    //Wait for a signal telling us that there's something on the list
    //If we get woken up but the list is still null, we go back to sleep
    while(q->head == NULL){
        //fprintf(stderr,"Transfer list is empty and removeTransfer goes to sleep (pthread_cond_wait)\n");
        pthread_cond_wait(&q->cond, &q->mutex);
        //fprintf(stderr,"removeTransfer is woken up - someone signalled the condition variable\n");
    }
    
    //By the time we get here, we know q->head is not null, so it's all good
    TransferNode* oldHead = q->head;
    q->head = oldHead->next;
    if (q->head == NULL) {
        q->tail = NULL;         // last node removed
    }
    free(oldHead);
      
    //Release lock
    pthread_mutex_unlock(&q->mutex);


}

ClientSocket* createClientSocket(int id, int socket){
    ClientSocket* q = (ClientSocket*)malloc(sizeof(ClientSocket));
    q->id = id;
    //fprintf(stderr, "test print2\n");
    q->socket = socket;
    return q;
}    


