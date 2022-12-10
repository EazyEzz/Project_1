#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <fcntl.h>
#include <string.h>

int b[3];

int main(int argc, char *argv[]){
    /*read in 14000 ints*/
    if(argc != 3){
        printf("Wrong number of command line arguments\n");
        return 1;
    }

    int f1, f2, f;

    if((f1 = open(argv[1], O_RDONLY, 0)) == -1){
        printf("Can't Open %s \n", argv[1]);
        return 2;
    }

    if((f2 = creat(argv[2], 0644)) == -1){
        printf("Can't Create %s \n", argv[2]);
        return 2;
    }

    
    off_t size;
    if((size = lseek(f1, 0 , SEEK_END)) == -1){
        printf("Error Seeking End of File\n");
        return 3;
    }

    lseek(f1, 0 , SEEK_SET);

    printf("Size of file = %ld\n", size);

    char *buff = (char*)malloc(size+1);
    int r = 0;

    if((read(f1, buff, size)) < 0){
        return 4;
    }
    printf("Read Complete\n");

    // for(int i = 0; i < 1; i++){
    //    printf("%c",  buff[i]);
    // }
    printf("\n");

    int j =0;
    char temp = '\0';
    for(int i = 0; i < size+1; i++){
        if(buff[i] != '\n' && buff[i] != ' ' && buff[i] != '\0'){
            strncat(&temp,&buff[i], 1);
        } else {
            b[j] = atoi(&temp);
            printf("b[%d] = %d\n", j , b[j]);
            ++j;
            temp = '\0';
        }
    }
    // printf("\n");
    // for(int i = 0; i < 3; i++){
    //     printf("%d\n",  b[i]);
    // }

    printf("\n");

    free(buff);

    return 0;
}