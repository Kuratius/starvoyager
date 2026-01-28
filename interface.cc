/*
	interface.cc
	
	(c) Richard Thrippleton
	Licensing terms are in the 'LICENSE' file
	If that file is not included with this source then permission is not given to use this source in any way whatsoever.
*/

#include <SDL.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "graphic.h"
#include "error.h"
#include "protocol.h"
#include "calc.h"
#include "interface.h"

int getCharFromSDL_SCANCODE(int scancode);

void interface::init()
{
	cons=NULL;
	for(int i=0;i<8;i++)
		mesg[i]=NULL;
	edit[0]='\0';
	inp=false;
	ent=false;
	cspr=NULL;
}

void interface::setup()
{
	sbox tot; //Total screen area

	tot=graphic::dimension();
    tot.h=400;
    tot.w=600;
	viewb.x=tot.x;
	viewb.y=tot.y;
	viewb.w=tot.h;
	viewb.h=tot.h;

	panelb.x=viewb.x+viewb.w;
	panelb.y=tot.y;
	panelb.w=(tot.x+tot.w)-panelb.x;
	panelb.h=400;

	consb.x=panelb.x+5;
	consb.y=panelb.y+200;
	consb.w=(panelb.x+panelb.w)-consb.x-5;
	consb.h=panelb.h-214;

	radarb.x=panelb.x+5;
	radarb.y=panelb.y+25;
	radarb.w=150;
	radarb.h=radarb.w;

	barsb.x=panelb.x+panelb.w-38;
	barsb.y=panelb.y+75;
	barsb.w=32;
	barsb.h=100;

	editb.x=consb.x;
	editb.y=consb.y+consb.h+1;
	editb.w=consb.w;
	editb.h=10;

	mesgb.x=viewb.x+7;
	mesgb.y=viewb.y+viewb.h-70;
	mesgb.w=viewb.w-20;
	mesgb.h=viewb.y+viewb.h-mesgb.y-8;

	if(cons)
		delete[] cons;
	cons=new char[(consb.w/7)*(consb.h/7)];
	cons[0]='\0';

	//SDL_EnableUNICODE(1);
	//SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY,SDL_DEFAULT_REPEAT_INTERVAL);

	mtmo=0;
}

void interface::poll()
{
	SDL_Event evnt; //Event to be polled
	int n; //Unused area for the argument for SDL_GetKeyState
	lkey=-1;
	lasc=0;
	SDL_PumpEvents();
	keys=SDL_GetKeyboardState(&n);
	while(lkey==-1 && SDL_PollEvent(&evnt))
	{
		if(evnt.type==SDL_KEYDOWN)
		{
			lkey=evnt.key.keysym.sym;
			scancode=evnt.key.keysym.scancode;
			lasc=getCharFromSDL_SCANCODE(evnt.key.keysym.scancode);
            if (keys[SDL_SCANCODE_LSHIFT])
                lasc=toupper(lasc);
			if(inp)
				lineedit();
		}
		if(evnt.type==SDL_QUIT)
			throw error("User requested quit");
	}
}

void interface::printtocons(const char* fmt,...)
{
	va_list fmts; //To help parsing the vargs

	inp=false;
	edit[0]='\0';
	cspr=NULL;
	va_start(fmts,fmt);
	vsprintf(cons,fmt,fmts);
	va_end(fmts);
}

void interface::spritetocons(graphic* spr)
{
	inp=false;
	edit[0]='\0';
	cspr=spr;
}

void interface::printtomesg(const char* fmt,...)
{
	va_list fmts; //To help parsing the vargs
	char txt[1024]; //Temporary text buffer

	if(fmt)
	{
		mtmo=100;
		if(mesg[0])
			delete mesg[0];
		mesg[0]=mesg[1];
		mesg[1]=mesg[2];
		mesg[2]=mesg[3];
		mesg[3]=mesg[4];
		mesg[4]=mesg[5];
		mesg[5]=mesg[6];
		mesg[6]=mesg[7];
		mesg[7]=NULL;
		va_start(fmts,fmt);
		vsprintf(txt,fmt,fmts);
		va_end(fmts);
		mesg[7]=new char[strlen(txt)+1];
		strcpy(mesg[7],txt);
		if((int)strlen(txt)>(mesgb.w/6))
			printtomesg("%s",txt+(mesgb.w/6));
	}
	else
	{
		for(int i=0;i<8;i++)
		{
			if(mesg[i])
			{
				delete[] mesg[i];
				mesg[i]=NULL;
			}
		}
	}
}

