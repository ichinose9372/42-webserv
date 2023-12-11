#include "PostRequest.hpp"

PostRequest::PostRequest() {}

PostRequest::~PostRequest() {}

void PostRequest::handlePostRequest(Request& req, Response& res)
{
    std::map<std::string, std::string> headers = req.getHeaders();
    //all headers display
    for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); ++it) 
    {
        std::cout << it->first << ": " << it->second << std::endl;
    }
    if (headers.find("filename") != headers.end()) 
    {
        std::string filename = headers["filename"];
        std::string path = req.getUri() + filename;
        std::ofstream outputFile(path);
        std::string body = req.getBody();
        outputFile << body;
        outputFile.close();

        res.setStatus("200 OK");
        res.setBody("<html><body><h1>200 OK</h1></body></html>");
        res.setResponse();
    } 
    else
    {
    // "filename" キーが存在しない場合の処理
        res.setStatus("400 Bad Request");
        res.setBody("<html><body><h1>400 Bad Request</h1></body></html>");
        res.setResponse();
   }
}
