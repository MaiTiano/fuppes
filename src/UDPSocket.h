/***************************************************************************
 *            UDPSocket.h
 *
 *  Copyright  2005  Ulrich Völkel
 *  mail@ulrich-voelkel.de
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
 
#ifndef _UDPSOCKET_H
#define _UDPSOCKET_H

#include "win32.h"

#ifndef WIN32
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#endif

#include <string>

#include "Message.h"

class CUDPSocket;

class IUDPSocket
{
	public:
		virtual void OnUDPSocketReceive(CUDPSocket*, CMessage*) = 0;
};

class CUDPSocket
{
	public:
		CUDPSocket();
		~CUDPSocket();
	
		void send_multicast(std::string a_message);
	
		void begin_receive();
		void setup_socket(bool do_multicast);
	  void teardown_socket();
	  void setup_random_port();
	
		upnpSocket get_socket_fd();
	
		//void set_cb_on_receive(void (*)(message*, void*), void*);	
	  void SetReceiveHandler(IUDPSocket*);
	  void call_on_receive(CMessage*);
	
		int  get_port();
	  std::string get_ip();
	  sockaddr_in get_local_ep();
	
	private:
		//int       sock;						// socket descriptor
	  //pthread_t receive_thread; //
				
		upnpSocket       sock;						// socket descriptor
  	upnpThread receive_thread;			//
				
	  int port;
		bool      is_multicast;   //

	
	  sockaddr_in local_ep;			// local end point
	
	  IUDPSocket* m_pReceiveHandler;
	  void*   cb_receive_handler;
	  // events
	  void (*cb_on_receive)(CMessage*, void*);
	
};

#endif /* _UDPSOCKET_H */
