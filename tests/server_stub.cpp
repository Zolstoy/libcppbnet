#include "server_stub.hpp"

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast.hpp>
#include <chrono>

using namespace std::chrono_literals;
using namespace boost;
using namespace boost::beast;
using namespace boost::beast::http;
using namespace boost::asio;
using namespace boost::asio::ssl;
using tcp = boost::asio::ip::tcp;

std::string const cert =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIDMTCCAhmgAwIBAgIUPW2I5vQZWOxWMHqP1Pu73GfKvhUwDQYJKoZIhvcNAQEL"
    "BQAwHTELMAkGA1UEBhMCRkkxDjAMBgNVBAMMBXZhaGlkMB4XDTI0MTIwMTIwMzAw"
    "NFoXDTI1MTIwMTIwMzAwNFowHTELMAkGA1UEBhMCRkkxDjAMBgNVBAMMBXZhaGlk"
    "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAtcLRSlxRbbOT4m1vKeWm"
    "HRxSpr6YdHT4TlJYcQnvNg7NQBoSQFLTY/c9vDwnwpC3nDc+I3VauZYb44Iocnht"
    "BK7AQPyscjM6dwVu0mxFIgc0i2t5+yrNs8n5jWzHsMu7ZgMc9RmRBzgadw/9VHcH"
    "RyFJt1wYIJI48PjNW/IfzeGYCNEjTdWYifBdZKt4gOrpcEvHzlsjebcVdXTrS8sI"
    "82zLKCGfy07JqDxHhMb4uIb/J/SKNkng2Dpr9Ythxfn5dD4BKuaKrEnxjLxBKX3J"
    "SUa5+bs3lP/LH5nz/cogBV6t6BIoJ7p//jgjSalCkXvGnKG/+asid1JJ0z5ZuM/R"
    "KwIDAQABo2kwZzAfBgNVHSMEGDAWgBQ6XXVXE9iMux7aLuk0hcRz42f+JTAJBgNV"
    "HRMEAjAAMBoGA1UdEQQTMBGCCWxvY2FsaG9zdIcEfwAAATAdBgNVHQ4EFgQU6Yab"
    "dvv0NBb/mYRdbOzN3T+gUcYwDQYJKoZIhvcNAQELBQADggEBAFLoifH57rdSzLV/"
    "ZuOGEKvn/KgAcM+p+Sj7vujwe+vntMMBSjChm98YsOPR26j0aweKfHlnrbPuerk1"
    "dvU34pe0v0TDzLIpJuIkfZe5MMx3WjvhwTPOWlAqxaMMxAD+95I6KChP4lV9xqLv"
    "iPgSDSODElS/qKb3kU4sA4m2CxmI6yCWW2tYsjoTkqrBmhjKql6UnBBrkb5K6tXm"
    "jcg0sq+u24j0Hzq9slk3Uxk3viqdN1X6p1sPCeAdO7Q2y6NBB8rTYu6klUQQRWL8"
    "NH4has89I4jp2ufcy1zY4ckN3uSZffG8S+v3jv/c9dmZoV7OO1CYnwvzgo01k9GD"
    "Vqi4i7M=\n"
    "-----END CERTIFICATE-----";

