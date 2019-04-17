#include<stdio.h>

int main ()
{
	int  v=0, i,l=0;
	//int arr[8] = {0,0,1,1,1,1,0,0};
	//int arr[8] = {1,0,0,0,1,0,1,1};
	int arr[12] = {0,0,1,1,0,0,1,0, 1,1,1,0};

	for(i=0; i < 12; i++){
		if(arr[i] == 1)
		{
			if(l == -1) {
				v++;
				l++;
			} else {
				l++;
			}

		} else {
			l--;
		}

	}
	printf(" v = %d \n",v);
}
