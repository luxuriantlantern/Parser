//
// Created by 18067 on 25-11-26.
//

#ifndef PARSER_GEN_H
#define PARSER_GEN_H

#include "cfg.h"

extern void initialize(int number_of_symb, int start_symb);
extern void calc_first(int symb);
extern void calc_follow(int symb);
extern int test_if_left_is_ok(int *p, int start, int end);
extern void calc(int* expr, int size);

/* inputs: */
extern int number_of_symb; /* number of symbols */
extern int number_of_prod; /* number of productions */
extern struct prod grammar[MAX_NUMBER_OF_PROD + 1];

/* outputs: */
extern struct state state_info[MAX_NUMBER_OF_STATE];
extern struct trans_result trans[MAX_NUMBER_OF_STATE][MAX_NUMBER_OF_SYMB];

extern int first[MAX_NUMBER_OF_SYMB][MAX_NUMBER_OF_SYMB];
extern int follow[MAX_NUMBER_OF_SYMB][MAX_NUMBER_OF_SYMB];
extern int is_ok_first[MAX_NUMBER_OF_SYMB];
extern int is_ok_follow[MAX_NUMBER_OF_SYMB];
extern int vis_first[MAX_NUMBER_OF_SYMB];
extern int vis_follow[MAX_NUMBER_OF_SYMB];


#endif //PARSER_GEN_H