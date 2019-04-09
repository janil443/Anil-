#include<stdio.h>
#define  SIZE 4
int top = -1;
int stack [ SIZE ];
int pop () 
{
	int value = 0;
	if(top >= 0) {
		return --top; 

	} else {
		printf("Stack is empty.... \n");
		return -1;
	}
}
int push ( int element )
{
	int value = 0;
	if(top >= SIZE) {
		printf("Stack is full.... \n");
	} else {
	stack[++top] = element;
	}

	return 0;
};
int is_full()
{

	if(top >= SIZE) {
		printf("Stack is full.... \n");
	} else {
		printf("Stack is Not full.... \n");
	}
		
}
void is_empty() {
	if(top <= -1) {
		printf("Stack is empty.... \n");
	} else {
		printf("Stack is Not empty.... \n");
	}
}
int list ()
{
	int i;	
	if(top <= -1) {
		printf("Stack is Empty.... \n");
	} else {
		for( i = 0; i < top ; i++)
		printf("%d\t",stack[i]);
		printf("\n");
	}
	return 0;
}
int main ()
{
	int opertion;
starting:
	printf("Enter 1. push 2. pop 3. list 4.is_full 5.is_empty \n");
	scanf("%d",&opertion);
	switch (opertion) {
		case 1:
			scanf("%d",&opertion);
			push(opertion);
			goto starting;			
			break;
		case 2:
			pop();
			goto starting;			
			break;
		case 3:
			list();
			goto starting;			
			break;
		case 4:
			is_full();
			goto starting;			
			break;
		case 5:
			is_empty();
			goto starting;			
			break;
		case 6:	
			printf("Exiting.........\n");	
			return 0;
		default:
			goto starting;			
	}

		return 0;
}
