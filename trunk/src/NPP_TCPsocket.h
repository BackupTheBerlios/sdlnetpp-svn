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
// TCPsocket : TCP socket server and client
////////////////////////////////////////////////////////////////////////////////

#ifndef NPP_TCPsocket_h
#define NPP_TCPsocket_h

#include "SDL_net.h"
#include "NPP_Socket.h"
#include "NPP_TCPsockbuf.h"
#include <iostream>
#include <stdexcept>

namespace NPP
{

class IPaddress;

class TCPsocket : public NPP::Socket, public TCPsockbuf, public std::iostream
{
	// the real deal for SDL_net
	::TCPsocket sock;

	// get my generic socket
	virtual NPP::Socket::GenericSocket get_generic_sock() const { return sock; }

public:
	// ctor to take a SDL_net TCPsocket
	// sock is 0 after finishing
	TCPsocket(::TCPsocket &sock) throw(std::logic_error);
	
	// ctor for a server socket on a port
	TCPsocket(Uint16 port) throw(std::runtime_error);

	// ctor for a server(ip=0.0.0.0) socket with a port
	//         or client with host ip number and port
	TCPsocket(const NPP::IPaddress ip_and_port) throw(std::runtime_error);

	// ctor for a server (host="" or "0.0.0.0") with port
	//         or client with hostname/ip number and port
	TCPsocket(const std::string host, Uint16 port) throw(std::runtime_error);

	// dtor
	// closes the socket
	virtual ~TCPsocket();

	// accept a new connection on a server socket
	// throws a runtime error if socket is invalid or accept fails
	TCPsocket *accept() throw(std::runtime_error);

	// (server) get the local address(0.0.0.0) and port
	// (client) get the peer address and port
	// throws on invalid sockets and failures
	NPP::IPaddress get_peer_address() throw(std::logic_error,std::runtime_error);

	// run a single SocketSet::check on this socket
	int poll(Uint32 timeout=0);

	// send a raw buffer of data of size len
	int send(const void *data, int len);

	// receive a raw buffer of data of size len
	int recv(void *data, int len);

	// send a raw buffer of data size of data.size()
	int send(const std::string data);

	// receive a raw buffer of data of any size up to a '\n'
	// the '\n' is eaten
	int recv(std::string &data);

	// test for a valid socket
	// sock != NULL and poll() > -1
	virtual bool valid();
};

};

#endif
