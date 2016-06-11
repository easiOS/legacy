#include "video.h"
#include "eelphant.h"

int main(void)
{
	vinit(240, 160, 0, 0, 0);
	vsetcol(255, 0, 0, 0);
	eelphant_main(240, 160);
	while(1);
	return 0;
}
