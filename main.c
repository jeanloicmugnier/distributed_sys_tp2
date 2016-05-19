/* 
 * File:   main.c
 * Author: Jean-Loïc Mugnier <mugnier at polytech.unice.fr>
 *
 * Created on May 7, 2016, 12:49 AM
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <string.h>
#include "somador.h"
//#include "mtwist.h"
#include "mtwist.c"

#define MAX = 100;
#define MIN = -100;
#define N8 100000000
#define N9 1000000000
#define N10 10000000000 // must use long!!!




char * list; // list of random number 
int *acc; // sum of the numbers in the list
char* sum_lock;
char* lock_index;

//long * med;

double set_beg_time(clk *clk) {
    clk->beg = clock();
    return 0;
}

double set_end_time(clk *clk) {
    clk->end = clock();
    return 0;
}

/**
 * Aquire the lock, allow the thread to execute the CZ
 * however, no one else can access it
 * @return 
 */
void acquire(char* lock) {
    while (test_and_set2(
            lock));
}

/**
 * Format the time
 * @return pointer to 
 */
char * format_time(double diff) {
    char str[20];
    sprintf(str, "%f", diff);
    char * ex = "exec time: ";
    char * con = concat(ex, str);
    return con;
}

char* concat(char *s1, char *s2) {
    char *result = malloc(strlen(s1) + strlen(s2) + 1); //+1 for the zero-terminator
    //in real code you would check for errors in malloc here
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

void print_time(double time) {
    //    double tx = clk_calc_time(clk);
    char* res = format_time(time);
    printf(" res: %s \n", res);

}

/**
 * 
 * Release the lock, so another thread can enter the CZ
 * @return 
 */
void release(char* lock) {
    *lock = 0;
}

/**
 * 
 * @param flag
 * @return 
 */
int test_and_set2(char* flag) {
    int anterior = *flag;
    *flag = 1;
    return anterior;
}

/**
 * fill the vector 
 * @param n, size of the vector
 * @return 
 */
int fill_vector(long *n) {
    int rnd;
    for (int i = 0; i < *n; i++) {
        rnd = get_rand();
        list[i] = rnd;
    }
    return 0;
}

/**
 *NÃO ROLA
 */
int fill_with_threads(long *n, int *k) {
    printf("fill\n");

    long med = *n / (*k);
    int rest = *n - (med * *k);
    pthread_t *thread = malloc(sizeof (pthread_t) * *k);
    for (int i = 0; i < *k; i++) {

        sum_args *args = malloc(sizeof (sum_args));
        args->index = med*i;
        args->nb = med;
        if (pthread_create(&thread[i], NULL, &add, (void *) args)) { // 3 arg = func to exec 4 arg = arg passed to func
            printf("thread : %d \n", i);
            printf("Error while creating threds. \n");
            exit(EXIT_FAILURE);
        }
        //        free(args);
    }
    for (int i = 0; i < *k; i++) {
        pthread_join(thread[i], NULL);
        printf("WAITING THREAD:  %d \n", i);
    }



    pthread_t *lon_th = malloc(sizeof (pthread_t));
    // only enter the loop if rest>0
    if (rest) {
        sum_args * args = malloc(sizeof (sum_args));
        args->index = *k*med;
        args->nb = rest;
        if (pthread_create(&lon_th[0], NULL, &add, (void *) args)) { // 3 arg = func to exec 4 arg = arg passed to func
            printf("Error while creating threds. \n");
            exit(EXIT_FAILURE);
        }
        pthread_join(thread[0], NULL);
    }
    return 0;
}

/**
 NÃO ROLA
 */
void *add(void * arg) {
    sum_args *args = malloc(sizeof (sum_args));
    args = (sum_args *) arg;
    int med = args->nb;
    long ind = args->index;
    char rnd;
    int i;
    printf("ADD THREAD: %lu , START: %lu  ,  \n", ind / med, ind);
    for (i = 0; i < med; i++) {
        rnd = get_rand();
        list[ind + i] = rnd;
    }
    printf("ADD END DEBUG\n");
    return args;
}

/**
 * rand number between -100 and 100
 * @return  -100 <int <100
 */
char get_rand() {
    char rnd = rand() %200 - 100;
    return rnd;
}

/**
 * se a divisão de numero pelo numero de numeros a serem lidos, 
 * não for divisível pelo numeor de threads, algum thread terá que ler mais
 * numeros que os outros
 * 
 * criar as k threads
 * 
 * @param n, quantidade de numeros total
 * @param k, quantidade de threads
 * @return 
 */
int manage_threads(long *n, int *k) {
    long med = *n / (*k);
    int rest = *n - (med * *k);
    printf("DEBUG");
    pthread_t *thread = malloc(sizeof (pthread_t) * *k);
    for (int i = 0; i < *k; i++) {
        sum_args *args = malloc(sizeof (sum_args));
        args->index = med*i;
        args->nb = med;
        if (pthread_create(&thread[i], NULL, &sum, (void *) args)) { // 3 arg = func to exec 4 arg = arg passed to func
            printf("Error while creating threds. \n");
            exit(EXIT_FAILURE);
        }
        //        free(args);

    }
    for (int i = 0; i < *k; i++) {
        pthread_join(thread[i], NULL);
    }



    pthread_t *lon_th = malloc(sizeof (pthread_t));
    // only enter the loop if rest>0
    if (rest) {
        sum_args * args = malloc(sizeof (sum_args));
        args->index = *k*med;
        args->nb = rest;
        if (pthread_create(&lon_th[0], NULL, &sum, (void *) args)) { // 3 arg = func to exec 4 arg = arg passed to func
            printf("Error while creating threds. \n");
            exit(EXIT_FAILURE);
        }
        pthread_join(thread[0], NULL);
    }
    return 0;
}

/**
 * bloqueia o acc para leitura 
 * realiza a soma dos numeros lidos 
 * adicona dentro do acc
 * libera o lock
 * @param nb, how many numbers have to read
 * @param index, where to start reading the vector
 * @return 
 */
void *sum(void * arg) {
    sum_args *args = malloc(sizeof (sum_args));
    args = (sum_args *) arg;
    int med = args->nb;
    long ind = args->index;
    double *sum = malloc(sizeof (double));
    *sum = 0;
    int i;
    for (i = 0; i < med; i++) {
        printf("Thread: %lu , VAL: %d\n", ind / med,list[ind+i]);
        *sum += list[ind + i];
    }
    acquire(sum_lock);
    *acc += *sum;
    release(sum_lock);
    return args;
}

/**
 * 
 * @param n, lista com os valores de numeros 
 * @param k, lista com valores de nb de threads
 * @return 
 */
int executor(long *n, int *k) {
    fill_vector(n);
    manage_threads(n, k);
    return 0;
}

/**
 * use it to get the time of the begining of the executation
 * "                           " end
 * 
 * @return the time when called
 */
double clk_calc_time(clk * clk) {
    double t = (double) (clk->end - clk->beg) / CLOCKS_PER_SEC;

    return t;
}

int print_exe(double n[3][13]) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 13; j++) {
            printf("%f ", n[i][j]);
        }
        printf("\n");
    }
    return 0;
}

