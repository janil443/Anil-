#include <stdio.h>
#include <stdlib.h>

int main ()
{
	int num = 1;

	while(num <=100)
	{	
		if(!(num%15))
			printf("FizzBuzz ");	
		if(!(num%3))
			printf("Fizz ");	
		else if (!(num%5))
			printf("Buzz ");
		else
			printf("%d ",num);
		num++;
	}
	return 0;
}
