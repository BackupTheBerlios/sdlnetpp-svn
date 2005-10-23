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
// UDPpacket : UDP packet and packet vectors
////////////////////////////////////////////////////////////////////////////////

#include "NPP_UDPpacket.h"
#include <algorithm>
#include <iostream>

namespace NPP
{

using namespace std;
	
UDPpacket::UDPpacket(::UDPpacket *packet_)
	: packet(packet_)
{
	if(packet)
	{
		address=packet->address;
		channel=packet->channel;
	}
	else
	{
		address=::IPaddress();
		channel=-1;
	}
	// take ownership of incoming packet...sorta
	packet_=0;
}

UDPpacket::UDPpacket(const UDPpacket &src)
	: packet(0)
{
	if(src.packet)
	{
		set_data(src.get_data(),src.get_len());
		packet->status=0;
	}
	set_channel(src.get_channel());
	set_address(src.get_address());
}

UDPpacket::UDPpacket(Uint16 len, void *data,
		const NPP::IPaddress &ip_and_port, int channel)
	: packet(0)
{
	set_address(ip_and_port);
	set_channel(channel);
	set_data(data,len);
}

UDPpacket::~UDPpacket()
{
	if(packet)
		SDLNet_FreePacket(packet);
	packet=0;
}

void UDPpacket::set_channel(int channel_)
{
	channel=channel_;
	if(packet)
		packet->channel=channel;
}

void UDPpacket::set_port(Uint16 port)
{
	SDLNet_Write16(port,&address.port);
	if(packet)
		SDLNet_Write16(port,&packet->address.port);
}

void UDPpacket::set_data(const void *data, Uint16 len, Uint16 offset)
{
	set_len(len+offset);
	if(packet && data && len)
	{
		const Uint8 *bytes=(const Uint8*)data;
		std::copy(bytes, bytes+len, packet->data+offset);
	}
}

void UDPpacket::set_len(Uint16 len)
{
	set_maxlen(len);
	if(packet)
		packet->len=len;
}

void UDPpacket::set_maxlen(Uint16 len)
{
	if(!len)
	{
		if(packet)
			SDLNet_FreePacket(packet);
		packet=0;
		return;
	}
	char *tmp=0;
	int minlen=!packet
				?0
				:len<packet->len
					?len
					:packet->len;
	if(packet)
	{
		if(packet->data && minlen)
		{
			tmp=new char[minlen];
			std::copy(packet->data, packet->data+minlen, tmp);
		}
		SDLNet_ResizePacket(packet,len);
	}
	else
	{
		packet=SDLNet_AllocPacket(len);
		set_channel(channel);
		set_address(address);
		packet->len=0;
	}
	if(packet->len>len)
		packet->len=len;
	if(tmp)
	{
		std::copy(tmp, tmp+minlen, packet->data);
		set_address(address);
		delete [] tmp;
	}
}

NPP::IPaddress UDPpacket::get_address() const
{
	return address;
}

void UDPpacket::set_address(const NPP::IPaddress &ip_and_port)
{
	address=ip_and_port.ipaddr;
	if(packet)
		packet->address=address;
}

unsigned char &UDPpacket::operator[](int i) throw(out_of_range)
{
	if(!packet || i<0 || i>=packet->maxlen)
		throw out_of_range("accessing out of bounds in UDPpacket, or packet is NULL");
	return ((unsigned char*)packet->data)[i];
}

};

std::ostream &operator<<(std::ostream &out, const NPP::UDPpacket &packet)
{
	out.write((const char*)packet.get_data(),packet.get_len());
	return out;
}
