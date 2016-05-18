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

#define MAX = 100;
#define MIN = -100;
#define N8 100000000
#define N9 1000000000
#define N10 10000000000 // must use long!!!




char * list; // list of random number 
int *acc; // sum of the numbers in the list
char* sum_lock;
char* lock_index;
long * med;

//typedef struct clk {
//    clock_t beg;
//    clock_t end;
//};

//typedef struct exec {
//    int time;
//    int qt_nb;
//    int nb_threads;
//};
//typedef clk clk;
//typedef clk clk;

int set_beg_time(clk *clk) {
    clk->beg = clock();
    return 0;
}

int set_end_time(clk *clk) {
    clk->end = clock();
    return 0;
}

int spinlock() {
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
char * format_time(int diff) {
    char str[20];
    sprintf(str, "%d", diff);
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

void print_time(clk *clk) {
    int tx = clk_calc_time(clk);
    char* res = format_time(tx);
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

///**
// * Responsible for puting rand number in the lista
// * test and_set for preventing concurrent actons
// * @return 
// */
//int put_numb(int qt) {
//    while (qt--) {
//        get_rand();
//    }
//    return 0;
//}

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
 * rand number between -100 and 100
 * @return  -100 <int <100
 */
int get_rand() {
    int rnd = (rand() % 200) - 100;
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
int gerenciador(long *n, int *k) {
    int rest = *n - (*med * *k);
    pthread_t *thread = malloc(sizeof (pthread_t) * *k);

    for (int i = 0; i < *k; i++) {
        long* ind = malloc(sizeof (long));
        *ind = (*med) * i;
        printf("THREAD: %d ", i);
        if (pthread_create(&thread[i], NULL, &somador, (void *) ind)) { // 3 arg = func to exec 4 arg = arg passed to func
            printf("Error while creating threds. \n");
            exit(EXIT_FAILURE);
        }
        printf("\n\nAFTER THREAD: %d\n\n ", i);
        
    }

    if (rest) {
        //        manage_rest();
    }
    //
    //    int rest = 0;
    //    int index = 0;
    //    while (n % k) {
    //        rest++;
    //        n--;
    //    }
    //    //cria os threads passando o index de onde deve começar a ler.
    //    if (rest) {
    //        somador(med + rest, index);
    //        rest--;
    //    } else {
    //        somador(med, index);
    //    }
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

void *somador(void * arg) {
    long * ind = (long *) arg;
    double *sum = malloc(sizeof (double));
    *sum = 0;
    for (int i = 0; i < *med; i++) {
        *sum += list[*ind + i];
    }
    //    printf("\n  SOMADOR SUM: %d  \n", *sum);
    acquire(sum_lock);
    *acc += *sum;
    printf("\n\nACC: %d \n\n", *acc); //segmentation fault
    release(sum_lock);

    printf("\n\n nTHREAD : %lu  LEAVING MODAFUCKA\n\n", *ind / (*med)); //segmentation fault
    return ind;
}

/**
 * empty the vector
 * @return 
 */
int empty_vector() {
    return 0;
}

/**
 * 
 * @param n, lista com os valores de numeros 
 * @param k, lista com valores de nb de threads
 * @return 
 */
int executor(long *n, int *k, clk * clo) {
    // for i in lista k
    /////for j in lista numeros
    //    for (int j = 0; j < n; j++) {

    printf("before fill vector\n");
    fill_vector(n);
    printf("after fill vector\n");
    //    int beg_time = get_time();

    set_beg_time(clo);
    printf("before gerenciador\n\n BEGclo: %lu\n", (long) clo->beg);
    gerenciador(n, k);
    printf("after gerenciador\n");
    set_end_time(clo);
    printf("after set end time \n\nENDclo: %lu\n", (long) clo->beg);
    print_time(clo);
    printf("after print time\n");
    printf("ACC %d\n", *acc);
    //    int end_time = get_time();
    //    int time = calculate_time(beg_time, end_time);

    return 0;
}

/**
 * use it to get the time of the begining of the executation
 * "                           " end
 * 
 * @return the time when called
 */
int clk_calc_time(clk * clk) {
    double t = (double) (clk->end - clk->beg) / CLOCKS_PER_SEC;

    return t;
}

int test_fonctions() {

    return 0;
}

int allocate(long size) {
    printf("beg allocate ");

    return 0;
}

int initialize(int argc, char** argv) {
    long *n = malloc(sizeof (long));
    *n = atoi(argv[1]); // how many numbers
    int *k = malloc(sizeof (int));
    *k = atoi(argv[2]); // how many threads
    med = malloc(sizeof (long));
    *med = *n / *k;
    printf("EXECUTION WITH: \n %d threads and %lu numbers", *k, *n);
    srand(time(NULL));
    list = (char *) malloc(*n * sizeof (char));
    acc = malloc(sizeof (int));
    *acc = 0;
    sum_lock = malloc(sizeof (char));
    *sum_lock = 0;
    lock_index = malloc(sizeof (char));
    *lock_index = 0;
    clk * clo = malloc(sizeof (clk));
    clo->beg = 0;
    clo->end = 0;
    printf("1st arg:  how many numbers 2nd arg: number of threads\n");
    if (argc != 3) {
        printf("wrong number of arguments [2 args needed] \n");
        return -1;
    }
    printf("before set");
    printf("before allocate\n");
    //    allocate(n);
    printf("after allocate\n");

    executor(n, k, clo);
    //    printf("before fill vector\n");
    //    fill_vector(n);
    //    printf("after fill vector\n");

    printf("after printing time\n");
    //    executor(n, k);

    return 0;
}

void test_lock() {
    // fazer dois threads
    // fazer o lock
    // escrever com um thread
    // printar algo
    // fazer dormir
    // tentar escrever com o outro


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

    printf("args: %d\n", argc);
    initialize(argc, argv);
    free(list);
    return (EXIT_SUCCESS);
}
//