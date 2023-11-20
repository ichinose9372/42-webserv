//http request and response are parsed here

#include <iostream>
#include <string>
#include <exception>
#include <fstream>
#include <vector>
#include <sstream>

// std::vector<std::string> split(const std::string &s, char delimiter) {
//     std::vector<std::string> tokens;
//     std::string token;
//     std::istringstream tokenStream(s);
//     while (std::getline(tokenStream, token, delimiter)) {
//         tokens.push_back(token);
//     }
//     return tokens;
// }

// int main()
// {
//     std::string request;
//     std::string response;
//     std::string line;
//     std::string request_type;
//     std::ifstream file("http_request.txt");
//     std::vector<std::string> token_vector;
//     if (file.is_open())
//     {
//         while (getline(file, line))
//         {
//             std::vector<std::string> tokens_tmp = split(line, ' ');
//             token_vector.insert(token_vector.end(), tokens_tmp.begin(), tokens_tmp.end());
//         }
//         file.close();
//     }
//     else
//     {
//         std::cout << "Unable to open file";
//     }
//     std::string request_of_token;
//     std::string host;
//     for(std::vector<std::string>::iterator it = token_vector.begin(); it != token_vector.end(); ++it) {
//         //parse request
//         if (*it == "GET")
//         {
            
//             it++;
//             request_of_token = *it;
//             request_of_token.erase(0,1);
//         }
//         //parse headers
//         if (*it == "Host:")
//         {
//             it++;
//             host = *it;
//         }
//         if (*it == "HTTP/1.1")
//         {
//             request_type = *it;
//         }
//         // std::cout << *it << std::endl;
//     }
//     // std::cout << "request =  " << request_of_token << std::endl;
//     // std::cout << "host =  " << host << std::endl;


//     //create response
//     //use sample.html file
//     //open file
//     std::ifstream file2(request_of_token);
//     std::string return_string;
//     std::string html;
//     std::string status;
//     std::string response_header;


//     if (file2.is_open())
//     {
//         while (getline(file2, line))
//         {
//             html += line;
//         }
//         file2.close();
//         status = "200 OK";

//     }
//     if (status == "200 OK")
//     {
//         response_header = "HTTP/1.1 200 OK\r\n";
//     }
//     else
//     {
//         response_header = "HTTP/1.1 404 Not Found\r\n";
//         html = "<html><body><h1>404 Not Found</h1></body></html>";
//     }
//     return_string += response_header;
//     return_string += "Content-Type: text/html\r\n";
//     return_string += "Content-Length: ";
//     return_string += std::to_string(html.length());
//     return_string += "\r\n";
//     return_string += "Connection: close\r\n";
//     return_string += "\r\n";
//     return_string += html;

//     std::cout << return_string << std::endl;
    
//     return 0;
// }
#include "Request.hpp"
#include "Response.hpp"
#include "Controller.hpp"


int main(void)
{
    std::string request = 
    "POST /sample.html HTTP/1.1\r\n"
    "Host: www.example.com\r\n"
    "Content-Type: application/x-www-form-urlencoded\r\n"
    "Content-Length: 27\r\n"
    "\r\n"
    "username=user&password=pass";

    Request req(request);
    Controller con;
    
    std::string return_status = con.openFile(req.getUri());
    std::string return_body = con.getBody(return_status, req.getUri());
    Response res;
    std::cout << res.getResponse(return_status, return_body) << std::endl;

    return 0;
}