std::string const key =
    "-----BEGIN PRIVATE KEY-----\n"
    "MIIEvQIBADANBgkqhkiG9w0BAQEFAASCBKcwggSjAgEAAoIBAQC1wtFKXFFts5Pi"
    "bW8p5aYdHFKmvph0dPhOUlhxCe82Ds1AGhJAUtNj9z28PCfCkLecNz4jdVq5lhvj"
    "gihyeG0ErsBA/KxyMzp3BW7SbEUiBzSLa3n7Ks2zyfmNbMewy7tmAxz1GZEHOBp3"
    "D/1UdwdHIUm3XBggkjjw+M1b8h/N4ZgI0SNN1ZiJ8F1kq3iA6ulwS8fOWyN5txV1"
    "dOtLywjzbMsoIZ/LTsmoPEeExvi4hv8n9Io2SeDYOmv1i2HF+fl0PgEq5oqsSfGM"
    "vEEpfclJRrn5uzeU/8sfmfP9yiAFXq3oEignun/+OCNJqUKRe8acob/5qyJ3UknT"
    "Plm4z9ErAgMBAAECggEAHOKT/hxDuIpUUySPCPp89p1cqTEa6073cwL1GSm6AT5C"
    "8g/ynJRNEdLl1bc9nlb/Ru0ki+AHhfzL+9DgeqiWsqrO1MUS5qcrgGS1ou0f43N/"
    "rzRqUzcPL6ZGaWpDJd6KroCKJo1kleAdnJRG7xhnaK9qlqAlGXADapAvmpAU69PM"
    "MwpW9S96QvVHfPP7LXO/nvNzqLnrNysprHkSH6iV4ao37LEqzgUF0tABTk0Q67UJ"
    "O4XSToMAJ8GOBjYSKVK3PJm3saqTobff9Oz2HgUWUyr92kSESPhfNEVlMskmgvE3"
    "CcajxOxudxg94AAU7Es1UE5bMtY2e/Cs1088yzC3SQKBgQDvtYHI+4Kcur2ply0p"
    "QIBSSspJZ7fGT9/waK0EFlAyQ/qAaFH0Ilb6U2/L52TSR0EbSImQN7VxkUrosHym"
    "HahB6yHXkI2G8nDcmSdNjyiiC00+LWyKCtixE+bRCAuReZmypSk1Fz8GwYb3gaBR"
    "YcsWGsMeomFpL6q6yIgo43r8xQKBgQDCHR9fciT7zHTWAyPNlPLVzuJlvi164OC8"
    "GkHHxx+CybIDZVrUdUfYk80kxC+bvlUIaMs2D0MVUg2Hv8IbtMjEs+FV4vM/Df9J"
    "e9SWhOTWz25Jc7ZRYKVKc848l6TQd5JMU4JjeqmmVAza27l6Iu4TQb+r9GrZgBxX"
    "6NBj8vZVLwKBgFsW1iLRsGhubfQsBnVOlXSwBv6t8x/g6nAo1tZexErVmjOBcOMc"
    "yYCGhE0vuRhPC2aaweuTv9dQJu8VYcieLHogJ9QKkj1dk5XAfTbz17T8JnYiPMSY"
    "Ko/fyC5WqE63rrg8GtSZ6NFgaTFUiN9kEhBsSwkxG2MlQfOIkHU5PFshAoGBAL6c"
    "4GjWapDERdq9/JNs90STQmgMZxap6qVr1zp5Q20n6GFDTv0gKav3/1NiPyndrhxy"
    "41GzjPlLuLObzt1sGlZmGRlAogJCGXSsX6Zq21hBGxiPwvGISOeiblu7wYFgWU4Q"
    "FxLeqecF1BW5/Bl+YXCReMk/Wwk3rx14JeJv/ArLAoGAPwBXLX1HwQeHoFn4ImZV"
    "r0fUKkD4LzaAJ4gbEqzAQ8AD8vmqq+CBpu1YCLO6SFqHsFj1RUfk1ScVVD9tlL7E"
    "CI5ivNoxDpThvZhP6v42T7JQKK49YaGySE/k3y0wztfsk8qn6dAI6pwFMgtfsFFo"
    "RZb6vjD6zPWZElSkrwGczDM=\n"
    "-----END PRIVATE KEY-----";

server_stub::server_stub()
    : ctx{ssl::context::tlsv12}
{
    ctx.use_certificate_chain(boost::asio::buffer(cert.data(), cert.size()));

    ctx.use_private_key(boost::asio::buffer(key.data(), key.size()), boost::asio::ssl::context::file_format::pem);
}

void
server_stub::start(std::stop_token stoken)
{
    net::io_context ioc{1};

    tcp::acceptor acceptor{ioc, {net::ip::make_address("127.0.0.1"), 443}};

    for (;;)
    {
        tcp::socket socket{ioc};

        acceptor.async_accept(socket, beast::bind_front_handler(&server_stub::on_accept, this));

        ioc.run_for(100ms);
        if (stoken.stop_requested())
        {
            break;
        }
    }
}

void
server_stub::on_accept(beast::error_code ec, boost::asio::ip::tcp::socket socket)
{
    ssl::stream<tcp::socket &> stream{socket, ctx};

    stream.handshake(ssl::stream_base::server);

    beast::flat_buffer buffer;

    http::request<http::string_body> req;

    http::read(stream, buffer, req, ec);
}
