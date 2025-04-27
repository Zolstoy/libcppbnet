#pragma once

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast.hpp>

class server_stub
{
   private:
    boost::asio::ssl::context ctx;

   public:
    server_stub();
    void start(std::stop_token stoken);

   private:
    void on_accept(boost::beast::error_code ec, boost::asio::ip::tcp::socket socket);
};
