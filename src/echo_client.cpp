#include <iostream>
#include "boost/asio.hpp"
#include "boost/array.hpp"

using boost::asio::ip::tcp;

void client_run(){
    try {
      boost::asio::io_service io_service;

      tcp::endpoint endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 13);
      tcp::socket socket(io_service);
      socket.connect(endpoint);
      io_service.run();
      for (;;)
      {
          boost::array<char, 128> buf;
          boost::system::error_code error;
          std::string line;
          std::getline(std::cin, line);
          socket.send(boost::asio::buffer(line.data(), line.size()));
          size_t size = socket.receive(boost::asio::buffer(buf));
          std::cout << std::string{ buf.data(), size } << std::endl;
      }
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}

int main()
{
    client_run();
    return 0;
}