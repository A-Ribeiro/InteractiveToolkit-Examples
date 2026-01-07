#include "HTTP.h"
#include <InteractiveToolkit/Platform/SocketTCP.h>

namespace ITKExtension
{
    namespace Network
    {
        bool HTTP::is_valid_header_character(char _chr)
        {
            return (_chr >= 32 && _chr <= 126) ||
                   _chr == '\r' ||
                   _chr == '\n' ||
                   _chr == '\t' ||
                   _chr == ' ';
        }

        bool HTTP::readFromStream(Platform::SocketTCP *socket)
        {
            clear();

            bool first_line_readed = false;

            std::vector<char> line;
            line.reserve(HTTP_MAX_HEADER_RAW_SIZE);
            int max_http_header_lines = HTTP_MAX_HEADER_COUNT;

            char input_buffer[HTTP_READ_BUFFER_CHUNK_SIZE] = {0};
            uint32_t read_feedback = 0;
            uint32_t curr_reading = 0;
            bool found_crlf = false;

            while (!found_crlf)
            {
                if (!socket->read_buffer((uint8_t *)input_buffer, sizeof(input_buffer), &read_feedback))
                {
                    if (socket->isReadTimedout())
                    {
                        printf("[HTTP] Read NO Headers timed out\n");
                        return false;
                    }
                    else
                    {
                        printf("[HTTP] Connection or thread interrupted with the read feedback: %u\n", read_feedback);
                        return false;
                    }
                }

                curr_reading = 0;
                do
                {
                    for (uint32_t i = curr_reading; i < read_feedback; i++)
                    {
                        if (!is_valid_header_character(input_buffer[i]))
                        {
                            printf("[HTTP] Invalid character in HTTP header: %u\n", (uint8_t)input_buffer[i]);
                            return false;
                        }
                        line.push_back(input_buffer[i]);
                        if (line.size() >= HTTP_MAX_HEADER_RAW_SIZE || input_buffer[i] == '\0')
                        {
                            printf("[HTTP] HTTP line too long: %u\n", (uint32_t)line.size());
                            return false;
                        }
                        if (line.size() >= 2 && line[line.size() - 1] == '\n' && line[line.size() - 2] == '\r')
                            break;
                    }

                    found_crlf = line.size() == 2 && line[0] == '\r' && line[1] == '\n';

                    curr_reading += (uint32_t)line.size();

                    if (line.size() >= 2 && line[line.size() - 2] == '\r' && line[line.size() - 1] == '\n')
                    {
                        line.pop_back();
                        line.pop_back();
                    }

                    std::string header = std::string(line.begin(), line.end());

                    // first header
                    if (!first_line_readed)
                    {
                        first_line_readed = true;
                        if (!read_first_line(header))
                            return false;
                        // auto parts = ITKCommon::StringUtil::tokenizer(header, " ");
                        // if (parts.size() == 3 && ITKCommon::StringUtil::startsWith(parts[2], "HTTP/"))
                        // {
                        //     method = parts[0];
                        //     path = parts[1];
                        //     http_version = parts[2];
                        // }
                    }
                    else if (!found_crlf)
                    {
                        auto delimiter_pos = header.find(": ");
                        if (delimiter_pos != std::string::npos)
                        {
                            std::string key = header.substr(0, delimiter_pos);
                            std::string value = header.substr(delimiter_pos + 2);
                            headers[key] = value;
                        }
                    }

                    max_http_header_lines--;
                    if (max_http_header_lines <= 0)
                    {
                        printf("[HTTP] Too many HTTP header lines readed...\n");
                        return false;
                    }

                    line.clear();
                } while (!found_crlf && curr_reading < read_feedback);
            }

            // read body if Content-Length is set
            auto it = headers.find("Content-Length");
            if (it != headers.end())
            {
                uint32_t content_length;
                try
                {
                    content_length = (uint32_t)std::stoul(it->second);
                }
                catch (...)
                {
                    printf("[HTTPResponse] Invalid content_length code: %s\n", it->second.c_str());
                    return false;
                }
                if (content_length == 0)
                    return true;
                else if (content_length >= HTTP_MAX_BODY_SIZE) // 100 MB limit
                    return false;

                body.resize(content_length);

                // copy content from curr_reading to body
                uint32_t already_read = std::min(read_feedback - curr_reading, content_length);
                if (already_read > 0)
                    memcpy(&body[0], &input_buffer[curr_reading], already_read);

                uint32_t total_read = already_read;
                while (total_read < content_length)
                {
                    uint32_t to_read = std::min(content_length - total_read, (uint32_t)HTTP_READ_BUFFER_CHUNK_SIZE);
                    if (!socket->read_buffer((uint8_t *)&body[total_read], to_read, &read_feedback))
                    {
                        if (socket->isReadTimedout())
                        {
                            printf("[HTTP] Read Body timed out\n");
                            return false;
                        }
                        else
                        {
                            printf("[HTTP] Connection or thread interrupted with the read feedback: %u\n", read_feedback);
                            return false;
                        }
                    }
                    total_read += to_read;
                }
            }

            return true;
        }

