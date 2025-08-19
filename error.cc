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
    //char temp[129];
    //temp[128]='\0';
    int len=strlen(str);
	if(len>128)
    {
		//str[128]='\0';
        printf("FATAL ERROR! STRING LENGTH EXCEEDS BUFFER SIZE \n");
        return;
    }
/*
    for(int i=0; i<len;i++){
        temp[i]=str[i];
    }
*/

//    temp[len]='\0';


	strcpy(this->str,str);

}

void error::debug(const char* str,long num)
{
	printf("%s  %ld\n",str,num);
	fflush(NULL);
}
