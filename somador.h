/* 
 * File:   somador.h
 * Author: Jean-Loïc Mugnier <mugnier at polytech.unice.fr>
 *
 * Created on May 14, 2016, 6:28 PM
 */

#ifndef SOMADOR_H
#define	SOMADOR_H

typedef struct Clk {
    clock_t beg;
    clock_t end;
} clk;

typedef struct Sum_args {
    int nb;
    int index;
} sum_args;

typedef struct Exec {
    int time;
    int qt_nb;
    int nb_threads;
} exe;

int set_beg_time(clk *clk);
int set_end_time(clk *clk);
int spinlock();
void acquire(char* lock);
char * format_time(int diff);
void print_time(clk *clk);
void release(char* lock);
int test_and_set2(char* flag);
int put_numb(int qt);
int fill_vector(long *n);
int get_rand();
int manage_threads(long *n, int *k);
void *sum(void* arg);
int empty_vector();
int executor(long *n, int *k, clk* clo);
int clk_calc_time(clk *clk);
int test_fonctions();
char* concat(char *s1, char *s2);

#endif	/* SOMADOR_H */

