#include <memory>
#include <future>
#include <iostream>
#include <experimental/resumable>

#include "boost/asio.hpp"
#include "boost/array.hpp"

using namespace boost::asio;
class Connection :public std::enable_shared_from_this<Connection>
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
        if (auto size = __await async_read())
        {
            std::string read_str = std::string(buf_.data(), size);
            std::cout << "got size:" << size << ", msg:" << read_str << std::endl;
            if (read_str == "exit")
                return 0;

            return __await async_write(size);
        }
        return 0;
    }

    std::future<size_t> async_read()
    {
        return std::async([&] {
            buf_.assign(0);
            return socket_.receive(buffer(buf_));
        });
    }
    std::future<size_t> async_write(size_t size)
    {
        return std::async([&] {
            return socket_.send(buffer(buf_, size));
        });
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
        auto connection = Connection::create(acceptor_.get_io_service());
        acceptor_.async_accept(connection->socket(),
            std::bind(&Server::handle_accept, this, std::placeholders::_1, connection));
    }
    void handle_accept(const boost::system::error_code& ec, Connection::Ptr connection)
    {
        if (!ec)
        {
            size_t size;
            while (size = connection->run().get());
            std::cout << "end." << size << std::endl;
        }
        else
        {
            std::cout << "error:" << ec << std::endl;
        }
        start_accept();
    }
private:
    ip::tcp::acceptor acceptor_;
};