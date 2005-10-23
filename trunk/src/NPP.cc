/*
    SDL_net++
    Copyright (C) 2004 Jonathan Atkins

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    Jonathan Atkins
    jcatki@jonatkins.org
*/
////////////////////////////////////////////////////////////////////////////////
// SDL_net++ : init and various routines
////////////////////////////////////////////////////////////////////////////////

#include "NPP.h"
#include "SDL_net.h"
#include <iostream>
#include <sstream>

namespace NPP
{

using namespace std;

const char *release=PACKAGE"-"VERSION;
const char *package=PACKAGE;
const char *version=VERSION;

bool init()
{
	if(SDL_Init(0)!=-1)
	{
		atexit(SDL_Quit);
		if(SDLNet_Init()!=-1)
		{
			atexit(SDLNet_Quit);
			return true;
		}
		else
			return false;
	}
	else
		return false;
}

void error(const char *msg, int exitcode)
{
	if(strlen(SDL_GetError()))
		cerr<<msg<<": "<<SDL_GetError()<<endl;
	else
		cerr<<msg<<": unknown error"<<endl;
	exit(exitcode);
}

bool spliturl(string url_, string &protocol, string &host, Uint16 &port, string &path)
{
	char *url, *p,*q;
	string portnum;

	url=(char*)url_.c_str();
	//protocol="http";
	//host="";
	//path="/";
	//port=80;
	p=strstr(url,"://");
	if(p)
	{
		*p=0;
		if(p!=url)
			protocol=url;
		*p=':';
		url=p+3;
	}
	char *u=url;
	q=strchr(url,':');
	p=strchr(url,'/');
	if(q && (q<p || !p))
	{
		*q=0;
		url=q+1;
	}
	if(p)
	{
		path=p;
		*p=0;
	}
	if(q)
		portnum=q+1;
	host=u;
	if(!host.size())
		return false;
	if(q)
		*q=':';
	if(p)
		*p='/';
	if(portnum.size() && !(istringstream(portnum)>>port))
		return false;
	//cerr<<"http://|"<<host<<"|:|"<<port<<"|"<<path<<endl;
	return true;
}

};
