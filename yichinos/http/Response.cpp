#include "Response.hpp"

Response::Response()
{
    // std::cout << "Response constructor" << std::endl;
}

Response::~Response()
{
    // std::cout << "Response destructor" << std::endl;
}

Response::Response(Request& request)
{
   
  
}

const std::string& Response::getResponse()
{
    return response;
}