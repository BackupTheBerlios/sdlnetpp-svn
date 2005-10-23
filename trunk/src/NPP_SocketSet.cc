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
// SocketSet : input ready / connection waiting testing
////////////////////////////////////////////////////////////////////////////////

#include "NPP_SocketSet.h"
#include "NPP_Socket.h"

namespace NPP
{

using namespace std;
	
SocketSet::SocketSet()
	: set_dirty(false)
	, set(0)
{
}

SocketSet::~SocketSet()
{
	clear();
}

void SocketSet::add_sock(NPP::Socket &sock)
{
	socks.insert(&sock);
	set_dirty=true;
}

void SocketSet::remove_sock(NPP::Socket &sock)
{
	socks.erase(&sock);
	set_dirty=true;
}

void SocketSet::add(NPP::Socket &sock)
{
	add_sock(sock);
	sock.add_set(*this);
}

void SocketSet::remove(NPP::Socket &sock)
{
	remove_sock(sock);
	sock.remove_set(*this);
}

void SocketSet::clear()
{
	if(set)
	{
		SDLNet_FreeSocketSet(set);
		set=0;
	}
	for(Socks::iterator i=socks.begin(); i!=socks.end(); ++i)
		(*i)->remove_set(*this);
	socks.clear();
}

int SocketSet::check(Uint32 timeout)
{
	if(!socks.size())
	{
		if(set)
		{
			SDLNet_FreeSocketSet(set);
			set=0;
		}
		SDL_SetError("SocketSet is empty");
		return -1;
	}
	if(set_dirty)
	{
		set=SDLNet_AllocSocketSet(socks.size());
		if(!set)
			return -2;
		for(Socks::iterator i=socks.begin(); i!=socks.end(); ++i)
		{
			::SDLNet_GenericSocket sock=(::SDLNet_GenericSocket)(*i)->get_generic_sock();
			if(sock)
				SDLNet_AddSocket(set, sock);
		}
		set_dirty=false;
	}
		
	return SDLNet_CheckSockets(set,timeout);
}

NPP::Socket *SocketSet::operator[](int i) throw(out_of_range)
{
	if(i<0 || i>size())
		throw out_of_range("access out of bounds in SocketSet");
	Socks::iterator it=socks.begin();
	advance(it,i);
	return *it;
}

};
