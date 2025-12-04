#include "cfg.h"
#include "parser_gen.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


static int init_rhs[1];
int number_of_state;

void initialize(int number_of_symb, int start_symb)
{
    init_rhs[0] = start_symb;
    grammar[MAX_NUMBER_OF_PROD] = (struct prod){.l = MAX_NUMBER_OF_SYMB, .r = &init_rhs[0], .len = 1};
    memset(is_ok_first, 0, sizeof(is_ok_first));
    memset(is_ok_follow, 0, sizeof(is_ok_follow));
    calc_terminal();
    for(int i = 0; i < number_of_symb; ++ i)
        calc_first(i);
    calc_follow();
}

void calc_terminal()
{
    for(int i = 0; i < number_of_symb; i++)is_terminal[i] = 1;
    for(int i = 0; i < number_of_prod; i++)
    {
        struct prod *p = &grammar[i];
        is_terminal[p->l] = 0;
    }
    is_terminal[EOF_SYMBOL_ID] = 1;
}

void calc_first(int symb)
{
    if (is_ok_first[symb])
        return;
    vis_first[symb] = 1;
    if (is_terminal[symb])
    {
        first[symb][symb] = 1;
        return;
    }
    for(int i = 0; i < number_of_prod; i++)
    {
        struct prod *p = &grammar[i];
        if (p->l == symb)
        {
            for(int j = 0; j < p->len; j++)
            {
                int next_symb = p->r[j];
                if(!vis_first[next_symb])
                    calc_first(next_symb);
                for(int k = 0; k < number_of_symb; k++)
                {
                    if (first[next_symb][k])
                        first[symb][k] = 1;
                }
                break;
            }
        }
    }
    is_ok_first[symb] = 1;
}

void calc_follow()
{
    follow[grammar[MAX_NUMBER_OF_PROD].r[0]][EOF_SYMBOL_ID] = 1;
    int changed = 1;
    while (changed)
    {
        changed = 0;
        for (int i = 0; i < number_of_prod; i++)
        {
            struct prod *p = &grammar[i];
            int left = p->l;
            for (int j = 0; j < p->len; j++)
            {
                int U = p->r[j];
                if (j < p->len - 1)
                {
                    int V = p->r[j + 1];
                    for (int k = 0; k < number_of_symb; k++)
                    {
                        if (first[V][k] && !follow[U][k])
                        {
                            follow[U][k] = 1;
                            changed = 1;
                        }
                    }
                }
                if (j == p->len - 1)
                {
                    for (int k = 0; k < number_of_symb; k++)
                    {
                        if (follow[left][k] && !follow[U][k])
                        {
                            follow[U][k] = 1;
                            changed = 1;
                        }
                    }
                    if (follow[left][EOF_SYMBOL_ID] && !follow[U][EOF_SYMBOL_ID])
                    {
                        follow[U][EOF_SYMBOL_ID] = 1;
                        changed = 1;
                    }
                }
            }
        }
    }
}

void display_trans()
{
    for(int i = 0; i < number_of_state; ++ i)
    {
        printf("State %d:\n", i);
        for(int j = 0; j < state_info[i].size; ++ j)
        {
            struct handler *h = &state_info[i].s[j];
            struct prod *p = &grammar[h->prod_id];

            printf(" [%d ->", p->l);
            for(int k = 0; k < p->len; ++ k)
            {
                if(k == h->dot_pos)printf(" .");
                printf(" %d", p->r[k]);
            }
            if(h->dot_pos == p->len)printf(" .");
            printf("]\n");
        }
        for(int j = 0; j < number_of_symb; ++ j)
        {
            if(trans[i][j].t != -1)
            {
                if(trans[i][j].t == 0)
                    printf("  On symbol %d: Shift/Goto to state %d\n", j, trans[i][j].id);
                else if(trans[i][j].t == 1)
                    printf("  On symbol %d: Reduce using production %d\n", j, trans[i][j].id);
            }
        }
        int a = EOF_SYMBOL_ID;
        if (trans[i][a].t != -1)
        {
            if (trans[i][a].t == 2)
            {
                printf("  On symbol $ (%d): ACCEPT\n", a);
            }
            else if (trans[i][a].t == 1)
            {
                printf("  On symbol $ (%d): Reduce using production %d\n", a, trans[i][a].id);
            }
        }
    }
}


static int state_equal(struct state *s1, struct state *s2)
{
    if (s1->size != s2->size)
    {
        return 0;
    }
    for (int i = 0; i < s1->size; i++)
    {
        if (s1->s[i].prod_id != s2->s[i].prod_id || s1->s[i].dot_pos != s2->s[i].dot_pos)
        {
            return 0;
        }
    }
    return 1;
}

static int compare_handlers(const void *a, const void *b)
{
    struct handler *h1 = (struct handler *)a;
    struct handler *h2 = (struct handler *)b;
    if (h1->prod_id != h2->prod_id)
    {
        return h1->prod_id - h2->prod_id;
    }
    return h1->dot_pos - h2->dot_pos;
}

