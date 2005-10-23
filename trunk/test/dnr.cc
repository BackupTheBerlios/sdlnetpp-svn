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

using namespace std;

int main(int argc, char **argv)
{
	if(argc<2)
	{
		cerr<<argv[0]<<" hostname|ipaddress"<<endl;
		exit(0);
	}

	if(!NPP::init())
		NPP::error("NPP:init");

	try {
		cout<<"Resolving "<<argv[1]<<':'<<12345<<endl;

		NPP::IPaddress ip(argv[1],12345);

		cout<<"operator<< = "<<ip<<endl;
		cout<<"hostname   = "<<ip.get_hostname()<<endl;
		cout<<"dottedquad = "<<ip.get_dottedquad()<<endl;
		cout<<"ip         = "<<(void*)ip.get_ip()<<endl;
		cout<<"port       = "<<ip.get_port()<<endl;
	}
	catch(std::exception &e)
	{
		cerr<<"Caught exception: "<<e.what()<<endl;
	}

	return 0;
}
