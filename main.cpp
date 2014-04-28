/*
 * =====================================================================================
 *
 *       Filename:  lab_a.cpp
 *
 *    Description:  Uses BOOST.Asio to create a cross-platform
 *                  implementation of UDP/TCP/IP networking.
 *                  The purpose is to generate simple packets, as designated
 *                  by command-line arguments.
 *
 *        Version:  1.0
 *        Created:  04/13/2014 01:36:31 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jordan Hanna, jhanna01@uw.edu
 *        Company:  University of Washington
 *
 * =====================================================================================
 */

#include "main.hpp"
#include <iostream>
#include <exception>
#include <boost/array.hpp>
#include <boost/asio.hpp>

const int ARG_COUNT = 3;
const int MAX_PACKETS = 25;
const int LOWEST_PORT = 1024;
const int HIGHEST_PORT = 65000;

void runTCPServer ( UserInput input )
{
    try
    {
        // Create io service
        boost::asio::io_service io_service;

        std::cout << "Starting TCP server..." << std::endl;

        // acceptor used to accept calls from clients
        // pass to the acceptor the end point specifying protocol (TCPv4), and port.
        boost::asio::ip::tcp::acceptor acceptor(io_service,
            boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), input.port));

        std::cout << "Server ready" << std::endl;

        // Run server
        for(;;)
        {
            // Create a new TCP/IP socket on existing ASIO I/O service.
            boost::asio::ip::tcp::socket socket(io_service);

            // Wait for client connection
            acceptor.accept(socket);

            // Write a message to the client via socket
            std::string message("Hello from ASIO");
            boost::asio::write(socket, boost::asio::buffer(message));
        }
    }
    catch(std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    std::cout << "Goodbye!" << std::endl;

    return ;
}

void runUDPServer( UserInput input )
{
    try
    {
        // Create io service
        boost::asio::io_service io_service;

        std::cout << "Starting UDP server..." << std::endl;

        // create a socket object to receive requests on udp port input.port
        boost::asio::ip::udp::socket socket(io_service, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), input.port));

        std::cout << "Server ready" << std::endl;

        // Run server
        for(;;)
        {
            // Wait for a client to initiate contact with us.
            // The remote_endpoint object will be populated by boost::asio::ip::udp::socket::receive_from().
            boost::array<char, 1> recv_buf;
            boost::asio::ip::udp::endpoint remote_endpoint;

            // Receive client query
            boost::system::error_code error;
            size_t len = socket.receive_from(boost::asio::buffer(recv_buf), remote_endpoint, 0, error);
            std::cout << "Query from client " << remote_endpoint.address() << " of size " << remote_endpoint.size() << std::endl;
            std::cout.write(recv_buf.data(), len) << std::endl;

            if (error && error != boost::asio::error::message_size)
                throw boost::system::system_error(error);

            std::string message("Hello from UDP ASIO server");

            // Send the response
            boost::system::error_code ignored_error;
            socket.send_to(boost::asio::buffer(message), remote_endpoint, 0, ignored_error);
        }
    }
    catch(std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    std::cout << "Goodbye!" << std::endl;

    return ;
}

void runTCPClient()
{

}

void runUDPClient(std::string portStr)
{
    std::string host("localhost");
    try
    {
        boost::asio::io_service io_service;

        // resolver object to find the correct remote endpoint to use based on the host and service names
        boost::asio::ip::udp::resolver resolver(io_service);

        // Make query to the UDP (ipv4) host server
        boost::asio::ip::udp::resolver::query query(boost::asio::ip::udp::v4(), host, portStr);

        // Will return at least one endpoint in the list if it does not fail.
        // This means it is safe to dereference the return value directly.
        boost::asio::ip::udp::endpoint receiver_endpoint = *resolver.resolve(query);

        // Using datagram socket, not stream socket.
        boost::asio::ip::udp::socket socket(io_service);

        // initiate contact with the remote endpoint
        socket.open(boost::asio::ip::udp::v4());
        boost::array<char, 1> send_buf  = { 0 };
        socket.send_to(boost::asio::buffer(send_buf), receiver_endpoint);

        // Prepare to receive server response
        boost::array<char, 128> recv_buf;
        boost::asio::ip::udp::endpoint sender_endpoint;

        // Initialize our endpoint
        size_t len = socket.receive_from(
        boost::asio::buffer(recv_buf), sender_endpoint);

        std::cout.write(recv_buf.data(), len) << std::endl;
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}


void runClient( UserInput input )
{
    if (input.protocol == "UDP")
    {
        runUDPClient(input.portStr);
    }
    else if (input.protocol == "TCP")
    {
        runTCPClient();
    }
    return;
}

void runServer( UserInput input )
{
    if (input.protocol == "UDP")
    {
        runUDPServer(input);
    }
    else if (input.protocol == "TCP")
    {
        runTCPServer(input);
    }
    return;
}

/**
* Usage: netLabA <protocol> <port> <num of packets>
*/
bool parseArgs ( int argc, char *argv[], UserInput *input )
{
    bool result = true;
    if (argc - 1 == ARG_COUNT)
    {
        // arg 1: server or client
        int arg1 = std::stoi(argv[1]);
        if (arg1 == 0 || arg1 == 1)
        {
            input->servOrClient = arg1;
        }
        else
        {
            std::cout << "Invalid client server choice.\nUsage: netLabA <client (0) or server(1)> <protocol> <port>" << std::endl;
            result = false;
        }
        // arg 2: protocol
        if (std::string(argv[2]) == "TCP" || std::string(argv[2]) == "UDP")
        {
            input->protocol = std::string(argv[2]);
        }
        else
        {
            std::cout << "Invalid protocol.\nUsage: netLabA <client (0) or server(1)> <protocol> <port>" << std::endl;
            result = false;
        }
        // arg 3: port
        int arg3 = std::stoi(argv[3]);
        if (arg3 > LOWEST_PORT && arg3 < HIGHEST_PORT )
        {
            input->port = arg3;
            input->portStr = argv[3];
        }
        else
        {
            std::cout << "Invalid port, must be between " << LOWEST_PORT << " and " << HIGHEST_PORT << std::endl;
            std::cout << "Usage: netLabA <client (0) or server(1)> <protocol> <port>" << std::endl;
            result = false;
        }

    }
    else
    {
        std::cout << "Usage: netLabA <client (0) or server(1)> <protocol> <port>" << std::endl;
        result = false;
    }

    return result;
}



int main ( int argc, char *argv[] )
{
    UserInput input;
    if (parseArgs(argc, argv, &input))
    {
        if(input.servOrClient == 1)
        {
            runServer(input);
        }
        else if(input.servOrClient == 0)
        {
            runClient(input);
        }
    }
    else
    {
        return 1;
    }

    return 0;
}
