#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
    FILE* fptr=NULL;
    char path[255];
    printf("Enter the Image Path");
    scanf("%s",path);
    fptr =fopen(path,'r');

    if(fptr){
        perror("Image Not Opened");
    }
    return 0;

}

