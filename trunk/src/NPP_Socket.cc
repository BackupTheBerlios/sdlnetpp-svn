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
// Socket : the base class for all types of sockets
////////////////////////////////////////////////////////////////////////////////

#include "NPP_Socket.h"
#include "NPP_SocketSet.h"
#include "NPP_TCPsocket.h"
#include "NPP_UDPsocket.h"

namespace NPP
{

Socket::Socket(Type type)
	: type(type)
{
}

Socket::~Socket()
{
	for(Sets::iterator i=sets.begin(); i!=sets.end(); ++i)
		(*i)->remove_sock(*this);
}

void Socket::add_set(NPP::SocketSet &set)
{
	sets.insert(&set);
}

void Socket::remove_set(NPP::SocketSet &set)
{
	sets.erase(&set);
}

void Socket::add_to_set(NPP::SocketSet &set)
{
	set.add(*this);
}

void Socket::remove_from_set(NPP::SocketSet &set)
{
	set.remove(*this);
}

bool Socket::ready() const
{
	return SDLNet_SocketReady(get_generic_sock())!=0;
}

};
