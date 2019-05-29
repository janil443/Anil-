#include <stdio.h>
#include <stdlib.h>


static int arr[3][3];
void get_pos(int *x, int *y, int n)
{
	if(*x - 1 < 0)
		*x = n;
	else 
		*x -= 1;
	if(*y + 1 > n)
		*y = 0;
	else
		*y += 1;
	 
//	printf("%s: x =%d y=%d \n ",__func__,*x,*y);
}
int swap(int arr[3][3], int x, int y)
{
//	printf("%s arr =%p x=%d y =%d\n",__func__,arr,x,y);
	int temp;
	 temp = arr[x][0];
	 arr[x][0] = arr[y][2];
	 arr[y][2] = temp;
	return 0;
}
int rotate_outer(int arr[3][3])
{
	int temp,a,b,c,d;
	a = arr[0][0]; 
	b = arr[0][2]; 
	c = arr[2][2]; 
	d = arr[2][0]; 
	arr[0][0] = d;
	arr[0][2] = a;
	arr[2][2] = b;
	arr[2][0] = c;
}

int rotate_inner(int arr[3][3])
{
	int temp,a,b,c,d;
	a = arr[0][1]; 
	b = arr[1][2]; 
	c = arr[2][1]; 
	d = arr[1][0]; 
	arr[0][1] = d;
	arr[1][2] = a;
	arr[2][1] = b;
	arr[1][0] = c;
}
int rotate_matrix(int arr[3][3]){

	int i ,j;
	int temp;
	rotate_outer(arr); 		
	rotate_inner(arr);
	return 0;
}
int get_mirror_image(int arr[3][3]){

	int i ,j;
//	printf("%s arr =%p\n",__func__,arr);
	for(i = 0 ; i < 3; i++)
	{
		swap(arr,i,i); 		

	}

	return 0;
}
int main ()
{
	int i,j,x, y,a,b,z;
	//	printf("%s arr =%p\n",__func__,arr);

	for(i = 1 ; i < 10; i++)
	{
		if(i == 1) {
			x = 0;
			y = 1;
		} else {
			get_pos(&x, &y, 2);		
		}
		if(arr[x][y] != 0) {
			x = a+1;
			y = b; 
		} 
		a = x;
		b = y;
		arr[x][y] = i;	
	}
	
#if 1
	for(i = 0 ; i < 3; i++)
	{
		for(j = 0 ; j < 3; j++)
			printf("%d ",arr[i][j]);
		printf("\n");

	}
	printf("\n");
	for (z =0; z < 3; z++){
		rotate_matrix(arr);

		for(i = 0 ; i < 3; i++)
		{
			for(j = 0 ; j < 3; j++)
				printf("%d ",arr[i][j]);
			printf("\n");

		}
	printf("\n");
	}
	printf("\n");
	get_mirror_image(arr);

	for(i = 0 ; i < 3; i++)
	{
		for(j = 0 ; j < 3; j++)
			printf("%d ",arr[i][j]);
		printf("\n");

	}
	printf("\n");
	for (z =0; z < 3; z++){
		rotate_matrix(arr);

		for(i = 0 ; i < 3; i++)
		{
			for(j = 0 ; j < 3; j++)
				printf("%d ",arr[i][j]);
			printf("\n");

		}
	printf("\n");
	}
#endif
	return 0;
}
