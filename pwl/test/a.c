#include <stdlib.h>
struct T
{
	char _buff[256];
};
struct T* g_buff[10];
int main()
{
	int i = 0;
	for(; i < 10; ++ i)
	{
		g_buff[i] = malloc(sizeof(struct T));
		free(g_buff[i]);	
	}
	return 0;
}
