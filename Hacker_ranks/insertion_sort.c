#include <stdio.h>
#include <stdlib.h>
/* Insertion Sort */
int main ()
{
		int arr[10] = { -1, 50, 100, 4, 8, 1, 5, 0, 7, 5};

		int i,temp,r,j= 0;

		for(i = 0 ; i < 10 ; i ++) {
				printf("%d, ",arr[i]);
		}
		printf("\n");
		for(i = 0 ; i < 9 ; i ++) {
			r = i+1;
			while(1){

				if(arr[r] < arr[r-1]) {
					temp = arr[r-1];
					arr[r-1] = arr[r];
					arr[r] = temp;
					for(j = 0 ; j < 10 ; j ++) {
						printf("%d, ",arr[j]);
					}
					printf("\n");
					r--;
				} else  {
					//printf("Sorted\n");
					break;
				}
			}
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
