#include <stdio.h>
#include <stdlib.h>

int main ()
{
	int arr[10] = { -1, 50, 100, 4, 8, 1, 5, 0, 7, 5};

	int i,temp,r,j= 0;

	for(i = 0 ; i < 10 ; i ++) {
		printf("%d, ",arr[i]);
	}
	printf("\n");
	for(i = 0 ; i < 10 ; i ++) {
		r = i;
		for( j = i; j < 10; j ++) {
			if (!(arr[r] <= arr[j]))
				r = j;
		}
		temp = arr[i];
		arr[i] = arr[r];
		arr[r] = temp;
	}
	for(i = 0 ; i < 10 ; i ++) {
		printf("%d, ",arr[i]);
	}
	printf("\n");
#if 0
	for(i = 0 ; i < 10 ; i ++) {
		printf("%d, ",srr[i]);
	}
	printf("\n");
#endif
	return 0;
}
