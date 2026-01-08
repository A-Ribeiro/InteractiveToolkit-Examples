#pragma once

#include <InteractiveToolkit/Platform/Core/SocketUtils.h>

#include <regex>

#include <netdb.h>
#include <arpa/inet.h>

namespace Platform
{
    namespace SocketTools
    {

        static inline bool isIPv4Address(const std::string &str)
        {
            // Simple regex for IPv4: 4 groups of 1-3 digits separated by dots
            std::regex ipv4_pattern(
                "^(\\d{1,3})\\.(\\d{1,3})\\.(\\d{1,3})\\.(\\d{1,3})$");

            std::smatch matches;
            if (!std::regex_match(str, matches, ipv4_pattern))
                return false;

            for (size_t i = 1; i < matches.size(); ++i)
            {
                int octet = std::stoi(matches[i].str());
                if (octet < 0 || octet > 255)
                    return false;
            }

            return true;
        }

        struct ParsedURL
        {
            std::string protocol; // http, https
            std::string hostname; // domain or IP
            uint16_t port;        // port number
            std::string path;     // path after hostname
            bool valid;
            bool ssl;

            ParsedURL() : port(0), path("/"), valid(false), ssl(false) {}
        };

        static inline ParsedURL parseURL(const std::string &url)
        {
            ParsedURL result;

            // URL format: [protocol://]hostname[:port][/path]
            std::regex url_pattern("^(?:([^:]+)://)?([^:/]+)(?::(\\d+))?(/.*)?$");

            std::smatch matches;

            if (!std::regex_match(url, matches, url_pattern))
                return result;
            
            result.protocol = matches[1].str();
            result.hostname = matches[2].str();

            // Default protocol to https if not specified
            if (result.protocol.empty())
                result.protocol = "https";

            // Parse port
            if (matches[3].matched)
                result.port = std::stoi(matches[3].str());
            else
            {
                // Default ports based on protocol
                if (result.protocol == "https")
                    result.port = 443;
                else if (result.protocol == "http")
                    result.port = 80;
                else
                    result.port = 443;
            }

            // Determine SSL based on protocol
            result.ssl = (result.protocol != "http");

            // Parse path (defaults to "/" if not specified)
            if (matches[4].matched)
                result.path = matches[4].str();

            result.valid = true;

            return result;
        }

        std::string resolveHostname(const std::string &hostname_or_ip)
        {
            // If it's already an IPv4 address, return as-is
            if (isIPv4Address(hostname_or_ip))
                return hostname_or_ip;

            // Use gethostbyname to resolve hostname to IP
            struct hostent *host = gethostbyname(hostname_or_ip.c_str());
            if (host == nullptr)
            {
                printf("Failed to resolve hostname: %s\n", hostname_or_ip.c_str());
                return "";
            }

            // Convert to dotted decimal notation
            struct in_addr **addr_list = (struct in_addr **)host->h_addr_list;
            if (addr_list[0] != nullptr)
            {
                return inet_ntoa(*addr_list[0]);
            }

            return "";
        }

    }
}