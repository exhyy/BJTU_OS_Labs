#include "utils.h"
#include <stdlib.h>
#include <stdio.h>

int cmp(const void *p1, const void *p2)
{
    const Pair *a = (const Pair *)p1;
    const Pair *b = (const Pair *)p2;
    return a->first > b->first;
}

int main()
{
    int cnt = 4;
    Pair id_pairs[4];
    id_pairs[0].first = 3;
    id_pairs[0].second = 1;

    id_pairs[1].first = 1;
    id_pairs[1].second = 4;

    id_pairs[2].first = 21;
    id_pairs[2].second = 7;

    id_pairs[3].first = 10;
    id_pairs[3].second = 10;

    qsort(id_pairs, cnt, sizeof(Pair), cmp);

    for (int i = 0; i < cnt; i++)
    {
        printf("%d %d\n", id_pairs[i].first, id_pairs[i].second);
    }
    printf("%-10p%-10d\n", (void *)123456, 999);
    fprintf(stdout, "%-12s %-12s %-12s %-12s\n", "abc", "123", "hello", "ca");
    return 0;
}