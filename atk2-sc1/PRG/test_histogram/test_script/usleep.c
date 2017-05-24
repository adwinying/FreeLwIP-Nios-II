#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

int main(void)
{
	srand((unsigned) time(NULL));
	Sleep((rand()%46 + 5) * 10);

	return 0;
}
