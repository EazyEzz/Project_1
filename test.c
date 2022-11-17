#include <stdio.h> //for printf error checks
#include <string.h> //for printf strerror check
#include <unistd.h> //for write() and STDOUT_FILENO
#include <mqueue.h> //for EXIT_SUCCESS and EXIT_FAILURE
#include <sys/types.h> //for size_t, struct
#include <sys/wait.h>
#include <errno.h>
#include <ctype.h> //for isdigit()
#include <stdlib.h> //for atoi


#define QUEUE_NAME "/myMsgQ"
#define WRITE_FILE "mQueue.txt"

int main(int argc, char *argv[]){
    int MSG_SIZE;
    int MSG_CAPACITY = 10; //ERROR WHEN CAP IS 50
    char** inputFiles;

    int lFlag = 0;
    int wFlag = 0;
    int cFlag = 0;
    int LFlag = 0;
    int flags;

    // Loop handles the flags/options
    while((flags = getopt(argc, argv, "lwcL")) != -1){ /* getopt() returns -1 when all command line options are parsed*/
        switch(flags){
            case 'l':
                lFlag = 1;
                break;
            case 'w':
                wFlag = 1;
                break;
            case 'c': 
                cFlag = 1;
                break;
            case 'L':
                LFlag = 1;
                break;
            default: /* '?' */
                write(STDERR_FILENO, "Unknown Option Entered\n", 23);
                write(STDOUT_FILENO, "Options Are: -lwcL\n", 19);
                return 0;
        }
    }
    int sum =lFlag + wFlag + cFlag + LFlag;
    if(sum == 0){
        lFlag = 1;
        wFlag = 1;
        cFlag = 1;
    }

    char lF = lFlag + '0';
    write(STDOUT_FILENO, "Line Count Flag = ", 18);
    write(STDOUT_FILENO, &lF, 1);
    write(STDOUT_FILENO, "\n",1);

    char wF = wFlag + '0'; 
    write(STDOUT_FILENO, "Word Count Flag = ", 18);
    write(STDOUT_FILENO, &wF, 1);
    write(STDOUT_FILENO, "\n",1);

    char cF = cFlag + '0'; 
    write(STDOUT_FILENO, "Character Count Flag = ", 23);
    write(STDOUT_FILENO, &cF, 1);
    write(STDOUT_FILENO, "\n",1);

    char LF = LFlag + '0'; 
    write(STDOUT_FILENO, "Max Line Length Flag = ", 23);
    write(STDOUT_FILENO, &LF, 1);
    write(STDOUT_FILENO, "\n",1);

    argc -= optind; //handle the arguments after the flags if any
    argv += optind; //deal with the variables after flags
    
    char argCount = argc + '0';
    write(STDOUT_FILENO, "Argument Count = ", 17);
    write(STDOUT_FILENO, &argCount, 1 );
    write(STDOUT_FILENO, "\n", 1);

    if(argc == 0){
        write(STDERR_FILENO, "Incorrect Argument\n",19);
        return 1;
    }
    
    if(argc >= 1){
        char *isInt = argv[0];
        if(!isdigit(isInt[0])){ //checking for valid int and if int is negative
            write(STDERR_FILENO, "Error: Invalid Buffer Size Argument\n",36);
            return 1;
        } else {
            int x = atoi(argv[0]);
            if(x == 0 || x > 256){
                write(STDERR_FILENO, "Buffer Size Out Of Range: Must be > 0 OR <= 256\n", 48);
                return 1;
            } else{
                write(STDOUT_FILENO, "Buffer Accepted\n",16);
                printf("%s\n", argv[0]);
                MSG_SIZE = x;
            }
            if(argc == 1){
                write(STDOUT_FILENO, "DEFAULT FILE BEING USED: prj1inp.txt\n", 37);
                inputFiles = (char**)malloc(argc * sizeof(char));
                inputFiles[0] = (char*)malloc(12);
                inputFiles[0] = "prj1inp.txt";
                printf("%s\n", inputFiles[0]);
            } else if(argc > 1){
                write(STDOUT_FILENO, "Input Files Detected\n",21);
                inputFiles = (char**)malloc((argc - 1) * sizeof(char));
                for(int i = 1; i < argc; i++){
                    int t = 0;
                    char *z = argv[i];
                    while(z[t] != '\0'){
                        t++;
                    }
                    printf("Size of Input File = %d\n", t);
                    inputFiles[i-1] = (char*)malloc( (t+1) * sizeof(char));
                    inputFiles[i-1] = argv[i];
                    printf("%s\n", inputFiles[i-1]);
                } 
            }            
        }
    }

    struct mq_attr attr;

    attr.mq_maxmsg = MSG_CAPACITY;
    attr.mq_msgsize = MSG_SIZE;

    mqd_t mqd = mq_open(QUEUE_NAME, O_CREAT | O_RDWR, 0644, &attr);
    if(mqd == (mqd_t) -1){
        write(STDERR_FILENO, "QUEUE OPEN ERROR\n", 17);
        printf("%s\n", strerror(errno));
        mq_unlink(QUEUE_NAME);
        return 2;
    }
    write(STDOUT_FILENO, "QUEUE OPENED SUCCESSFULLY\n",26);

    int fd[2]; //file descriptor for PIPE fd[0] = read, fd[1] = write
    if(pipe(fd) == -1){
        write(STDERR_FILENO, "ERROR OPENING PIPE\n", 19);
        return 2;
    } else {
        write(STDOUT_FILENO, "PIPE OPENED\n", 12);
    }
    int fOut;
    if((fOut = creat(WRITE_FILE, 0644)) == -1){
        write(STDERR_FILENO, "UNABLE TO CREATE FILE DESTINATION\n", 34);
        return 2;
    } else {
        write(STDOUT_FILENO, "WRITE FILE CREATED\n", 19);
    }

    pid_t pid = fork();

    if(pid < 0){
        write(STDERR_FILENO, "Fork Failed\n", 12);
        return 3;
    }
    if (pid == 0){ //Child process branch
        close(fd[0]); //Child does not read from pipe

        int f1; //source file descriptor
        //loop through all argument files
        for(int i = 0; i <= (argc -1); i++){
            if((f1 = open(inputFiles[i], O_RDONLY, 0)) == -1){
                write(STDERR_FILENO, "UNABLE TO OPEN INPUT FILE\n",26);
                return 4;
            } else {
                write(STDOUT_FILENO, "INPUT FILE OPENED:: SUCCESS\n",28);
            }
            
            char buf1[MSG_SIZE]; //buffer for read from txt
            int inTxt; 
            //child reads from txt file
            while((inTxt = read(f1, buf1, MSG_SIZE)) > 0){
                if(write(fd[1], buf1,inTxt) != inTxt){
                    write(STDERR_FILENO, "WRITING_TO_PIPE_FAILED\n", 23);
                    close(f1);
                    return 4;
                }
            }
        }
        close(f1); //close source file descriptor
        close(fd[1]); //close child write end of pipe

        // write(STDOUT_FILENO, "Child Process1\n", 15);
        // write(STDOUT_FILENO, "Child Process2\n", 15);
        
        if(mq_close(mqd) == -1){
            write(STDERR_FILENO, "Child: Queue Close Error\n", 25);
            mq_unlink(QUEUE_NAME);
            return 4;
        }
        // CTRL + K ---> CTRL + C to comment, CTRL + U to uncomment
        // if(mq_unlink(QUEUE_NAME) == -1){
        //     write(STDERR_FILENO, "Child: mq_unlink Error\n", 23);
        //     printf("%s\n",strerror(errno));
        //     mq_unlink(QUEUE_NAME);
        //     return 4;
        // }

        write(STDOUT_FILENO, "Child Done\n", 11);
        return 0;
        //End of Child Branch
    } else { //Parent process
        close(fd[1]); //close parent write side of pipe

        int characters = 0;
        int words = 0;
        int lines = 0;
        int maxLineLn = 0;

        char buf2[MSG_SIZE];
        int readP;
        while((readP = read(fd[0], buf2, MSG_SIZE)) > 0){
            int i = 0;
            while(i < readP){
                if(buf2[i] == '\n' || buf2[i] == '\0'){
                    lines++;
                }
                if(buf2[i] != ' ' || buf2[i] != '\n' || buf2[i] != '\t' || buf2[i] != '\0'){
                    characters++;
                }
                if(buf2[i] == ' ' || buf2[i] == '\n' || buf2[i] == '\t' || buf2[i] == '\0'){
                    words++;
                }
                i++;
            }
        }
        //increment count for words and line for the last word
        words++; 
        lines++;

        printf("NUMBER OF LINES: %d\n", lines);
        printf("NUMBER OF CHARACTERS: %d\n", characters);
        printf("NUMBER OF OF WORDS: %d\n", words);
        // write(STDOUT_FILENO, "Parent Process1\n", 16);
        // write(STDOUT_FILENO, "Parent Process2\n", 16);

        if(mq_close(mqd) == -1){
            write(STDERR_FILENO, "Parent: Queue Close Error\n", 26);
            mq_unlink(QUEUE_NAME);
            return 4;
        }
        if(mq_unlink(QUEUE_NAME) == -1){
            write(STDERR_FILENO, "Parent: mq_unlink Error\n", 24);
            return 4;
        }

        wait(NULL);
        write(STDOUT_FILENO, "Parent Done\n", 12);
        return 0;
    } //End of parent branch
}