/*
 * UdpSockets.cpp
 *
 *  Created on: Dec 9, 2012
 *      Author: Stephan Hesse
 */

#if 0

#include <asl/framework/UdpSockets.h>

UdpReceiver::UdpReceiver(std::string host, unsigned short port)
:socket(Poco::Net::SocketAddress(host, port))
,filtering(false)
{
	socket.bind(Poco::Net::SocketAddress(host, port));

}

UdpReceiver::~UdpReceiver() {
	socket.close();
}

void UdpReceiver::setCurrentEndpoint(std::string host, unsigned short port, bool filter) {

	filtering = filter;
	if(filtering) {
		filteredSender = Poco::Net::SocketAddress(host, port);
	}
}

const Poco::Net::SocketAddress & UdpReceiver::getCurrentEndpoint() const {
	return filteredSender;
}

int UdpReceiver::receivePacket(void * buffer,
    int length,
    int flags) {

	if(filtering) {
		return socket.receiveFrom(buffer, length, filteredSender, flags);
	} else {
		return socket.receiveBytes(buffer, length, flags);
	}
}

UdpSender::UdpSender(std::string host, unsigned short port)
:socket(Poco::Net::SocketAddress(host, port))
{

}

UdpSender::~UdpSender() {
	socket.close();
}

const Poco::Net::DatagramSocket & UdpSender::getLocal() {
	return socket;
}

void UdpSender::setCurrentEndpoint(std::string host, unsigned short port) {
	destination = Poco::Net::SocketAddress(host, port);
	socket.connect(destination);
}

const Poco::Net::SocketAddress & UdpSender::getCurrentEndpoint() const {
	return destination;
}

int UdpSender::sendPacketBuffer(unsigned char* bytes, size_t len) {
	return socket.sendBytes(bytes, len);
}

#endif
