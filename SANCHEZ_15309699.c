//      Essmer Sanchez 15309699
//      Compile include: -lrt -lphread -lm

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <fcntl.h>
#include <string.h>

#define INC_NUMS 14000
#define MAX_CHILD_THREAD 14

/*Buff array for incoming nums, and descripors*/
int b[INC_NUMS];
int f1,f2;
pthread_attr_t attr;

struct data_struct{
    int pos;
    int tid;
    double square_root_sum;
    double geometric_calc;
    double arithmetic_calc;
};

/*functions to read_in txt file, runner creates worker threads within*/
/*comp_worker funcs used by worker threads for calculation*/
void txt_input(int argc, char *argv[]);
void* runner(void *arg);
void* comp_worker1(void *arg);
void* comp_worker2(void *arg);
void* comp_worker3(void *arg);


int main(int argc, char *argv[]){
    txt_input(argc, argv);

    if((f2 = creat(argv[2], 0644)) == -1){
        printf("Can't Create %s \n", argv[2]);
        return 1;
    }

    pthread_attr_init(&attr);
    pthread_t childT[MAX_CHILD_THREAD];
    struct data_struct *th_data = (struct data_struct *)malloc(14 * sizeof(struct data_struct));
    
    /* create the 14 worker child threads */
    for(int tnum  = 0; tnum  < MAX_CHILD_THREAD; tnum++){
        th_data[tnum].pos = tnum * 1000;
        th_data[tnum].tid = tnum + 1;
        if(pthread_create(&childT[tnum], &attr, &runner, &th_data[tnum]) != 0){
            printf("Failed to create thread");
            return 2;
        }
    }
    /*buffers used to join child threads */
    struct data_struct *out;
    char *out_buff = (char*)malloc(70);

    for(int i = 0; i < MAX_CHILD_THREAD; i++){
        /*for each child joining, copy over data to main struct array*/
        if(pthread_join(childT[i],(void**) &out) != 0){
            printf("Failed to join thread\n");
            return 3;
        }
        th_data[i].square_root_sum = out->square_root_sum;
        th_data[i].geometric_calc = out->geometric_calc;
        th_data[i].arithmetic_calc = out->arithmetic_calc;

        /*sprintf used to write message to buffer and convert int & dbl to string*/
        /*writing to allocated mem then writing to file*/
        /*size_t used to determine actual length of string in buff*/
        sprintf(out_buff,"Worker Child Pthread Number = %d:", th_data[i].tid);
        size_t tid_l = strlen(out_buff);
        write(f2, out_buff, tid_l);
        write(f2,"\t", 1);

        sprintf(out_buff,"Root of the Sum of Squares = %f", th_data[i].square_root_sum);
        size_t sq_l = strlen(out_buff);
        write(f2, out_buff, sq_l);
        write(f2,"\t", 1);

        sprintf(out_buff, "Geometric Average = %f", th_data[i].geometric_calc);
        size_t geo_l = strlen(out_buff);
        write(f2, out_buff, geo_l);
        write(f2,"\t", 1);

        sprintf(out_buff,"Arithmetic Average = %f", th_data[i].arithmetic_calc);
        size_t arith_l = strlen(out_buff);
        write(f2, out_buff, arith_l);
        write(f2, "\n",1);
    }

    /*Calculate min values*/
    /*holding value then comparing the remaining values to it*/
    double sq_min = th_data[0].square_root_sum;
    for(int i = 1; i < 4; i++){
        if(th_data[i].square_root_sum < sq_min){
            sq_min = th_data[i].square_root_sum;
        }
    }
    double geo_min = th_data[0].geometric_calc;
    for(int i = 1; i < 4; i++){
        if(th_data[i].geometric_calc < geo_min){
            geo_min = th_data[i].geometric_calc;
        }
    }
    double arith_min = th_data[0].arithmetic_calc;
    for(int i = 1; i < 4; i++){
        if(th_data[i].arithmetic_calc < geo_min){
            geo_min = th_data[i].arithmetic_calc;
        }
    }
    /*freeing up allocated struc mem since no longer needed*/
    free(th_data);

    sprintf(out_buff,"Main Program Thread: Min of the Root of the Sum of the Squares = %f", sq_min);
    size_t sqO = strlen(out_buff);
    write(f2, "\n", 1);
    write(f2,out_buff, sqO);
    write(f2,"\n", 1);

    sprintf(out_buff,"Main Program Thread: Min of the Geometric Averages = %f", geo_min);
    size_t geO = strlen(out_buff);
    write(f2,out_buff, geO);
    write(f2,"\n", 1);

    sprintf(out_buff, "Main Program Thread: Min of the Arithmetic Averages = %f", arith_min);
    size_t arO = strlen(out_buff);
    write(f2,out_buff, arO);
    write(f2,"\n", 1);

    free(out_buff);

    write(f2,"TERMINATING.", 12);
    close(f2);
    return 0;
}

