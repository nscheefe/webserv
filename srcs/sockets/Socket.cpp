/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mreidenb <mreidenb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/20 16:46:20 by mreidenb          #+#    #+#             */
/*   Updated: 2024/05/20 18:24:50 by mreidenb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"


/**
 * Constructs a new Socket object.
 *
 * This constructor creates a new socket with the specified domain, type, and protocol,
 * and binds it to the specified interface and port. If the socket creation fails,
 * the program will print an error message and exit.
 *
 * @param domain The domain for the socket (e.g., AF_INET for IPv4, AF_INET6 for IPv6).
 * @param type The type of the socket (e.g., SOCK_STREAM for TCP, SOCK_DGRAM for UDP).
 * @param protocol The protocol to be used by the socket. Typically set to 0 to automatically choose the appropriate protocol based on the socket type.
 * @param interface The interface to bind the socket to, specified as an IP address in network byte order.
 * @param port The port number to bind the socket to.
 */
ServerSocket::ServerSocket(int domain, int type, int protocol, u_long interface, int port)
{
	if ((this->socket_fd = socket(domain, type, protocol)) == 0) {
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	this->address.sin_family = domain;
	this->address.sin_addr.s_addr = interface;
	this->address.sin_port = htons(port);
	bind_socket();
}

/**
 * Destroys the Socket object.
 *
 * This destructor closes the socket file descriptor.
 */
ASocket::~ASocket()
{
	close(this->socket_fd);
}

/**
 * Binds the socket to the specified interface and port.
 *
 * This function binds the socket to the interface and port specified in the constructor.
 * If the bind operation fails, the program will print an error message and exit.
 */
void ServerSocket::bind_socket()
{
	if (::bind(this->socket_fd, (struct sockaddr *)&this->address, sizeof(this->address)) < 0) {
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
}

//getter

/**
 * Returns the file descriptor of the socket.
 *
 * @return The file descriptor of the socket.
 */
int ASocket::getFD()
{
	return this->socket_fd;
}

/**
 * Returns the address structure of the socket.
 *
 * @return The address structure of the socket.
 */
struct sockaddr_in ServerSocket::getAddress()
{
	return this->address;
}