int initialize(long *n) {
    srand(time(NULL));
    list = (char *) malloc(*n * sizeof (char));
    acc = malloc(sizeof (int));
    *acc = 0;
    sum_lock = malloc(sizeof (char));
    *sum_lock = 0;
    lock_index = malloc(sizeof (char));
    *lock_index = 0;
    return 0;
}

/**
 * run_the execution for all the combination of N and K
 * where N eq size o the array
 * K eq number os threads
 * @return 0, if no error
 */
int run_all() {
    // start clock
    clk * clo = malloc(sizeof (clk));
    clo->beg = 0;
    clo->end = 0;
    int nb_thread[13] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 15, 20, 30};
    long list_size[3] = {N8, N9, N10};
    double ex[3][13]; //sizelist/threads/
    int size;
    int nb_t;
    double sum_time;
    int iter;
    int times = 10;
    long n;
    int k;
    for (size = 2; size < 3; size++) {
        printf("\nSIZE OF LIST\n: %lu", list_size[size]);
        initialize(&list_size[size]);
        for (nb_t = 0; nb_t < 13; nb_t++) {
            //set beg clock
            set_beg_time(clo);
            printf("NB OF THREADS: %d\n", nb_thread[nb_t]);
            sum_time = 0;
            iter = 0;
            while (iter < times) {
                printf("ITER: %d\n", iter);
                n = list_size[size];
                k = nb_thread[nb_t];
                executor(&n, &k);
                set_end_time(clo);
                sum_time += clk_calc_time(clo);
                iter++;
            }
            ex[size][nb_t] = sum_time / times;
            print_time(sum_time / times);
        }
        free_some();
        break;
    }
    print_exe(ex);

    return 0;
}

int free_some() {
    free(list);
    free(acc);
    return 0;
}

/*
 * que estrutura utilizar para botar os numeros (10^8-10^10) ||||||||| char list[N ]
 * alocar 1 byte por numero = N bytes ||||||||| utilizar char
 * considerar como 
 * TODO criar acc entre as threads de forma limpa 
 * type of the lock (int, string...)?? ||||||||| char 0 | 1
 * spinlock busy wait
 * 10x para cada combinação para ter o tempo medio
 * fazer grafico tempo_medio_exec = f(N)
 * fazer o vector ficar accessível pra todas as funções ou passar como argumento ||||||||| static
 * como passar o index a partir do qual cada thread deve começar a ler ||||||||| usar valor medio 
 * bota os resultados de tempo de execução dentro da struct dedicada
 * 
 */
int main(int argc, char** argv) {
    printf("1st arg:  how many numbers 2nd arg: number of threads\n");
    if (argc != 3) {
        printf("wrong number of arguments [2 args needed] \n");
        return -1;
    }
    //    mt_state *st = malloc(sizeof (mt_state));
    //    uint32_t *seed = malloc(sizeof (uint32_t));
    //    mts_seed32(st, *seed);
    //    srand(time(NULL));
    //    int u = mt_seed() % 200 -100;
    //    printf("aaaaa %d ", u);
    //    mts_seed32new(&st, seed);
    //
    //    int k = 10;
    //    long q = 1000000000;
    //    clk * clo = malloc(sizeof (clk));
    //    clk * clo2 = malloc(sizeof (clk));
    //    clo->beg = 0;
    //    clo->end = 0;
    //    clo2->beg = 0;
    //    clo2->end = 0;
    //    printf("DEBUG1");
    //    initialize(&q);
    //    //    set_beg_time(clo);
    //    //    printf("DEBUG2");
    //    //    fill_vector(&q);
    //    //    printf("DEBUG3");
    //    //    set_end_time(clo);
    //    //    print_time(clk_calc_time(clo));
    //
    //    printf("DEBUG4");
    //
    //    set_beg_time(clo2);
    //    fill_with_threads(&q, &k);
    //    printf("DEBUG5");
    //    set_end_time(clo2);
    //    print_time(clk_calc_time(clo2));

    run_all();
    return (EXIT_SUCCESS);
}
//