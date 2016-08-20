#include <stdio.h>

int main(int argc, char* argv[])
{
	printf("Hello, userspace World!\n");
	printf("Argument vector:");
	for ( int i = 0; i < argc; i++ )
		printf(" %s", argv[i]);
	printf("\n");
	return 0;
}
