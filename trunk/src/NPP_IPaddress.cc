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

#include "NPP_IPaddress.h"
#include <sstream>

namespace NPP
{
	
const IPaddress IPaddress::Any(INADDR_ANY);
const IPaddress IPaddress::None(INADDR_NONE);
const IPaddress IPaddress::Broadcast(INADDR_BROADCAST);

union Quad {
	Uint32 l;
	Uint8 c[sizeof(Uint32)];
	Quad(Uint32 l=0) : l(SDLNet_Read32(&l)) {}
};

IPaddress::IPaddress(const ::IPaddress &ipaddr_)
	: ipaddr(ipaddr_)
	, hostname()
	, dottedquad()
{
}

IPaddress::IPaddress(const NPP::IPaddress &ip)
	: ipaddr(ip.ipaddr)
	, hostname(ip.hostname)
	, dottedquad(dottedquad)
{
}

IPaddress::IPaddress(const std::string host, Uint16 port) throw(std::runtime_error)
	: hostname()
	, dottedquad()
{
	int err;
	if(host.size())
		err=SDLNet_ResolveHost(&ipaddr, host.c_str(), port);
	else
		err=SDLNet_ResolveHost(&ipaddr, NULL, port);
	if(err==-1)
		throw std::runtime_error("Could not resolve host");
}

IPaddress::IPaddress(Uint32 ip, Uint16 port)
{
	SDLNet_Write32(ip,&ipaddr.host);
	SDLNet_Write16(port,&ipaddr.port);
}

const char *IPaddress::get_hostname()
{
	if(!hostname.size())
	{
		const char *host=SDLNet_ResolveIP(&ipaddr);
		if(!host)
			return get_dottedquad();
		hostname=host;
	}
	return hostname.c_str();
}

const char *IPaddress::get_dottedquad()
{
	if(!dottedquad.size())
	{
		std::ostringstream oss;
		Quad q(SDLNet_Read32(&ipaddr.host));
		oss <<(int)q.c[0]<<'.'
			<<(int)q.c[1]<<'.'
			<<(int)q.c[2]<<'.'
			<<(int)q.c[3];
		dottedquad=oss.str();
	}
	return dottedquad.c_str();
}

Uint32 IPaddress::get_ip() const
{
	return SDLNet_Read32(&ipaddr.host);
}

Uint16 IPaddress::get_port() const
{
	return SDLNet_Read16(&ipaddr.port);
}

};

std::ostream &operator<<(std::ostream &out, const NPP::IPaddress &ip)
{
	NPP::Quad q(ip.get_ip());
	out <<(int)q.c[0]<<'.'
		<<(int)q.c[1]<<'.'
		<<(int)q.c[2]<<'.'
		<<(int)q.c[3]
		<<':'
		<<ip.get_port();
	return out;
}
