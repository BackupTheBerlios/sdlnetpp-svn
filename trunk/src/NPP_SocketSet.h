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

#ifndef NPP_SocketSet_h
#define NPP_SocketSet_h

#include "SDL_net.h"
#include <set>
#include <stdexcept>

namespace NPP
{

class Socket;

class SocketSet
{
	// type for set of socks
	typedef std::set<NPP::Socket*> Socks;

	// socks in this set
	Socks socks;

	// do we need to rebuild set?
	bool set_dirty;

	// the real deal for SDL_net
	SDLNet_SocketSet set;

	// called by by Socket to un/register itself
	void add_sock(NPP::Socket &sock);
	void remove_sock(NPP::Socket &sock);

	// make copying illegal
	SocketSet(const SocketSet &set) {};

public:
	// ctor
	SocketSet();

	// dtor
	~SocketSet();

	// add a socket to this set
	void add(NPP::Socket &sock);

	// remove a socket from this set
	void remove(NPP::Socket &sock);

	// remove all sockets from this set
	void clear();

	// update the ready status on all sockets
	// wait for timeout milliseconds for a socket in the set to have data
	// the default is not to wait
	// -1 will wait for up to 49 days or so
	int check(Uint32 timeout=0);

	// traverse the member sockets as an array
	// throws out_of_range if you go beyond [0..size()-1]
	NPP::Socket *operator[](int i) throw(std::out_of_range);

	// get the number of socket members
	int size() { return socks.size(); }

// friends
	friend class NPP::Socket;
};

};

#endif
