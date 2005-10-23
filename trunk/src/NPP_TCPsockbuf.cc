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
// TCPsockbuf : a streambuf for TCPsocket
////////////////////////////////////////////////////////////////////////////////

#include "NPP_TCPsockbuf.h"
#include "NPP_TCPsocket.h"

namespace NPP
{

TCPsockbuf::TCPsockbuf(TCPsocket *sock)
	: sock(sock)
{
	//XXX unbuffered
	setp(0,0);
	setg(0,0,0);
}

std::streambuf::int_type TCPsockbuf::overflow(int_type i)
{
	unsigned char c=i;
	if(!sock->valid() || sock->send(&c,1)<1)
		return EOF;
	return i;
}

std::streambuf::int_type TCPsockbuf::underflow()
{
	setg(0,0,0);
	if(!sock || sock->recv(&in,1)<1)
		return EOF;
	setg(&in,&in,(&in)+1);
	return (unsigned char)in;
}

std::streamsize TCPsockbuf::xsgetn(char_type* __s, std::streamsize __n)
{
	return sock->recv(__s, __n);
}

std::streamsize TCPsockbuf::xsputn(const char_type* __s, std::streamsize __n)
{
	return sock->send(__s, __n);
}
};
