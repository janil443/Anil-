#include<stdio.h>
#include<stdlib.h>

int main () {

	int arr[10] = { 3, 0, -1, 3, 4, 9, 2, 1, 88 ,23 };

	int i,temp,r,j= 0;

	for(i = 0 ; i < 10 ; i ++) {
		printf("%d, ",arr[i]);
	}
	printf("\n");

	for(i = 0 ; i < 10 ; i++) {

		for(j = 0 ; j < 10 - i ; j++) {
			if( arr[j] > arr[j+1])
			{
				temp = arr[j+1];						
				arr[j+1] = arr[j];
				arr[j] = temp;
				//		printf("[%d] <--> [%d]\n, ",arr[j],arr[j+1]);
			} else {
				//		printf("*[%d] <--> [%d]\n, ",arr[j],arr[j+1]);

			}
		}
#if 0
		for(r = 0 ; r < 10 ; r ++) {
			printf("*%d, ",arr[r]);
		}
		printf("\n");
#endif
	}
	for(i = 0 ; i < 10 ; i ++) {
		printf("%d, ",arr[i]);
	}
	printf("\n");
	return 0;
}




