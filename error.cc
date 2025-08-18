/*
	error.cc
	
	(c) Richard Thrippleton
	Licensing terms are in the 'LICENSE' file
	If that file is not included with this source then permission is not given to use this source in any way whatsoever.
*/

#include <string.h>
#include <stdio.h>
#include <signal.h>
#include "error.h"

error::error(const char* str)
{
    char temp[129];
    temp[128]='\0';
	if(strlen(str)>128)
    {
		//str[128]='\0';
        printf("FATAL ERROR! STRING LENGTH EXCEEDS BUFFER SIZE \n"); 
    }

    for(int i=0; i<128;i++){
        temp[i]=str[i];
    }


	strcpy(this->str,temp);
}

void error::debug(const char* str,long num)
{
	printf("%s  %ld\n",str,num);
	fflush(NULL);
}
