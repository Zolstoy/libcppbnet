#include "client.hpp"

#include <base64/base64.h>

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast.hpp>
#include <map>

#include "boost/asio/io_context.hpp"
#include "boost/asio/ssl/host_name_verification.hpp"

namespace beast = boost::beast;   // from <boost/beast.hpp>
namespace http = beast::http;     // from <boost/beast/http.hpp>
namespace net = boost::asio;      // from <boost/asio.hpp>
using tcp = net::ip::tcp;         // from <boost/asio/ip/tcp.hpp>

namespace bnetcpp {

client::client(std::string const& token)
    : token_(token)
{}

std::map<REGION, std::string> region_oauth_uris = {
    {REGION::EU, "https://oauth.battle.net/token"},
    {REGION::US, "https://oauth.battle.net/token"},
    {REGION::APAC, "https://oauth.battle.net/token"},
    {REGION::CN, "https://oauth.battlenet.com.cn/token"},
};

client
authenticate_custom(std::string const& host, short port, std::string const& id, std::string const& secret,
                    std::string const& ca_cert)
{
    net::io_context ioc;

    tcp::resolver resolver(ioc);

    auto const results = resolver.resolve(host, std::to_string(port));

    net::ssl::context ctx(net::ssl::context::sslv23);
    if (!ca_cert.empty())
    {
        ctx.add_certificate_authority(boost::asio::buffer(ca_cert.data(), ca_cert.size()));
    }

    net::ssl::stream<tcp::socket> ssl_stream(ioc, ctx);

    net::connect(ssl_stream.lowest_layer(), results.begin(), results.end());

    ssl_stream.lowest_layer().set_option(tcp::no_delay(true));
    ssl_stream.set_verify_callback(net::ssl::host_name_verification(host));
    ssl_stream.set_verify_mode(net::ssl::verify_peer);

    ssl_stream.handshake(net::ssl::stream_base::client);

    http::request<http::string_body> req{http::verb::post, "/oauth/token", 11};
    req.set(http::field::host, host + ":" + std::to_string(port));
    req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
    req.set(http::field::content_type, "multipart/form-data; boundary=----WebKitFormBoundary7MA4YWxkTrZu0gW");
    req.set(http::field::authorization, "Basic " + base64_encode_mime("client_id:client_secret"));

    std::string body =
        "------WebKitFormBoundary7MA4YWxkTrZu0gW\r\n"
        "Content-Disposition: form-data; name=\"grant_type\"\r\n\r\n"
        "client_credentials\r\n"
        "------WebKitFormBoundary7MA4YWxkTrZu0gW--\r\n";

    req.body() = body;
    req.prepare_payload();

    // Envoyer la requête HTTP
    http::write(ssl_stream, req);

    // Cette variable va contenir la réponse
    beast::flat_buffer buffer;

    // Déclare une réponse pour contenir la réponse du serveur
    http::response<http::dynamic_body> res;

    // Recevoir la réponse HTTP
    http::read(ssl_stream, buffer, res);

    // Fermer la connexion
    ssl_stream.lowest_layer().shutdown(tcp::socket::shutdown_both);

    return client("token");
}

client
authenticate(REGION region, std::string const& id, std::string const& secret)
{
    return authenticate_custom(region_oauth_uris[region], 443, std::move(id), std::move(secret));
}
}   // namespace bnetcpp