void txt_input(int argc, char *argv[]){
    /*read in 14000 ints*/
    if(argc != 3){
        printf("Wrong number of command line arguments\n");
        exit(1);
    } else if(argv[1] == argv[2]){
        printf("ERROR: INPUT AND OUTPUT FILES ARE THE SAME\n");
        exit(1);
    }

    if((f1 = open(argv[1], O_RDONLY)) == -1){
        printf("Can't Open Input File");
        exit(1);
    }
   /*determining size of input file, SEEK_END traverses the whole file*/
    off_t size;
    if((size = lseek(f1, 0 , SEEK_END)) == -1){
        printf("Error Seeking End of File\n");
        exit(1);
    }
    /*reset seek to start of file*/
    /*not sure if this is necessary since seek is no longer used*/
    lseek(f1, 0 , SEEK_SET);

    char *buff = (char*)malloc(size+1);

    if((read(f1, buff, size)) <= 0){
        printf("Error Reading From File\n");
        exit(1);
    }

    for(int i = 0; i < 1; i++){ //NEED TO INCLUDE THIS FOR LOOP OTHERWISE
        buff[i];                //GET Segmentation fault (core dumped)
    }                           //NOT SURE WHY?!?!?!

    int j = 0;
    char temp = '\0';
    for(int i = 0; i < size+1; i++){
        /*traverse buff array one at a time and cocatenate to temp if its valid*/
        if(buff[i] != '\n' && buff[i] != ' ' && buff[i] != '\0'){
            strncat(&temp,&buff[i], 1);
        } else {
            b[j] = atoi(&temp);
            j++;
            temp = '\0';
            /*once cat complete reset temp buff and add value to global b array */
        }
    }

    close(f1);
    free(buff);
}

void* runner(void* arg){
    //worker child receives th_data[i] struct
    //ret_struct used to return th_data[i] to main with values, malloc freed in main
    struct data_struct *in_struct = (struct data_struct*)arg;
    struct data_struct *ret_struct =(struct data_struct*)malloc(sizeof(struct data_struct));
    double *pass_pos = malloc(sizeof(double));
    double res_arr[3];

    *ret_struct = *in_struct;
    *pass_pos = in_struct->pos;  

    pthread_t workers[3]; //computation workers for child threads
    
    //create 3 comp worker thread, each working on its own specific calculation
    for(int i = 0; i < 3; i++){
        if(i == 0){
            if(pthread_create(&workers[i], &attr, comp_worker1, pass_pos) != 0){
                printf("Failed to create thread\n");
                exit(1);
            } 
        } else if(i == 1){
            if(pthread_create(&workers[i], &attr, comp_worker2, pass_pos) != 0){
                printf("Failed to create thread\n");
                exit(1);
            }
        }else if(i == 2){
            if(pthread_create(&workers[i], &attr, comp_worker3, pass_pos) != 0){
                printf("Failed to create thread\n");
                exit(1);
            }
        }
    }

    for(int i = 0; i < 3; i++){
        /*Joing computation threads with returned solutions*/
        /*temp used as buffer and t used to convert void** */
        void *temp;
        double *t;
        if(i == 0){
            if(pthread_join(workers[i],&temp) != 0){
                printf("Failed to create thread\n");
                exit(1);
            }
            t = (double*)temp;
            res_arr[i] = *t;
        } else if(i == 1){
            if(pthread_join(workers[i],&temp) != 0){
                printf("Failed to create thread\n");
                exit(1);
            }
            t = (double*)temp;
            res_arr[i] = *t;
        }else if(i == 2){
            if(pthread_join(workers[i],&temp) != 0){
                printf("Failed to create thread\n");
                exit(1);
            }
            t = (double*)temp;
            res_arr[i] = *t;
        }
    }
    ret_struct->square_root_sum = res_arr[0];
    ret_struct->arithmetic_calc = res_arr[1];
    ret_struct->geometric_calc = res_arr[2];

    return (void *)ret_struct;
}

void *comp_worker1(void *arg){
    /*ROOT SUM SQUARES CALCULATION*/ 
    /*receives &in_struct->pos as double pass_pos*/
    /*ret_sq needed to return void** to join() */
    double *x = (double*)arg;
    double *ret_sq = (double *)malloc(sizeof(double));
    int p = *x;
    double sq_sum = 0.0;

    for(int i = 0; i < 1000; i++){
        sq_sum += b[p + i] * b[p + i];
    }

    sq_sum = sqrt(sq_sum);

    *ret_sq = sq_sum;
    return (void**)ret_sq;
}

void *comp_worker2(void *arg){ 
    /*GEOMETRIC AVERAGE CALCULATION*/
    /*receives &in_struct->pos as double pass_pos*/
    /*ret_geo needed to return void** to join() */
    double *x = (double*)arg;
    double *ret_geo = (double *)malloc(sizeof(double));
    int p = *x;
    double sum = 0.0;

    /*Since multiplying 1000 #'s to each other would cause an overflow*/
    /*a strategy to calculate geomtric average is to first:*/
    /*find sum of natual log of each number*/
    /*then divide the sum by range, 1000 in this case*/
    /*lastly do e^(result of sum)*/
    /*****NOTE:*****/
    /* If a zero is encountered, I could have the whole Geometric Average for this range */ 
    /* equal to 0. But I decided to skip over it and continue */
    for(int i = 0; i < 1000; i++){
        int check = b[p+i];
        if(check == 0){
            /*do nothing to skip 0*/
        } else{
            sum += log(b[p + i]);
        }
    }
    /*range of numbers remains the same regardless if zero is found*/
    sum = sum/1000;

    *ret_geo = (double)exp(sum);

    return (void**)ret_geo;
}

void *comp_worker3(void *arg){
    /*ARITHMETIC AVERAGE CALCULATION*/
    /*receives &in_struct->pos as double pass_pos*/
    /*ret_arith needed to return void** to join() */
    double *x = (double*)arg;
    double *ret_arith = (double *)malloc(sizeof(double));
    int p = *x;
    double arith_avg = 0.0;

    for(int i = 0; i < 1000; i++){
        arith_avg += b[p + i];
    }
    arith_avg = (double)arith_avg/1000;

    *ret_arith = arith_avg;
    return (void**)ret_arith;
}