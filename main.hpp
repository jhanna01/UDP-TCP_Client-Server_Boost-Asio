
#ifndef SERVER_H_INCLUDED
#define SERVER_H_INCLUDED

#include <string>

struct UserInput
{
    std::string protocol;
    std::string portStr;
    int port;
    int servOrClient;
};

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  runServer
 *  Description:
 * =====================================================================================
 */
void runTCPServer ( UserInput input );
void runUDPServer ( UserInput input );
/*
 * ===  FUNCTION  ======================================================================
 *         Name:  parseArgs
 *  Description:  Parses command-line arguments.
 *  Usage: lab_a <protocol> <num of packets>
 * =====================================================================================
 */

bool parseArgs ( int argc, char *argv[], UserInput input );

#endif // SERVER_H_INCLUDED
