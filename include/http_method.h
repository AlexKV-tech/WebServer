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
constexpr std::string_view httpMethodToString(HttpMethod method)
{
    switch (method)
    {
        case HttpMethod::GET: return "GET";
        case HttpMethod::POST: return "POST";
        case HttpMethod::PUT: return "PUT";
        case HttpMethod::DELETE: return "DELETE";
        default: return "UNKNOWN";
    }
}
inline HttpMethod stringToHttpMethod(std::string_view method)
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