void interface::render()
{
	char* p1;
	char* p2; //Pointers for parsing text into console
	int y; //Y co-ordinate for writing to console
	char line[256]; //Line of console for scratchpad operations
	int elen; //String length of edit buffer
	int cwid; //Console width (in characters)

	graphic::clip(&mesgb);

	if(mtmo>0)
	{
		mtmo--;
		for(int i=0,y=mesgb.y;i<8;i++,y+=7)
			if(mesg[i])
				graphic::string(mesg[i],mesgb.x,y,false);
	}

	graphic::clip(&consb);

	if(cspr)
		cspr->draw(consb.x+consb.w/2,consb.y+consb.h/2,0,1,0,false);
	cwid=consb.w/6;
	y=consb.y;
	p1=cons;
	p2=cons;
	while(*p2)
	{
		if(*p2=='\0' || *p2=='\n' || p2-p1==cwid || p2-p1>=255)
		{
			memcpy(line,p1,p2-p1+1);
			line[p2-p1+1]='\0';
			graphic::string(line,consb.x,y,true);
			y+=7;
			p1=p2+1;
			if(*p2=='\0')
				break;
		}
		p2++;
	}

	graphic::clip(&editb);
	if(inp)
		graphic::box(&editb,graphic::RED);
	else
		graphic::box(&editb,graphic::BLACK);
	elen=strlen(edit);
	if(pwd)
	{
		for(int i=0;i<elen;i++)
			line[i]='*';
		line[elen]='\0';
		graphic::string(line,editb.x+2,editb.y+2,false);
	}
	else
	{
		if(elen>cwid)
			graphic::string(edit+(elen-cwid),editb.x+2,editb.y+2,false);
		else
			graphic::string(edit,editb.x+2,editb.y+2,false);
	}
}

sbox interface::consb;

bool interface::getline(char* put,bool hide)
{
	pwd=hide;
	if(!inp)
	{
		inp=true;
		ent=false;
		edit[0]='\0';
	}
	if(ent)
	{
		strcpy(put,edit);
		inp=false;
		ent=false;
		return true;
	}
	else
		return false;
}

sbox interface::viewb,interface::radarb,interface::barsb,interface::panelb;
bool interface::inp;
int interface::lkey;
unsigned char interface::lasc;
const uint8_t* interface::keys;
int interface::scancode;

void interface::lineedit() //Function to handle line-editing
{
	if(scancode==SDL_SCANCODE_RETURN)
	{
		ent=true;
		return;
	}
	if(scancode==SDL_SCANCODE_BACKSPACE)
	{
        //printf("tried to remove character\n");
		for(int i=1;i<65;i++)
		{
			if(edit[i]=='\0')
			{
				edit[i-1]='\0';
				break;
			}
		}
	}
	if(lasc>=32 && lasc<127)
	{
		for(int i=0;i<64;i++)
		{
			if(edit[i]=='\0')
			{
			    //char * s=SDL_GetScancodeName(lasc);
                //printf("got: %s\n", s);
				edit[i]=lasc;
				edit[i+1]='\0';
				break;
			}
		}
	}
}


int getCharFromSDL_SCANCODE(int scancode){
    switch (scancode){
        case SDL_SCANCODE_0:
            return '0'; 
        case SDL_SCANCODE_1:
            return '1';
        case SDL_SCANCODE_2:
            return '2';

        case SDL_SCANCODE_3:
            return '3';

        case SDL_SCANCODE_4:
            return '4';

        case SDL_SCANCODE_5:
            return '5';

        case SDL_SCANCODE_6:
            return '6';

        case SDL_SCANCODE_7:
            return '7';
        case SDL_SCANCODE_8:
            return '8';
        case SDL_SCANCODE_9:
            return '9';
        case SDL_SCANCODE_A:
            return 'A';
        case SDL_SCANCODE_B:
            return 'B';
        case SDL_SCANCODE_C:
            return 'C';
        case SDL_SCANCODE_D:
            return 'D';
        case SDL_SCANCODE_E:
            return 'E';
        case SDL_SCANCODE_F:
            return 'F';
        case SDL_SCANCODE_G:
            return 'G';
        case SDL_SCANCODE_H:
            return 'H';
        case SDL_SCANCODE_I:
            return 'I';
        case SDL_SCANCODE_J:
            return 'J';
        case SDL_SCANCODE_K:
            return 'K';
        case SDL_SCANCODE_L:
            return 'L';
        case SDL_SCANCODE_M:
            return 'M';
        case SDL_SCANCODE_N:
            return 'N';
        case SDL_SCANCODE_O:
            return 'O';
        case SDL_SCANCODE_P:
            return 'P';
        case SDL_SCANCODE_Q:
            return 'Q';
        case SDL_SCANCODE_R:
            return 'R';
        case SDL_SCANCODE_S:
            return 'S';
        case SDL_SCANCODE_T:
            return 'T';
        case SDL_SCANCODE_U:
            return 'U';
        case SDL_SCANCODE_V:
            return 'V';
        case SDL_SCANCODE_X:
            return 'X';
        case SDL_SCANCODE_Y:
            return 'Y';
        case SDL_SCANCODE_Z:
            return 'Z';
        default:
        return 0;
    }   


}




sbox interface::mesgb;
sbox interface::editb;
char* interface::cons;
char* interface::mesg[8];
char interface::edit[65];
bool interface::pwd;
bool interface::ent;
graphic* interface::cspr;
int interface::mtmo;
