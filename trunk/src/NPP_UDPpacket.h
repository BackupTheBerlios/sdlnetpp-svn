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
//
// NOTE: channels are documented in SDL_net
////////////////////////////////////////////////////////////////////////////////

#ifndef NPP_UDPpacket_h
#define NPP_UDPpacket_h

#include "NPP_IPaddress.h"
#include <vector>

namespace NPP
{

class UDPsocket;
class IPaddress;
	
class UDPpacket
{
	// the real deal for SDL_net
	::UDPpacket *packet;

	// cache address for later packet creation
	::IPaddress address;

	// cache channel for later packet creation
	int channel;

	Uint32 at_tick;

public:
	// ctor to take a real SDL_net UDPpacket or nothing
	UDPpacket(::UDPpacket *packet=0);

	// copy ctor
	UDPpacket(const UDPpacket &src);

	// ctor for allocating a length of data
	// sets data if data pointer passed in is not NULL
	// sets ipaddress if passed in, defaults to broadcast
	// sets channel if passed in, defaults to -1 (no channel)
	UDPpacket(Uint16 len, void *data=0, 
			const NPP::IPaddress &ip_and_port=NPP::IPaddress::Broadcast, int channel=-1);

	// dtor
	// frees packet
	~UDPpacket();

	// get the current channel setting
	int get_channel() const { return packet?packet->channel:-2; }

	// set the channel
	void set_channel(int channel);

	// set the destination port
	void set_port(Uint16 port);
	
	// get the data pointer
	void *get_data() const { return packet?packet->data:0; }

	// set the data and size starting at offset
	void set_data(const void *data, Uint16 len, Uint16 offset=0);
	void append(const void *data, Uint16 len) {
		set_data(data,len,packet?packet->len:0);
	}

	// get the size of the data
	Uint16 get_len() const { return packet?packet->len:-1; }

	// resize packet to be of size len, maxlen is also set
	// preserves the data that still fits in len
	void set_len(Uint16 len);

	// get the maximum size allowed in this packet
	// usually same as len, unless you built your own packet or resized it
	Uint16 get_maxlen() const { return packet?packet->maxlen:-1; }

	// resize data to be size len, also set len if maxlen is smaller
	// preserves the data that still fits in len
	void set_maxlen(Uint16 len);

	// get the sent status of the packet
	// number of destinations successfully sent to
	// only useful after a UDPsocket::send
	int get_status() const { return packet?packet->status:-1; }

	// get the address of the source/destination of the packet
	NPP::IPaddress get_address() const;

	// set the address of the destination of the packet
	// if not 0.0.0.0 then channel is set to -1
	void set_address(const NPP::IPaddress &ip_and_port);

	// set at_tick
	void set_at_tick(Uint32 tick=::SDL_GetTicks()) { at_tick=tick; }
	// get at_tick
	Uint32 get_at_tick() { return at_tick; }
	
	// direct data array access
	// throws out_of_range if you go beyond [0..maxlen-1]
	unsigned char &operator[](int i) throw(std::out_of_range);
	
// friends
	friend class UDPsocket;
};

typedef std::vector<UDPpacket> UDPpacketV;

};

// output raw bytes
std::ostream &operator<<(std::ostream &out, const NPP::UDPpacket &packet);

#endif
