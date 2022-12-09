#include <stdio.h>

/* swap: swap the values of v[i] and v[j] */
void swap(int v[], int i, int j)
{
    int temp = v[i]; v[i] = v[j]; v[j] = temp;
}


void qsort(int v[], int left, int right)
{
    int i, last;
    if (left >= right) /* base case: if the group is too small, stop */
        return;

    /* others: pick e as the middle-indexed # in the current group.
    Partition the group into A and B such that (all elms in A) <= e and
    (all elms in B) > e */
    swap(v, left, (left + right) / 2);

    last = left;

    for (i = left + 1; i <= right; i++)
        if (v[i] <= v[left])
            swap(v, ++last, i);

    swap(v, left, last);
    
    qsort(v, left, last-1); /* sort group A [left..last-1] */
    qsort(v, last+1, right); /* sort group B [last+1..right]*/
}

int main(void)
{
    int v[] = {6, 1, 4, 3, 5, 9, 10, 2, 7, 8};
    qsort(v, 0, 9);
    
    for (int i = 0; i < 10; i++)
        printf("%d ", v[i]);
}