/*
 *
 * UpdSockets.h
 *
 *  Created on: Dec 9, 2012
 *      Author: Stephan Hesse
 */

#ifndef UPDSOCKETS_H_
#define UPDSOCKETS_H_

//#include <Poco/Net/DatagramSocket.h>

#include <string> 

class UdpReceiver {
public:

	UdpReceiver(std::string host, unsigned short port);

	~UdpReceiver();

	void setCurrentEndpoint(std::string host, unsigned short port, bool filter=true);

//	const Poco::Net::SocketAddress & getCurrentEndpoint() const ;

	int receivePacket(void * buffer,
					int length,
					int flags = 0) ;
private:

	bool filtering;
//	Poco::Net::SocketAddress filteredSender;
//	Poco::Net::DatagramSocket socket;

};

class UdpSender {

public:

	UdpSender(std::string host, unsigned short port);

	~UdpSender();

//	const Poco::Net::DatagramSocket & getLocal() ;

	void setCurrentEndpoint(std::string host, unsigned short port);

//	const Poco::Net::SocketAddress & getCurrentEndpoint() const;

	int sendPacketBuffer(unsigned char* bytes, size_t len);

private:

//	Poco::Net::SocketAddress destination;
//	Poco::Net::DatagramSocket socket;

};
#endif /* UPDSOCKETS_H_ */
