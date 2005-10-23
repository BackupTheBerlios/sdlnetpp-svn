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

#ifndef NPP_TCPsockbuf
#define NPP_TCPsockbuf

#include <streambuf>
#include <ios>

namespace NPP
{

class TCPsocket;
	
class TCPsockbuf : public std::streambuf
{
	TCPsocket *sock;
	char in;

protected:
	TCPsockbuf(TCPsocket *sock);
	virtual ~TCPsockbuf() {}
	virtual int_type overflow(int_type c);
	virtual int_type underflow();

	// calls recv
	virtual std::streamsize xsgetn(char_type* __s, std::streamsize __n);
	// calls send
	virtual std::streamsize xsputn(const char_type* __s, std::streamsize __n);
};

};

#endif 
