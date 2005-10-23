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
#include <iostream>
#include <sstream>
#include <string>
#include <cstring>

using namespace std;

int main(int argc, char **argv)
{
	// check args...
	if(argc<2)
	{
		cerr<<argv[0]<<" url"<<endl;
		exit(0);
	}

	// init
	if(!NPP::init())
		NPP::error(NPP::release);

	cerr<<"webget : "<<NPP::release<<endl;
	
	try {
		string protocol("http"), host="", path="/";
		Uint16 port=80;

		// figure out the url pieces
		if(!NPP::spliturl(argv[1],protocol,host,port,path))
		{
			cerr<<"invalid url"<<endl;
			exit(1);
		}
		// check the protocol
		if(protocol!="http")
		{
			cerr<<"Protocol \""<<protocol<<"\" is not http!"<<endl;
			cerr<<"This program only handles http."<<endl;
			exit(1);
		}
		// check the port number
		if(!port)
		{
			cerr<<"port "<<port<<" is not valid!"<<endl;
			exit(1);
		}
		cerr<<"Connecting to "<<host<<':'<<port<<endl;

		// connect to the http server
		NPP::TCPsocket sock(host,port);

		NPP::IPaddress peer=sock.get_peer_address();
		cerr<<"Connected to "<<peer.get_hostname()<<"("<<peer.get_dottedquad()<<"):"
			<<peer.get_port()<<endl;

		cerr<<"Requesting: "<<path<<endl;

		// send the request
		sock<<"GET "<<path<<" HTTP/1.0"<<endl
			<<"User-Agent: webget/1.0"<<endl
			<<"Host: "<<host<<endl
			<<"Accept: */*"<<endl
			<<"Connection: close"<<endl
			<<endl;

		string line;
		int size=-1,bytes=0;

		// get the headers...
		while(std::getline(sock,line) && line.size()
				&& (line[0]!='\r' || line.size()!=1))
		{
			if(!line.find("Content-Length:"))
				if(!(istringstream(&line[15])>>size))
					size=-1;
			cerr<<line<<endl;
		}

		cerr<<"size="<<size<<endl;

		// do stats on the data portion :)
		Uint32 t=SDL_GetTicks(), last=t;

		if(!size)
			return 0;
		//size=-1;
		if(size==-1)
		{
			// get the data...
			while(sock.recv(line)>-1) // this may call getline anyways
			{
				cout<<line<<endl;
				bytes+=line.size()+1; //getline strips the '\n' so we add one here
				if(SDL_GetTicks()-last>500)
				{
					cerr<<'\r'<<bytes<<" bytes received";
					last=SDL_GetTicks();
				}
			}
			t=SDL_GetTicks()-t;
			cerr<<endl;
		}
		else
		{
			char *buf=new char[size]();
			t=SDL_GetTicks();
			bytes=sock.recv(buf,size);
			t=SDL_GetTicks()-t;
			if(bytes<0)
				NPP::error("sock.recv");
			if(bytes>0)
				cout.write(buf,bytes);
			delete buf;
		}

		// finish stats :)
		if(t)
			cerr<<"Received "<<bytes/1024.0<<" kbytes in "<<t/1000.0<<" seconds at "
				<<bytes/1024.0/(t/1000.0)<<" kb/s"<<endl;
	}
	catch(std::exception &e)
	{
		cerr<<"Caught exception: "<<e.what()<<endl;
	}

	return 0;
}
