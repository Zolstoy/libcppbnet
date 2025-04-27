#pragma once

#include <map>
#include <string>

#include "config.hpp"

namespace bnetcpp {

class BNETCPP_API client
{
   private:
    std::string token_;

   public:
    client(std::string const& token);
};

enum class REGION
{
    EU,
    US,
    APAC,
    CN
};

extern std::map<REGION, std::string> region_oauth_uris;

BNETCPP_API client authenticate(REGION region, std::string const& id, std::string const& secret);
BNETCPP_API client authenticate_custom(std::string const& host, short port, std::string const& id,
                                       std::string const& secret, std::string const& ca_cert = "");

}   // namespace bnetcpp