void closure(struct state *s)
{
    int changed = 1;
    while (changed)
    {
        changed = 0;
        for (int i = 0; i < s->size; i++)
        {
            struct prod *p = &grammar[s->s[i].prod_id];
            int dot = s->s[i].dot_pos;

            if (dot < p->len)
            {
                int B = p->r[dot];
                if (!is_terminal[B])
                {
                    for (int j = 0; j < number_of_prod; j++)
                    {
                        if (grammar[j].l == B)
                        {
                            int found = 0;
                            for (int k = 0; k < s->size; k++)
                            {
                                if (s->s[k].prod_id == j && s->s[k].dot_pos == 0)
                                {
                                    found = 1;
                                    break;
                                }
                            }
                            if (!found)
                            {
                                if (s->size < MAX_NUMBER_OF_PROD + 1)
                                {
                                    s->s[s->size++] = (struct handler){ .prod_id = j, .dot_pos = 0 };
                                    changed = 1;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    qsort(s->s, s->size, sizeof(struct handler), compare_handlers);
}

void go_to(struct state *src, int symbol, struct state *dst)
{
    struct handler next_handlers[MAX_NUMBER_OF_PROD + 1];
    dst->size = 0;

    for (int i = 0; i < src->size; i++)
    {
        struct prod *p = &grammar[src->s[i].prod_id];
        int dot = src->s[i].dot_pos;

        if (dot < p->len && p->r[dot] == symbol)
        {
            next_handlers[dst->size++] = (struct handler){ .prod_id = src->s[i].prod_id, .dot_pos = dot + 1 };
        }
    }

    if (dst->size == 0)
    {
        dst->s = NULL;
        return;
    }

    dst->s = next_handlers;
    closure(dst);

    struct handler *final_handlers = (struct handler *)malloc(dst->size * sizeof(struct handler));
    if (final_handlers == NULL)
    {
        fprintf(stderr, "Memory allocation failed in go_to.\n");
        exit(1);
    }
    memcpy(final_handlers, dst->s, dst->size * sizeof(struct handler));
    dst->s = final_handlers;
}

int find_state(struct state *s)
{
    for (int i = 0; i < number_of_state; i++)
    {
        if (state_equal(&state_info[i], s))
        {
            free(s->s);
            return i;
        }
    }

    if (number_of_state >= MAX_NUMBER_OF_STATE)
    {
        fprintf(stderr, "Maximum number of states reached.\n");
        display_trans();
        exit(1);
    }
    state_info[number_of_state] = *s;
    return number_of_state++;
}


void calc_trans()
{
    number_of_state = 0;
    for (int i = 0; i < MAX_NUMBER_OF_STATE; i++)
    {
        for (int j = 0; j < MAX_NUMBER_OF_SYMB; j++)
        {
            trans[i][j] = (struct trans_result){ .t = -1, .id = -1 };
        }
        trans[i][EOF_SYMBOL_ID] = (struct trans_result){ .t = -1, .id = -1 };
    }

    struct state initial_state = { .size = 1 };
    initial_state.s = (struct handler *)malloc(sizeof(struct handler) * (MAX_NUMBER_OF_PROD + 1));
    if (initial_state.s == NULL)
    {
        fprintf(stderr, "Memory allocation failed for initial state.\n");
        exit(1);
    }
    initial_state.s[0] = (struct handler){ .prod_id = MAX_NUMBER_OF_PROD, .dot_pos = 0 };
    closure(&initial_state);

    state_info[number_of_state++] = initial_state;
    int current_state_index = 0;
    while (current_state_index < number_of_state)
    {
        struct state *I = &state_info[current_state_index];

        for (int i = 0; i < I->size; i++)
        {
            struct handler *h = &I->s[i];
            struct prod *p = &grammar[h->prod_id];

            if (h->dot_pos == p->len)
            {
                if (h->prod_id == MAX_NUMBER_OF_PROD)
                {
                    int a = EOF_SYMBOL_ID;
                    if (trans[current_state_index][a].t != -1)
                    {
                        fprintf(stderr, "Conflict detected in state %d on EOF symbol %d (ACCEPT vs %d/%d).\n",
                                current_state_index, a, trans[current_state_index][a].t, trans[current_state_index][a].id);
                        display_trans();
                        exit(1);
                    }
                    trans[current_state_index][a] = (struct trans_result){ .t = 2, .id = 0 };
                }

                else
                {
                    for (int a = 0; a <= EOF_SYMBOL_ID; a++)
                    {
                        int is_valid_terminal = 0;
                        if (a < number_of_symb && is_terminal[a]) {
                            is_valid_terminal = 1;
                        } else if (a == EOF_SYMBOL_ID) {
                            is_valid_terminal = 1;
                        }
                        if (is_valid_terminal && follow[p->l][a])
                        {
                            if (trans[current_state_index][a].t != -1)
                            {
                                fprintf(stderr, "Conflict detected in state %d on symbol %d (Reduce %d vs %d/%d).\n",
                                        current_state_index, a, h->prod_id, trans[current_state_index][a].t, trans[current_state_index][a].id);
                                display_trans();
                                exit(1);
                            }
                            trans[current_state_index][a] = (struct trans_result){ .t = 1, .id = h->prod_id };
                        }
                    }
                }
            }
        }
        for (int X = 0; X <= number_of_symb; X++)
        {
            struct state J;
            J.s = NULL;
            J.size = 0;

            struct handler temp_handlers[MAX_NUMBER_OF_PROD + 1];
            J.s = temp_handlers;
            go_to(I, X, &J);

            if (J.size > 0)
            {
                int next_state_id = find_state(&J);
                if (is_terminal[X])
                {
                    if (trans[current_state_index][X].t != -1)
                    {
                         fprintf(stderr, "Shift-Reduce conflict on symbol %d (State %d).\n", X, current_state_index);
                         display_trans();
                         exit(1);
                    }
                    trans[current_state_index][X] = (struct trans_result){ .t = 0, .id = next_state_id };
                }
                else
                {
                    trans[current_state_index][X] = (struct trans_result){ .t = 0, .id = next_state_id };
                }
            }
        }
        current_state_index++;
    }
}