#include "TCPHeader.h"

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
