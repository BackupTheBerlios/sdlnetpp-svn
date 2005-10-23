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

#ifndef NPP_Socket_h
#define NPP_Socket_h

#include <set>

namespace NPP
{

class SocketSet;

class Socket
{
public:
	// types of sockets
	enum Type { UDP, TCP };

private:
	// type for set of sets
	typedef std::set<NPP::SocketSet*> Sets;

	// my type of socket
	Type type;

	// sets of which this socket is a member of
	Sets sets;

	// called by by SocketSet to un/register itself
	void add_set(NPP::SocketSet &set);
	void remove_set(NPP::SocketSet &set);

	// make copying illegal
	Socket(const Socket &sock) {}

protected:
	// ctor for a type of socket, used by TCP and UDP socket classes
	Socket(Type type);

	// type of generic socket (for SDL_net calls)
	typedef void *GenericSocket;

	// get a generic socket pointer, used for SocketSets
	virtual GenericSocket get_generic_sock() const =0;

public:
	// dtor
	virtual ~Socket();

	// get type of socket
	Type get_type() const { return type; }

	// put this socket into a SocketSet
	void add_to_set(NPP::SocketSet &set);

	// remove this socket from a SocketSet
	void remove_from_set(NPP::SocketSet &set);

	// test ready for reading
	// only useful after a set.check() with this socket in it
	// returns true if there's data waiting to be read
	bool ready() const;

	// test if the socket is valid or not
	virtual bool valid()=0;

// friends
	friend class NPP::SocketSet;
};

};

#endif
