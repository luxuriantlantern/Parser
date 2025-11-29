//
// Created by 18067 on 25-11-26.
//

#include "cfg.h"
#include "parser_gen.h"
#include <stdio.h>

/* inputs: */
int number_of_symb; /* number of symbols */
int number_of_prod; /* number of productions */
struct prod grammar[MAX_NUMBER_OF_PROD + 1];

/* outputs: */
struct state state_info[MAX_NUMBER_OF_STATE];
struct trans_result trans[MAX_NUMBER_OF_STATE][MAX_NUMBER_OF_SYMB];

int first[MAX_NUMBER_OF_SYMB][MAX_NUMBER_OF_SYMB];
int follow[MAX_NUMBER_OF_SYMB][MAX_NUMBER_OF_SYMB];
int is_ok_first[MAX_NUMBER_OF_SYMB];
int is_ok_follow[MAX_NUMBER_OF_SYMB];
int vis_first[MAX_NUMBER_OF_SYMB];
int vis_follow[MAX_NUMBER_OF_SYMB];
int is_terminal[MAX_NUMBER_OF_SYMB];

static int init_rhs[MAX_NUMBER_OF_PROD][MAX_NUMBER_OF_STATE];

void initialize_grammar1()
{
    number_of_symb = 3;
    // E F +
    // 0 1 2
    number_of_prod = 2;
    init_rhs[0][0] = 0; init_rhs[0][1] = 2; init_rhs[0][2] = 1;
    init_rhs[1][0] = 1;
    grammar[0] = (struct prod){ .l = 0, .r = init_rhs[0], .len = 3 }; // E -> E + F
    grammar[1] = (struct prod){ .l = 0, .r = init_rhs[1], .len = 1}; // E -> F
    initialize(number_of_symb, 0);
}

void initialize_grammar2()
{
    number_of_symb = 8;
    // E F G ID + * ( )
    // 0 1 2 3  4 5 6 7
    number_of_prod = 6;
    init_rhs[0][0] = 0; init_rhs[0][1] = 4; init_rhs[0][2] = 1;
    init_rhs[1][0] = 1;
    init_rhs[2][0] = 1; init_rhs[2][1] = 5; init_rhs[2][2] = 2;
    init_rhs[3][0] = 2;
    init_rhs[4][0] = 6; init_rhs[4][1] = 0; init_rhs[4][2] = 7;
    init_rhs[5][0] = 3;
    grammar[0] = (struct prod){ .l = 0, .r = init_rhs[0], .len = 3 }; // E -> E + F
    grammar[1] = (struct prod){ .l = 0, .r = init_rhs[1], .len = 1 };       // E -> F
    grammar[2] = (struct prod){ .l = 1, .r = init_rhs[2], .len = 3 }; // F -> F * G
    grammar[3] = (struct prod){ .l = 1, .r = init_rhs[3], .len = 1 };       // F -> G
    grammar[4] = (struct prod){ .l = 2, .r = init_rhs[4], .len = 3 }; // G -> ( E )
    grammar[5] = (struct prod){ .l = 2, .r = init_rhs[5], .len = 1 };       // G -> ID
    initialize(number_of_symb, 0);
}

void initialize_grammar3()
{
    number_of_symb = 4;
    // E ID + *
    // 0 1 2 3
    number_of_prod = 3;
    init_rhs[0][0] = 0; init_rhs[0][1] = 2; init_rhs[0][2] = 0;
    init_rhs[1][0] = 0; init_rhs[1][1] = 3; init_rhs[1][2] = 0;
    init_rhs[2][0] = 1;
    grammar[0] = (struct prod){ .l = 0, .r = init_rhs[0], .len = 3 }; // E -> E + E
    grammar[1] = (struct prod){ .l = 0, .r = init_rhs[1], .len = 3 }; // E -> E * E
    grammar[2] = (struct prod){ .l = 0, .r = init_rhs[2], .len = 1 }; // E -> ID
    initialize(number_of_symb, 0);
}

void initialize_grammar4()
{
    number_of_symb = 6;
    // L E F ID , +
    // 0 1 2 3  4 5
    number_of_prod = 5;
    init_rhs[0][0] = 1;
    init_rhs[1][0] = 0; init_rhs[1][1] = 4; init_rhs[1][2] = 1;
    init_rhs[2][0] = 1; init_rhs[2][1] = 5; init_rhs[2][2] = 2;
    init_rhs[3][0] = 2;
    init_rhs[4][0] = 3;
    grammar[0] = (struct prod){ .l = 0, .r = init_rhs[0], .len = 1 }; // L -> E
    grammar[1] = (struct prod){ .l = 0, .r = init_rhs[1], .len = 3 }; // L -> L , E
    grammar[2] = (struct prod){ .l = 1, .r = init_rhs[2], .len = 3 }; // E -> E + F
    grammar[3] = (struct prod){ .l = 1, .r = init_rhs[3], .len = 1 }; // E -> F
    grammar[4] = (struct prod){ .l = 2, .r = init_rhs[4], .len = 1 }; // F -> ID
    initialize(number_of_symb, 0);
}

void test_calc_first_and_follow()
{
    for(int i = 0; i < number_of_symb; i++)
    {
        printf("First(%d): ", i);
        for(int j = 0; j < number_of_symb; j++)
        {
            if (first[i][j])
                printf("%d ", j);
        }
        printf("\n");
    }
    for(int i = 0; i < number_of_symb; i++)
    {
        if(is_terminal[i])continue;
        printf("Follow(%d): ", i);
        for(int j = 0; j < number_of_symb; j++)
        {
            if (follow[i][j])
                printf("%d ", j);
        }
        printf("\n");
    }
}

void test_test_if_left_is_ok()
{
    initialize_grammar1();
    int res1 = test_if_left_is_ok((int[]){0,2,1,2}, 0, 3);
    int res2 = test_if_left_is_ok((int[]){0, 2, 1}, 0, 2);
    int res3 = test_if_left_is_ok((int[]){1}, 0, 0);
    printf("%d %d %d\n", res1, res2, res3); // expect 0 1 1
}

void test_calc_1()
{
    initialize_grammar1();
    test_calc_first_and_follow();
    calc((int[]){1, 2, 1, 2, 1}, 5);
    // F + F + F
}

void test_calc_2()
{
    initialize_grammar2();
    // calc((int[]){6, 3, 4, 3, 7, 5, 3}, 7);
    // calc((int[]){3, 4, 6, 3, 5, 3, 7}, 7);
    calc((int[]){3, 5, 6, 3, 4, 3, 7}, 7);
}

void test_calc_3()
{
    initialize_grammar3();
    test_calc_first_and_follow();
    calc((int[]){1, 2, 1, 3, 1}, 5);
}

void test_calc_4()
{
    initialize_grammar4();
    calc((int[]){3,5,3,4,3,4,3,5,3}, 9);
}

void test_calc_5()
{
    initialize_grammar1();
    calc((int[]){1, 2}, 2);
}

int main()
{
    test_calc_5();
}