        bool HTTP::writeToStream(Platform::SocketTCP *socket)
        {
            std::string request_line = mount_first_line() + "\r\n";
            for (const char &ch : request_line)
            {
                if (!is_valid_header_character(ch))
                {
                    printf("[HTTP] Invalid character in HTTP request line: %u\n", (uint8_t)ch);
                    return false;
                }
            }
            uint32_t write_feedback = 0;
            if (!socket->write_buffer((uint8_t *)request_line.c_str(), (uint32_t)request_line.size(), &write_feedback))
                return false;

            for (const auto &header_pair : headers)
            {
                std::string header_line = header_pair.first + ": " + header_pair.second + "\r\n";
                for (const char &ch : header_line)
                {
                    if (!is_valid_header_character(ch))
                    {
                        printf("[HTTP] Invalid character in HTTP request line: %u\n", (uint8_t)ch);
                        return false;
                    }
                }
                if (!socket->write_buffer((uint8_t *)header_line.c_str(), (uint32_t)header_line.size(), &write_feedback))
                    return false;
            }

            // end of headers
            const char *end_of_headers = "\r\n";
            if (!socket->write_buffer((uint8_t *)end_of_headers, 2, &write_feedback))
                return false;

            // body
            if (body.size() > 0)
            {
                if (!socket->write_buffer((uint8_t *)body.data(), (uint32_t)body.size(), &write_feedback))
                    return false;
            }

            return true;
        }

        HTTP &HTTP::setHeader(const std::string &key,
                              const std::string &value)
        {
            std::string key_copy;
            key_copy.resize(key.length());
            for (int i = 0; i < (int)key.length(); i++)
                key_copy[i] = (!is_valid_header_character(key[i])) ? ' ' : key[i];
            std::string value_copy;
            value_copy.resize(value.length());
            for (int i = 0; i < (int)value.length(); i++)
                value_copy[i] = (!is_valid_header_character(value[i])) ? ' ' : value[i];
            headers[key_copy] = value_copy;
            return *this;
        }

        HTTP &HTTP::setBody(const std::string &body,
                            const std::string &content_type)
        {
            if (body.size() > 0)
            {
                headers["Content-Type"] = content_type;
                headers["Content-Length"] = std::to_string(body.size());
            }
            else
            {
                headers.erase("Content-Type");
                headers.erase("Content-Length");
            }
            this->body = std::vector<uint8_t>(body.begin(), body.end());
            return *this;
        }

        HTTP &HTTP::setBody(const uint8_t *body, uint32_t body_size,
                            const std::string &content_type)
        {
            if (body_size > 0)
            {
                headers["Content-Type"] = content_type;
                headers["Content-Length"] = std::to_string(body_size);
            }
            else
            {
                headers.erase("Content-Type");
                headers.erase("Content-Length");
            }
            this->body.assign(body, body + body_size);
            return *this;
        }

        std::string HTTP::bodyAsString() const
        {
            return std::string(body.begin(), body.end());
        }

    }
}