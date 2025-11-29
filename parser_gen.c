#include "cfg.h"
#include "parser_gen.h"
#include <stdio.h>
#include <string.h>

static int init_rhs[1];

void initialize(int number_of_symb, int start_symb)
{
    init_rhs[0] = start_symb;
    grammar[MAX_NUMBER_OF_PROD] = (struct prod){.l = MAX_NUMBER_OF_SYMB, .r = &init_rhs[0], .len = 1};
    memset(is_ok_first, 0, sizeof(is_ok_first));
    memset(is_ok_follow, 0, sizeof(is_ok_follow));
    for(int i = 0; i < number_of_symb; ++ i)
        calc_first(i);
    for(int i = 1; i < number_of_symb; ++ i)
        calc_follow(i);
}

void calc_first(int symb)
{
    if (is_ok_first[symb])
        return;
    vis_first[symb] = 1;
    int is_terminal = 1;
    for(int i = 0; i < number_of_prod; i++)
    {
        struct prod *p = &grammar[i];
        if (p->l == symb)
        {
            is_terminal = 0;
            break;
        }
    }
    if (is_terminal)
        first[symb][symb] = 1;
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

void calc_follow(int symb)
{
    if (is_ok_follow[symb])
        return;
    vis_follow[symb] = 1;
    for(int i = 0; i < number_of_prod; i++)
    {
        struct prod *p = &grammar[i];
        for(int j = 0; j < p->len - 1; j++)
        {
            int U = p->r[j], V = p->r[j + 1];
            if (U == symb)
            {
                for(int k = 0; k < number_of_symb; k++)
                {
                    if (first[V][k])
                        follow[symb][k] = 1;
                }
            }
        }
        for(int j = 0; j < p->len; j++)
        {
            int U = p->r[j];
            if (U == symb)
            {
                if(!vis_follow[p->l])
                    calc_follow(p->l);
                for(int k = 0; k < number_of_symb; k++)
                {
                    if (follow[p->l][k])
                        follow[symb][k] = 1;
                }
            }
        }
    }
    is_ok_follow[symb] = 1;
}

int test_if_left_is_ok(int *p, int start, int end)
{
    int n = end - start + 1;
    if (n <= 0) return 0;

    struct handler cur_storage[MAX_NUMBER_OF_PROD + 1], nxt_storage[MAX_NUMBER_OF_PROD + 1];
    struct state cur = { .size = 0, .s = cur_storage };
    struct state nxt = { .size = 0, .s = nxt_storage };

    int vis_prod[MAX_NUMBER_OF_PROD + 1];
    int i, j;

    cur.s[cur.size++] = (struct handler){ .prod_id = MAX_NUMBER_OF_PROD, .dot_pos = 0 };

    int changed = 1;
    while (changed)
    {
        changed = 0;
        memset(vis_prod, 0, sizeof(vis_prod));

        for (i = 0; i < cur.size; i++)
        {
            struct prod *prd = &grammar[cur.s[i].prod_id];
            int dot = cur.s[i].dot_pos;

            if (dot < prd->len)
            {
                int sym = prd->r[dot];
                for (j = 0; j < number_of_prod; j++)
                {
                    if (grammar[j].l == sym && !vis_prod[j])
                    {
                        vis_prod[j] = 1;
                        int k, found = 0;
                        for (k = 0; k < cur.size; k++) {
                            if (cur.s[k].prod_id == j && cur.s[k].dot_pos == 0) {
                                found = 1; break;
                            }
                        }
                        if (!found) {
                            cur.s[cur.size++] = (struct handler){ .prod_id = j, .dot_pos = 0 };
                            changed = 1;
                        }
                    }
                }

            }
        }
    }

    for(int pos = 0; pos < n; pos++)
    {
        int a = p[start + pos];
        nxt.size = 0;

        for (i = 0; i < cur.size; i++) {
            struct prod *prd = &grammar[cur.s[i].prod_id];
            int dot = cur.s[i].dot_pos;

            if (dot < prd->len && prd->r[dot] == a) {
                nxt.s[nxt.size++] = (struct handler){ .prod_id = cur.s[i].prod_id, .dot_pos = dot + 1 };
            }
        }

        changed = 1;
        while (changed) {
            changed = 0;
            memset(vis_prod, 0, sizeof(vis_prod));

            for (i = 0; i < nxt.size; i++) {
                struct prod *prd = &grammar[nxt.s[i].prod_id];
                int dot = nxt.s[i].dot_pos;

                if (dot < prd->len) {
                    int sym = prd->r[dot];
                    for (j = 0; j < number_of_prod; j++)
                    {
                        if (grammar[j].l == sym && !vis_prod[j])
                        {
                            vis_prod[j] = 1;
                            int k, found = 0;
                            for (k = 0; k < nxt.size; k++) {
                                if (nxt.s[k].prod_id == j && nxt.s[k].dot_pos == 0) {
                                    found = 1; break;
                                }
                            }
                            if (!found) {
                                nxt.s[nxt.size++] = (struct handler){ .prod_id = j, .dot_pos = 0 };
                                changed = 1;
                            }
                        }
                    }
                }
            }
        }
        // for(i = 0; i < cur.size; ++ i)
        // {
        //     printf("%d -> ", grammar[cur.s[i].prod_id].l);
        //     for(j = 0; j < cur.s[i].dot_pos; ++ j)
        //         printf("%d ", grammar[cur.s[i].prod_id].r[j]);
        //     printf(".");
        //     for(j = cur.s[i].dot_pos; j < grammar[cur.s[i].prod_id].len; ++ j)
        //         printf(" %d", grammar[cur.s[i].prod_id].r[j]);
        //     printf("\n");
        // }
        // printf("\n");

        cur.size = nxt.size;
        for (i = 0; i < nxt.size; i++) cur.s[i] = nxt.s[i];

        if (cur.size == 0) return 0;
    }
    if(cur.size > 0)return 1;
    return 0;
}

// E F G ID + * ( )
// 0 1 2 3  4 5 6 7

void calc(int* expr, int size)
{
    int left[MAX_NUMBER_OF_STATE], top = 0;
    left[top ++] = expr[0];
    int pos = 1;
    int cnt = 0;
    printf("|");
    for(int i = 0; i < size; ++ i)
        printf("%d ", expr[i]);
    printf("\n");
    int grammar_error = 0;
    while(pos < size)
    {
        for(int i = 0; i < top; ++ i)
            printf("%d ", left[i]);
        printf("|");
        for(int i = pos; i < size; ++ i)
            printf("%d ", expr[i]);
        printf("\n");
        left[top ++] = expr[pos];
        int left_is_ok = test_if_left_is_ok(left, 0, top - 1);
        if (!left_is_ok)
        {
            top --;
            int finish = 0;
            for(int i = top - 1; i >= 0; -- i)
            {
                for(int j = 0; j < number_of_prod; ++ j)
                {
                    if(grammar[j].len != (top - i))continue;
                    int ok = 1;
                    for(int k = 0; k < grammar[j].len; ++ k)
                    {
                        if(grammar[j].r[k] != left[i + k])
                        {
                            ok = 0;
                            break;
                        }
                    }
                    if(ok)
                    {
                        int tmp = left[i];
                        left[i] = grammar[j].l;
                        if(test_if_left_is_ok(left, 0, i))
                        {
                            finish = 1;
                            top = i + 1;
                            break;
                        }
                        else
                        {
                            left[i] = tmp;
                        }
                    }
                }
                if(finish)break;
            }
            if(!finish)
            {
                printf("Grammar Error! Neither reduce nor shift is ok\n");
                return;
            }
        }
        else
        {
            top--;
            for(int i = top - 1; i >= 0; -- i)
            {
                for(int j = 0; j < number_of_prod; ++ j)
                {
                    if(grammar[j].len != (top - i))continue;
                    int ok = 1;
                    for(int k = 0; k < grammar[j].len; ++ k)
                    {
                        if(grammar[j].r[k] != left[i + k])
                        {
                            ok = 0;
                            break;
                        }
                    }
                    if(ok)
                    {
                        int tmp = left[i];
                        left[i] = grammar[j].l;
                        if(test_if_left_is_ok(left, 0, i))
                        {
                            int is_follow = 0;
                            for(int k = 0; k < number_of_symb; ++ k)
                                if(follow[left[i]][expr[pos]])is_follow = 1;
                            if(is_follow)
                            {
                                grammar_error = 1;
                                break;
                            }
                        }
                        left[i] = tmp;
                    }
                }
                if(grammar_error)break;
            }
            if(grammar_error)break;
            left[top ++] = expr[pos ++];
        }
    }
    if(grammar_error)
    {
        printf("Grammar Error! Both reduce and shift is ok\n");
        return;
    }
    for(int i = 0; i < top; ++ i)
        printf("%d ", left[i]);
    printf("|\n");
    int change = 1;
    while(change)
    {
        change = 0;
        for(int i = top - 1; i >= 0; -- i)
        {
            int finish = 0;
            for(int j = 0; j < number_of_prod; ++ j)
            {
                if(grammar[j].len != (top - i))continue;
                int ok = 1;
                for(int k = 0; k < grammar[j].len; ++ k)
                {
                    if(grammar[j].r[k] != left[i + k])
                    {
                        ok = 0;
                        break;
                    }
                }
                if(ok)
                {
                    int tmp = left[i];
                    left[i] = grammar[j].l;
                    if(test_if_left_is_ok(left, 0, i))
                    {
                        finish = 1;
                        change = 1;
                        top = i + 1;
                        break;
                    }
                    else
                    {
                        left[i] = tmp;
                    }
                }
            }
            if(finish)break;
        }
        if(change)
        {
            for(int i = 0; i < top; ++ i)
                printf("%d ", left[i]);
            printf("|\n");
        }
    }
    printf("%d|\n", MAX_NUMBER_OF_SYMB);
}