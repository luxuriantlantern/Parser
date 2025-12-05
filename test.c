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
struct trans_result trans[MAX_NUMBER_OF_STATE][MAX_NUMBER_OF_SYMB + 2];

int first[MAX_NUMBER_OF_SYMB][MAX_NUMBER_OF_SYMB];
int follow[MAX_NUMBER_OF_SYMB][MAX_NUMBER_OF_SYMB + 2];
int is_ok_first[MAX_NUMBER_OF_SYMB];
int is_ok_follow[MAX_NUMBER_OF_SYMB];
int vis_first[MAX_NUMBER_OF_SYMB];
int vis_follow[MAX_NUMBER_OF_SYMB];
int is_terminal[MAX_NUMBER_OF_SYMB + 2];

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


void calc_example(int* expr, int size)
{
    int left[10005];
    int symbol_top = 0;
    int state_stack[10005];
    int state_top = 0;
    int input_index = 0;

    state_stack[state_top++] = 0;

    while(1)
    {
        for(int i = 0; i < symbol_top; ++ i)
            printf("%d ", left[i]);
        printf("| ");
        for(int i = input_index; i < size; ++ i)
            printf("%d ", expr[i]);

        int current_state = state_stack[state_top - 1];
        int lookahead;

        if (input_index < size) {
            lookahead = expr[input_index];
        } else {
            lookahead = EOF_SYMBOL_ID;
        }

        struct trans_result current_action = trans[current_state][lookahead];

        if(current_action.t == 0)
        {
            if (lookahead == EOF_SYMBOL_ID) {
                break;
            }

            printf("Action: Shift to state %d\n", current_action.id);
            left[symbol_top ++] = lookahead;
            state_stack[state_top ++] = current_action.id;
            input_index ++;
        }
        else if (current_action.t == 1)
        {
            int prod_id = current_action.id;
            struct prod *p = &grammar[prod_id];
            int A = p->l;
            int len = p->len;

            printf("Action: Reduce using production %d (%d -> ", prod_id, A);
            for(int i = 0; i < len; ++ i)
                printf("%d ", p->r[i]);
            printf(")\n");

            symbol_top -= len;
            state_top -= len;

            left[symbol_top ++] = A;

            int new_stack_top_state = state_stack[state_top - 1];
            struct trans_result goto_res = trans[new_stack_top_state][A];

            state_stack[state_top ++] = goto_res.id;
        }
        else if (current_action.t == 2)
        {
            printf("Action: ACCEPT\n");
            return;
        }
        else
        {
            printf("Action: ERROR (State %d, Symbol %d)\n", current_state, lookahead);
            break;
        }
    }
}

void test_calc_1()
{
    initialize_grammar1();
    calc_trans();
    display_trans();
    printf("\nExample Calculation1(OK):\n");
    calc_example((int[]){1, 2, 1, 2, 1}, 5);
    printf("\nExample Calculation2(Error):\n");
    calc_example((int[]){1, 2, 2}, 3);
}

void test_calc_2()
{
    initialize_grammar2();
    calc_trans();
    display_trans();
    printf("\nExample Calculation1(OK):\n");
    calc_example((int[]){6, 3, 4, 3, 7, 5, 3}, 7);
    printf("\nExample Calculation2(Error):\n");
    calc_example((int[]){3, 4, 3, 6}, 4);
}

void test_calc_3()
{
    initialize_grammar3();
    calc_trans();
    display_trans();
}

void test_calc_4()
{
    initialize_grammar4();
    calc_trans();
    display_trans();
    printf("\nExample Calculation1(OK):\n");
    calc_example((int[]){3, 5, 3, 4, 3, 4, 3, 5, 3}, 9);
    printf("\nExample Calculation2(Error):\n");
    calc_example((int[]){3, 5, 3, 4}, 4);
}

void display_follow()
{
    for(int i = 0; i < number_of_symb; ++ i)
    {
        printf("Follow(%d): ", i);
        for(int j = 0; j < number_of_symb + 2; ++ j)
        {
            if(follow[i][j])
                printf("%d ", j);
        }
        if(follow[i][EOF_SYMBOL_ID])
            printf("$");
        printf("\n");
    }
}

int main()
{
    test_calc_1();
}