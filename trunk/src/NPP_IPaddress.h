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
// IPaddress : name resolution and connection addressing
////////////////////////////////////////////////////////////////////////////////

#ifndef NPP_IPaddress_h
#define NPP_IPaddress_h

#include "SDL_net.h"
#include <string>
#include <iostream>
#include <stdexcept>

namespace NPP
{

class TCPsocket;
class UDPsocket;
class UDPpacket;

class IPaddress
{
	// the real deal for SDL_net
	::IPaddress ipaddr;

	// hostname lookup cache
	std::string hostname;

	// dotted quad cache
	std::string dottedquad;

public:
	// ctor from SDL_net IPaddress
	IPaddress(const ::IPaddress &ipaddr);

	// copy ctor
	IPaddress(const NPP::IPaddress &ip);

	// ctor from host and port
	// throws when host fails to resolve
	IPaddress(const std::string host, Uint16 port=0) throw(std::runtime_error);

	// ctor for host ordered ip and port
	IPaddress(Uint32 ip, Uint16 port=0);

	// INADDR_ANY
	static const IPaddress Any;

	// INADDR_NONE
	static const IPaddress None;

	// INADDR_BROADCAST
	static const IPaddress Broadcast;

	// get resolved true hostname
	const char *get_hostname();

	// get printable ip number
	const char *get_dottedquad();

	// get ip in host byte order
	Uint32 get_ip() const;

	// get port in host byte order
	Uint16 get_port() const;

// friends
	friend class NPP::TCPsocket;
	friend class NPP::UDPsocket;
	friend class NPP::UDPpacket;
};

}

// output dottedquad:port
std::ostream &operator<<(std::ostream &out, const NPP::IPaddress &ip);

#endif
