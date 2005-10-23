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
#include "SDL_net++.h"
#include "SDL_getenv.h"
#include <iostream>
#include <iomanip>
#include <sstream>

#define NON_THREADED

using namespace std;

int main(int argc, const char **argv)
{
	if(argc<2)
	{
		cerr<<argv[0]<<"[ip] port [message...]"<<endl;
		exit(0);
	}

	if(!NPP::init())
		NPP::error("NPP:init");

	try {
		int n=1;
		Uint16 port;
		if(strchr(argv[n],'.') || !(istringstream(argv[n])>>port))
			++n;
		if(n>=argc)
			NPP::error("missing port");
		if(!(istringstream(argv[n])>>port))
			NPP::error("port is not valid");
		NPP::IPaddress ip(INADDR_ANY);
		if(n==1)
			ip=NPP::IPaddress(INADDR_ANY,port);
		else
			ip=NPP::IPaddress(argv[1],port);

		if(argc-n<2)
		{
			cout<<"Listening on port "<<port<<endl;
			NPP::UDPsocket sock(port);
#ifdef NON_THREADED
			int i=0;
			NPP::UDPpacket packet;
			while(sock.poll(~0)>=0 && (i=sock.recv(packet))>0)
			{
				cerr<<endl
					<<"+++ Message received from "<<packet.get_address()
					<<" at "<<packet.get_at_tick()<<"(now="<<SDL_GetTicks()<<")"
					<<" size="<<packet.get_len()<<" +++"<<endl
					<<packet<<endl
					<<"--- "<<packet.get_address().get_hostname()<<" ---"<<endl<<endl;
			}
#else
			putenv("SDL_VIDEODRIVER=dummy");
			SDL_SetVideoMode(0,0,0,0);
			if(!sock.thread_start(1337))
				NPP::error("thread not started!");
			SDL_Event e;
			while(SDL_WaitEvent(&e))
			{
				switch(e.type)
				{
					case SDL_USEREVENT:
						if(e.user.code==1337)
						{
							NPP::UDPpacket *packet=(NPP::UDPpacket*)e.user.data1;
							if(!packet || !packet->get_data())
								NPP::error("WTF!");
							cerr<<endl
								<<"+++ Message received from "<<packet->get_address()
								<<" at "<<packet->get_at_tick()<<"(now="<<SDL_GetTicks()<<")"
								<<" size="<<packet->get_len()<<" +++"<<endl
								<<*packet<<endl
								<<"--- "<<packet->get_address().get_hostname()<<" ---"<<endl<<endl;
							delete packet;
						}
						break;
					case SDL_QUIT:
						return 0;
						break;
				}
			}
			NPP::error("no event");
			return 0;
#endif
		}
		else
		{
			cout<<"Broadcasting to port "<<port<<endl;
			NPP::UDPsocket sock;
			NPP::UDPpacket packet(strlen(argv[n+1]),(void*)argv[n+1]);
			packet.set_address(ip);
			for(int i=n+2; i<argc; i++)
			{
				packet.append(" ",1);
				packet.append((void*)argv[i],strlen(argv[i]));
			}
			cerr<<"Message sent to "<<packet.get_address()
				<<" size="<<packet.get_len()<<endl
				<<packet<<endl;
			if(sock.send(packet)<0)
				cerr<<"Error: "<<SDL_GetError();
		}

	}
	catch(std::exception &e)
	{
		cerr<<"Caught exception: "<<e.what()<<endl;
	}

	return 0;
}
