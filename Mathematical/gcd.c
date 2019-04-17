#include <stdio.h>
#if 0
int main()
{
	int a , b , temp;
	printf("Enter two number to get its GCD \n");
	scanf("%d %d",&a,&b);
	if(a == 0)
		printf("GCD: %d\n",b);
	else if(b == 0)
		printf("GCD: %d\n",a);
	else 
	{
		if( a < b) {
			temp = a;
			a = b;
			b = temp;
		}
		while( b != 0)
		{
			temp = b;
			b = a % b;
			a = temp;
		}
		printf("GCD: %d\n",a);
	}

	return 0;
}
#else


int gcd (int a, int b)
{
	if(a == 0)
	return b;

	return gcd(b%a, a);
}
int main()
{
	int a , b ;
	printf("Enter two number to get its GCD \n");
	scanf("%d %d",&a,&b);
	if(a == 0)
		printf("GCD: %d\n",b);
	else if(b == 0)
		printf("GCD: %d\n",a);
	printf("GCD: %d \n",gcd(a , b));

	printf("LCM %d \n",(a*b)/gcd(a, b));
	return 0;
}

#endif 
