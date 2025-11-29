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

// E F + -> 0 1 2
// E -> E + F
// E -> F

int main()
{
    // number_of_symb = 3;
    // number_of_prod = 2;
    // grammar[0] = (struct prod){ .l = 0, .r = (int[]){0, 2, 1}, .len = 3 };
    // grammar[1] = (struct prod){ .l = 0, .r = (int[]){1}, .len = 1};
    number_of_symb = 8; // E F G ID + * ( )
                        // 0 1 2 3  4 5 6 7
    number_of_prod = 6;
    grammar[0] = (struct prod){ .l = 0, .r = (int[]){0, 4, 1}, .len = 3 }; // E -> E + F
    grammar[1] = (struct prod){ .l = 0, .r = (int[]){1}, .len = 1 };       // E -> F
    grammar[2] = (struct prod){ .l = 1, .r = (int[]){1, 5, 2}, .len = 3 }; // F -> F * G
    grammar[3] = (struct prod){ .l = 1, .r = (int[]){2}, .len = 1 };       // F -> G
    grammar[4] = (struct prod){ .l = 2, .r = (int[]){6, 0, 7}, .len = 3 }; // G -> ( E )
    grammar[5] = (struct prod){ .l = 2, .r = (int[]){3}, .len = 1 };       // G -> ID
    initialize(number_of_symb, 0);
    calc((int[]){6, 3, 4, 3, 7, 5, 3}, 7);

    // int res1 = test_if_left_is_ok((int[]){0,2,1,2}, 0, 3);
    // int res2 = test_if_left_is_ok((int[]){0, 2, 1}, 0, 2);
    // int res3 = test_if_left_is_ok((int[]){1}, 0, 0);
    // printf("%d %d %d\n", res1, res2, res3); // expect 0 1 1
    // printf("%d", test_if_left_is_ok((int[]){0, 4}, 0, 1));

    // int res4 = test_if_left_is_ok((int[]){0, 4, 1, 4}, 0, 3);
    // printf("%d\n", res4); // expect 1
    // for(int i = 0; i < number_of_symb; i++)
    // {
    //     printf("First(%d): ", i);
    //     for(int j = 0; j < number_of_symb; j++)
    //     {
    //         if (first[i][j])
    //             printf("%d ", j);
    //     }
    //     printf("\n");
    // }
    // for(int i = 0; i < number_of_symb; i++)
    // {
    //     printf("Follow(%d): ", i);
    //     for(int j = 0; j < number_of_symb; j++)
    //     {
    //         if (follow[i][j])
    //             printf("%d ", j);
    //     }
    //     printf("\n");
    // }
}