#include <stdio.h>
#include <stdlib.h>

void find_the_no_of_pairs(int *arr, int no_of_socks) {
	int sale;
	int i,temp,r,j= 0;
	int n = no_of_socks;

	printf("Your pile of socks are:\n");
	for(i = 0; i< no_of_socks; i++)
	{
		printf("[%d] ",arr[i]);
	}
	printf("\n");
	for(i = 0 ; i < no_of_socks ; i ++) {
		r = i;
		for( j = i; j < no_of_socks; j ++) {
			if (!(arr[r] <= arr[j]))
				r = j;
		}
		temp = arr[i];
		arr[i] = arr[r];
		arr[r] = temp;
	}
	for(i = 0 ; i < no_of_socks ; i ++) {
		printf("%d, ",arr[i]);
	}
	printf("\n");
	temp = 0;
	--n;
	while(n >= 0)
	{
		printf("n=%d ",n);
		printf("arr[%d]=%d == arr[%d]=%d \n",n,arr[n],n-1,arr[n-1]);
		if(arr[n] == arr[n-1]){
			printf("True \n");
			temp++;
			n -= 2;
			continue;	
		}
			printf("False \n");
		n--;
	}
	printf("%s: pairs %d \n",__func__,temp);
	return ;

}
int main () {
	int no_of_socks;
	int *arr;
	printf("Enter the no of socks valid ranges is [1 to 100] \n");
scan:
	scanf("%d",&no_of_socks);
	if(no_of_socks < 1 || no_of_socks >100) {
		printf("Invalid input you had entered the %d no of socks [1 -100] are valid \n",no_of_socks);
		goto scan;				
	}
	else
		printf("You had entered the %d no of socks \n",no_of_socks);
	arr = calloc(no_of_socks, sizeof(int));
	if(arr) {
		printf("please enter the color codes for %d no of socks \n",no_of_socks);
		int i = 0;
		while(i < no_of_socks)
			scanf("%d",&arr[i++]);
		find_the_no_of_pairs(arr, no_of_socks);
		free(arr);
		return 0;
	} else {
		printf("No memory \n");
		return 0;	
	}





}



