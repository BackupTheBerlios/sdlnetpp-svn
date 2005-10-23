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
//
// NOTE: channels are documented in SDL_net
////////////////////////////////////////////////////////////////////////////////

#ifndef NPP_UDPsocket_h
#define NPP_UDPsocket_h

#include "NPP_Socket.h"
#include "NPP_UDPpacket.h"
#include "SDL_thread.h"

namespace NPP
{

class UDPpacket;
class IPaddress;

class UDPsocket : public NPP::Socket
{
	// the read deal for SDL_net
	::UDPsocket sock;

	// keep running thread?
	bool run_thread;

	// for any active threaded reader (see thread_start)
	::SDL_Thread *tid;

	// code for SDL_UserEvent::code
	int user_event_code;

	static const Uint16 UDP_MAXLEN=0xFFFF;

	// get my generic socket
	virtual NPP::Socket::GenericSocket get_generic_sock() const { return sock; }

	// thread runner
	static int thread_main(void *this_udp_socket);
	
public:
	// ctor to take a SDL_net UDPsocket
	// sock is 0 after finishing
	UDPsocket(::UDPsocket &sock) throw(std::logic_error);

	// ctor for a port
	// defaults to any free port the OS chooses
	// get the actual port with get_peer_address()
	UDPsocket(Uint16 port=0) throw(std::runtime_error);

	// dtor
	// closes the socket
	virtual ~UDPsocket();
	
	// bind a destination address and port to a channel
	int bind(const NPP::IPaddress ip_and_port, int channel=-1);

	// unbind all addresses from a channel
	// defaults to all channels
	void unbind(int channel=-1);

	// get address of a channel
	// -1 is self (0.0.0.0 and whatever port)
	NPP::IPaddress get_peer_address(int channel=-1);

	// run a single SocketSet::check on this socket
	// returns:
	// 		-1 = socket error (use SDL_GetError)
	// 		 0 = nothing came in yet
	// 		 1 = data waiting for recv
	int poll(Uint32 timeout=0);
	
	// send a UDPpacket
	int send(const NPP::UDPpacket &packet, int channel=-1);

	// receive a UDPpacket
	// it's size will be shrunk to the actual size of the packet received
	// *non-blocking* 
	// returns:
	// 		>0 = length of packet recv'd
	// 		 0 = no packet recv'd
	// 		-1 = error
	int recv(NPP::UDPpacket &packet, Uint16 maxlen=UDP_MAXLEN);

	// send a vector of UDPpackets
	// returns the number of successful packets to destinations sent to
	int send(const NPP::UDPpacketV &packets);

	// receive as many packets as are ready into a vector of UDPpackets
	// returns the number of packets received or -1 on errors
	int recv(NPP::UDPpacketV &packets);

	// test for a valid socket
	// sock != NULL and poll() > -1
	virtual bool valid();

	// put the socket into a threaded reader -> user event mode
	// user_event_code...pick one else it's 0
	::SDL_Thread *thread_start(int user_event_code=0);

	// stop the threaded reader, if any.
	// returns:
	// 		-1 = failed to read socket for some reason (SDL_WaitThread)
	// 		 0 = ended via SDL_KillThread
	// 		 1 = ended via SDL_WaitThread
	// 		 2 = no thread to kill...ok
	int thread_stop();
};

};

#endif
