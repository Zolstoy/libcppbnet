#include <gtest/gtest.h>

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/http/string_body_fwd.hpp>
#include <stop_token>
#include <thread>

#include "bnetcpp/client.hpp"
#include "server_stub.hpp"

using namespace std::chrono_literals;
using tcp = boost::asio::ip::tcp;   // from <boost/asio/ip/tcp.hpp>

std::string const ca_cert =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIDGzCCAgOgAwIBAgIUVlpyalwiQIyyrcHPGXGm+1fEPMIwDQYJKoZIhvcNAQEL"
    "BQAwHTELMAkGA1UEBhMCRkkxDjAMBgNVBAMMBXZhaGlkMB4XDTI0MTIwMTIwMjEy"
    "NVoXDTI5MTEzMDIwMjEyNVowHTELMAkGA1UEBhMCRkkxDjAMBgNVBAMMBXZhaGlk"
    "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAk/W74DzJBDOw5OW+EXSN"
    "gMAfmgZnRc6sP698IcrsBFs78VqB0donQqltnD43Ohxe+iHDGdHI1H4I3dY3OgCY"
    "HSIibJEkCfO4z1A3NtsNI8y2+AO3QKhMm9XK4TwMW9aFCnaocB+SbIbfmSiW5tfU"
    "KXfVp8ya0ieAO5zTEkhXX6ZGqr1gFtyM7wx3pjUuzffMnFQPrIZoY9JxBe3qnPED"
    "mkjC5qTxKytAfb6PpYYSl+jhnykfsMyR9IrypwUIG+IXImPd8y/6+m6JN06fwQWV"
    "p49hu3XvvtGOEU23tEbgDQR5t0AjKMlHmT2Y0WG6GsAnDALnNBkGq7ZNrk17Mw91"
    "VQIDAQABo1MwUTAdBgNVHQ4EFgQUOl11VxPYjLse2i7pNIXEc+Nn/iUwHwYDVR0j"
    "BBgwFoAUOl11VxPYjLse2i7pNIXEc+Nn/iUwDwYDVR0TAQH/BAUwAwEB/zANBgkq"
    "hkiG9w0BAQsFAAOCAQEAH0QgIq509cxFwSxqZRpbLBuHbdUq+xFB42N0ttDNJZzi"
    "T01OWsPYtim8/WXlYC5PHv1FZthY9/7Ci2tEicm6X01CNnvNgeZx8bBGpOq0rqkY"
    "+9xRPSQXVoIbApg3KHDeUq6Fe9leASFohEbXk7gbi9c1yuT4Z+O19KmY8/rtvR1N"
    "U9c0sNvcDC5Q4bVai6KAhLxzLCBaYSqY4ku881K3pBSNVEy5gBVj466DOFNLPNg6"
    "Oha9NBAsvMsXonrrYDYtwk92p3L9O55b/YKG0MYW4qCB27SZnYZwDea9+h/MLvFV"
    "lBjhUjWT859gkyO6pYSTfndSpnWAdtQK9zsTYociBQ==\n"
    "-----END CERTIFICATE-----";

TEST(all, case_01)
{
    ASSERT_NO_THROW(bnetcpp::authenticate_custom("localhost", 443, "id", "secret", ca_cert));
}

int
main(int argc, char **argv)
{
    auto stub = std::make_shared<server_stub>();
    std::jthread thr([stub](std::stop_token stoken) {
        try
        {
            stub->start(stoken);
        } catch (std::exception &e)
        {
            std::cerr << "ERROR IN STUB SERVER: " << e.what() << std::endl;
        }
    });

    testing::InitGoogleTest(&argc, argv);
    auto r = RUN_ALL_TESTS();
    thr.request_stop();
    return r;
}
