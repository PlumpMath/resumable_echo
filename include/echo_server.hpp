#include <memory>
#include <future>
#include <iostream>
#include <experimental/resumable>
#include <algorithm>

#include "boost/asio.hpp"
#include "boost/array.hpp"
#include "boost/asio/use_future.hpp"

using namespace boost::asio;
class Connection: public std::enable_shared_from_this<Connection>
{
public:
    using Ptr = std::shared_ptr<Connection>;
    static Ptr create(io_service& io)
    {
        return std::make_shared<Connection>(io);
    }

    std::future<size_t> run()
    {
        std::cout << "run" << std::endl;
        try 
        {
            while (true)
            {
              auto size = await socket_.async_read_some(buffer(buf_), use_future);
              std::string read_str = std::string(buf_.data(), size);
              std::cout << "got size:" << size << ", msg:" << read_str << std::endl;
              if (read_str == "exit")
                  return 0;
              await socket_.async_write_some(buffer(buf_, size), use_future);
            }
            return 0;
        }
        catch (const std::exception& e)
        {
            std::cerr << "1:" << e.what() << std::endl;
        }
    }

    ip::tcp::socket& socket()
    {
        return socket_;
    }
    Connection(io_service& io_service) : socket_(io_service), io_(io_service)
    {}
private:
    io_service& io_;
    ip::tcp::socket socket_;
    boost::array<char, 128> buf_;
};

class Server
{
public:
    Server(io_service& io_service) :
        acceptor_(io_service, ip::tcp::endpoint(ip::tcp::v4(), 13)) 
    {
    }

    void start_accept()
    {
        std::cout << "start accept" << std::endl;
        auto connection = Connection::create(acceptor_.get_io_service());
        acceptor_.async_accept(connection->socket(),
            std::bind(&Server::handle_accept, this, std::placeholders::_1, connection));

        //await acceptor_.async_accept(connection->socket(), use_future);
        //sessions_.push_back(connection);
        //connection->run();
        //start_accept();
    }
    void handle_accept(const boost::system::error_code& ec, Connection::Ptr connection)
    {
        try
        {
            if (!ec)
            {
                sessions_.push_back(connection);
                connection->run();
            }
            else
            {
                std::cout << "error:" << ec << std::endl;
            }
            start_accept();
        }
        catch (const std::exception& e)
        {
          std::cerr << "2:" << e.what() << std::endl;
          sessions_.erase(
              std::remove(begin(sessions_), end(sessions_), connection), end(sessions_));
        }
    }
private:
    ip::tcp::acceptor acceptor_;
    std::vector<Connection::Ptr> sessions_;
};