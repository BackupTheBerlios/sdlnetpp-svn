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

#include "NPP_TCPsocket.h"
#include "NPP_IPaddress.h"
#include "NPP_SocketSet.h"
#include <string>

namespace NPP
{

using namespace std;
	
TCPsocket::TCPsocket(::TCPsocket &sock_) throw(std::logic_error)
	: NPP::Socket(TCP)
	, TCPsockbuf(this)
	, std::iostream(this)
	, sock(sock_)
{
	if(!sock)
		throw std::logic_error("null sock passed to ");
	sock_=0; //prevent further usage...sorta
}

TCPsocket::TCPsocket(Uint16 port) throw(std::runtime_error)
	: NPP::Socket(TCP)
	, TCPsockbuf(this)
	, std::iostream(this)
{
	NPP::IPaddress ip("",port);
	sock=SDLNet_TCP_Open(&ip.ipaddr);
	if(!sock)
		throw std::runtime_error(SDL_GetError());
}

TCPsocket::TCPsocket(const NPP::IPaddress ip_and_port) throw(std::runtime_error)
	: NPP::Socket(TCP)
	, TCPsockbuf(this)
	, std::iostream(this)
{
	sock=SDLNet_TCP_Open(const_cast< ::IPaddress*>(&ip_and_port.ipaddr));
	if(!sock)
		throw std::runtime_error(SDL_GetError());
}

TCPsocket::TCPsocket(const std::string host, Uint16 port) throw(std::runtime_error)
	: NPP::Socket(TCP)
	, TCPsockbuf(this)
	, std::iostream(this)
{
	NPP::IPaddress ip_and_port(host,port);
	sock=SDLNet_TCP_Open(const_cast< ::IPaddress*>(&ip_and_port.ipaddr));
	if(!sock)
		throw std::runtime_error(SDL_GetError());
}

TCPsocket::~TCPsocket()
{
	if(sock)
		SDLNet_TCP_Close(sock);
	sock=0;
}

bool TCPsocket::valid()
{
	return (sock
			&& poll()>-1);
			//&& SDLNet_TCP_Recv(sock,0,0)==0);
}

TCPsocket *TCPsocket::accept() throw(std::runtime_error)
{
	if(!valid())
		throw std::runtime_error("invalid socket");
	::TCPsocket newsock=SDLNet_TCP_Accept(sock);
	if(!newsock)
		throw std::runtime_error(SDL_GetError());
	return new TCPsocket(newsock);
}

NPP::IPaddress TCPsocket::get_peer_address() throw(std::logic_error,std::runtime_error)
{
	if(!valid())
		throw std::runtime_error("invalid socket");
	::IPaddress *ipaddr=SDLNet_TCP_GetPeerAddress(sock);
	if(!ipaddr)
		throw std::logic_error(SDL_GetError());
	return IPaddress(*ipaddr);
}

int TCPsocket::poll(Uint32 timeout)
{
	if(!sock)
		return -1;
	static NPP::SocketSet set;
	set.clear();
	set.add(*this);
	return set.check(timeout);
}

int TCPsocket::send(const void *data, int len)
{
	if(!valid())
		return -1;
	return SDLNet_TCP_Send(sock,(void*)data,len);
}

int TCPsocket::recv(void *data, int len)
{
	int n=0;

	if(!valid())
		return -1;
	while(len>n)
	{
		int err=SDLNet_TCP_Recv(sock,(char*)data+n,len-n);
		if(err<0)
			return n?n:err;
		if(err<1)
			return n;
		n+=err;
	}
	return n;
}

int TCPsocket::send(const std::string str)
{
	if(!valid())
		return -1;
	return send(str.data(),str.size());
}

int TCPsocket::recv(std::string &data)
{
#if 0
	if(std::getline(*this,data))
		return data.size();
	return -1;
#else
	data="";
	char c;
	do
	{
		int err=recv(&c,1);
		if(err<0)
			return data.size()?data.size():err;
		if(err<1)
			return -1;
		if(c=='\n')
			break;
		data.append(1,c);
	} while(c!='\n');
	return data.size();
#endif
}

};
