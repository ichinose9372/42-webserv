#include "Controller.hpp"

Controller::Controller()
{
}

Controller::~Controller()
{
}

std::string Controller::openFile(const std::string& filePath)
{
    std::ifstream file(filePath);
    std::string content;
    std::string line;
  
    if (file.is_open()) 
    {
        content = "200 OK";
        file.close();
    } 
    else
    {
        content = "404 Not Found";
    }
    return content;
}

std::string Controller::getBody(const std::string& status, const std::string& filePath)
{
    std::string body;
    if (status == "200 OK")
    {
        std::ifstream file(filePath);
        std::string line;
        while (getline(file, line))
        {
            body += line;
        }
        file.close();
    }
    else
    {
        body = "<html><body><h1>404 Not Found</h1></body></html>";
    }
    return body;
}