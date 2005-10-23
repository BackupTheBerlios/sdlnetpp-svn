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
// UDPsocket : UDP socket server and client
////////////////////////////////////////////////////////////////////////////////

#include "NPP_UDPsocket.h"
#include "NPP_SocketSet.h"
#include <iostream>

namespace NPP
{

using namespace std;
	
UDPsocket::UDPsocket(::UDPsocket &sock_) throw(std::logic_error)
	: NPP::Socket(UDP)
	, sock(sock_)
	, run_thread(0)
	, tid(0)
{
	if(!sock)
		throw std::logic_error("null sock passed to ");
	sock_=0; //prevent further usage...sorta
}

UDPsocket::UDPsocket(Uint16 port) throw(std::runtime_error)
	: NPP::Socket(UDP)
	, run_thread(0)
	, tid(0)
{
	sock=SDLNet_UDP_Open(port);
	if(!sock)
		throw std::runtime_error(SDL_GetError());
}

UDPsocket::~UDPsocket()
{
	SDLNet_UDP_Close(sock);
}

bool UDPsocket::valid()
{
	return poll()>-1;
}

int UDPsocket::bind(const NPP::IPaddress ip_and_port, int channel)
{
	if(!valid())
		return -1;
	return SDLNet_UDP_Bind(sock, channel,
			const_cast< ::IPaddress*>(&ip_and_port.ipaddr ));
}

void UDPsocket::unbind(int channel)
{
	if(!valid())
		return;
	if(channel==-1)
	{
		for(int i=0; i<SDLNET_MAX_UDPCHANNELS; i++)
			::SDLNet_UDP_Unbind(sock, i);
		return;
	}
	SDLNet_UDP_Unbind(sock, channel);
}

NPP::IPaddress UDPsocket::get_peer_address(int channel)
{
	if(!valid())
		return NPP::IPaddress::None;
	::IPaddress *ipaddr=::SDLNet_UDP_GetPeerAddress(sock, channel);
	if(ipaddr)
		return *ipaddr;
	return NPP::IPaddress::Any;
}

int UDPsocket::poll(Uint32 timeout)
{
	if(!sock)
		return -1;
	NPP::SocketSet set;
	set.add(*this);
	if(timeout==(Uint16)~0)
	{
		int i;
		while((i=set.check(timeout))==0);
		return i;
	}
	return set.check(timeout);
}

int UDPsocket::send(const NPP::UDPpacket &packet, int channel)
{
	if(!valid())
		return -1;
	return ::SDLNet_UDP_Send(sock, channel, packet.packet);
}

int UDPsocket::recv(NPP::UDPpacket &packet, Uint16 maxlen)
{
	if(!valid())
		return -1;
	packet.set_maxlen(maxlen);
	int n=::SDLNet_UDP_Recv(sock,packet.packet);
	if(n<0)
	{
		packet.set_maxlen(0);
		return -1;
	}
	if(!n)
	{
		packet.set_maxlen(0);
		return 0;
	}
	packet.set_address(packet.packet->address);
	packet.set_channel(packet.packet->channel);
	packet.set_maxlen(packet.packet->len);
	packet.set_at_tick();
	return packet.get_len();
}

int UDPsocket::send(const NPP::UDPpacketV &packets)
{
	if(!valid())
		return -1;
	int n=0;
	for(unsigned int i=0; i<packets.size(); ++i)
		n+=send(packets[i]);
	return n;
}

int UDPsocket::recv(NPP::UDPpacketV &packets)
{
	if(!valid())
		return -1;
	packets.resize(1);
	while(recv(packets[packets.size()-1])>0)
		packets.resize(packets.size()+1);
	packets.resize(packets.size()-1);
	return packets.size();
}

::SDL_Thread *UDPsocket::thread_start(int code)
{
	run_thread=true;
	user_event_code=code;
	if(!tid)
		tid=SDL_CreateThread(thread_main,reinterpret_cast<void*>(this));
	return tid;
}

int UDPsocket::thread_main(void *this_udp_socket)
{
	UDPsocket &s=*reinterpret_cast<UDPsocket*>(this_udp_socket);
	NPP::UDPpacket *p=0;
	while(s.run_thread)
	{
		int n=s.poll(1000);
		if(n<0)
		{
			s.run_thread=false;
			return -1;
		}
		if(!n)
			continue;
		do
		{
			if(!p)
				p=new UDPpacket;
			n=s.recv(*p);
			if(n<0)
			{
				delete p;
				s.run_thread=false;
				return -1;
			}
			if(n)
			{
				SDL_Event e;
				e.type=SDL_USEREVENT;
				e.user.code=s.user_event_code;
				e.user.data1=p;
				e.user.data2=&s;
				SDL_PushEvent(&e);
				p=0;
			}
		} while(n && s.run_thread);
	}
	delete p;
	s.run_thread=false;
	s.tid=0;
	return 1;
}

int UDPsocket::thread_stop()
{
	if(!tid) return 2;
	::SDL_Thread *t=tid;
	if(run_thread) // try to let it go away...
	{
		run_thread=false;
		for(int i=105; i && tid; --i)
			SDL_Delay(10);
	}
	if(!tid)
	{
		int i=0;
		SDL_WaitThread(t,&i); // nice!
		return i;
	}
	else
		SDL_KillThread(tid); // naughty!
	tid=0;
	return 0;
}

};
