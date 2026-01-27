#ifndef HTTP_METHOD_H
#define HTTP_METHOD_H

#include <string>
enum class HttpMethod
{
    GET,
    POST,
    PUT,
    DELETE,
    NONE
};
inline std::string to_string(HttpMethod method)
{
    switch (method)
    {
    case HttpMethod::GET:
        return "GET";
    case HttpMethod::POST:
        return "POST";
    case HttpMethod::PUT:
        return "PUT";
    case HttpMethod::DELETE:
        return "DELETE";
    default:
        return "";
    }
}
inline HttpMethod to_http_method(const std::string& method)
{
    if (method == "GET")
        return HttpMethod::GET;
    else if (method == "POST")
        return HttpMethod::POST;
    else if (method == "PUT")
        return HttpMethod::PUT;
    else if (method == "DELETE")
        return HttpMethod::DELETE;
    return HttpMethod::NONE;
}
#endif