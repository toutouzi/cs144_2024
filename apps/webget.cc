#include "socket.hh"

#include <cstdlib>
#include <iostream>
#include <span>
#include <string>

using namespace std;

void get_URL( const string& host, const string& path )
{
  // cerr << "Function called: get_URL(" << host << ", " << path << ")\n";
  // cerr << "Warning: get_URL() has not been implemented yet.\n";
  TCPSocket sock1;
  sock1.connect( Address( host, "http" ) );

  sock1.write( "GET " + path + " HTTP/1.1\r\n" );
  sock1.write( "Host: " + host + "\r\n" );
  sock1.write( "Connection: close\r\n" );
  sock1.write( "\r\n" );
  // sock1.shutdown( SHUT_WR );

  std::string buffer;
  while ( !sock1.eof() ) {
    sock1.read( buffer );
    std::cout << buffer;
    buffer.clear();
  }

  sock1.close();

  // TCPSocket sck {};
  // sck.connect( Address( host, "http" ) );
  // sck.write( format( "GET {} HTTP/1.1\r\n"
  //                    "Host: {}\r\n"
  //                    "Connection: close\r\n"
  //                    "\r\n",
  //                    path,
  //                    host ) );
  // sck.shutdown( SHUT_WR );
  // std::string buf;
  // while ( !sck.eof() ) {
  //   sck.read( buf );
  //   std::cout << buf;
  //   buf.clear();
  // }
  // sck.close();
}

int main( int argc, char* argv[] )
{
  try {
    if ( argc <= 0 ) {
      abort(); // For sticklers: don't try to access argv[0] if argc <= 0.
    }

    auto args = span( argv, argc );

    // The program takes two command-line arguments: the hostname and "path" part of the URL.
    // Print the usage message unless there are these two arguments (plus the program name
    // itself, so arg count = 3 in total).
    if ( argc != 3 ) {
      cerr << "Usage: " << args.front() << " HOST PATH\n";
      cerr << "\tExample: " << args.front() << " stanford.edu /class/cs144\n";
      return EXIT_FAILURE;
    }

    // Get the command-line arguments.
    const string host { args[1] };
    const string path { args[2] };

    // Call the student-written function.
    get_URL( host, path );
  } catch ( const exception& e ) {
    cerr << e.what() << "\n";
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
