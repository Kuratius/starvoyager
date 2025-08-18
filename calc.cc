/*
	calc.cc
	
	(c) Richard Thrippleton
	Licensing terms are in the 'LICENSE' file
	If that file is not included with this source then permission is not given to use this source in any way whatsoever.
*/

#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "error.h"
#include "calc.h"
#include "os.h"


void calc::init()
{
	for(int i=0;i<10;i++)
	{
		wrp[i]=i*i*i*100;
	}
	srand(os::getseed());
}

int32_t calc::rnd(int32_t max)
{
	if(max==0)
		return 0;
	return (rand()%max);
}

void calc::getspeed(int32_t spd,char* put)
{
	int j; //To extract the value from the loop

	j=0;
	for(int i=9;i>=0;i--)
	{
		if(spd>=wrp[i])
		{
			j=i;
			break;
		}
	}
	if(j==0)
		if(spd<10)
			sprintf(put,"0.0%" PRId32 " c",spd);
		else
			sprintf(put,"0.%" PRId32 " c",spd);
	else
		sprintf(put,"Warp %hd",j);
}

bool calc::dateq(unsigned char* d1,unsigned char* d2,int n)
{
	for(int i=0;i<n;i++)
		if(d1[i]!=d2[i])
			return false;
	return true;		
}

void calc::obscure(char* str)
{
	for(int i=0,j=strlen(str);i<j;i++)
		str[i]+=i+1;
}

int32_t calc::wrp[10];
char calc::spds[33];
