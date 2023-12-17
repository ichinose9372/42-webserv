#include "PostRequest.hpp"

PostRequest::PostRequest() {}

PostRequest::~PostRequest() {}

void PostRequest::handlePostRequest(Request& req, Response& res)
{
    // cgi の実行ななのかファイルのアップロードなのかを判定して処理を分岐する
    //ファイルアップロードの場合
    std::string path = Controller::getFilepath(req);
    if (path != "")
    {
        std::ofstream outputFile(path);
        if (!outputFile.is_open())
        {
            res.setStatus("404 Not Found");
            res.setBody("<html><body><h1>404 Not Found</h1></body></html>");
            res.setResponse();
            return;
        }
        std::string body = req.getBody();
        if (req.getMaxBodySize() != 0 && body.size() > req.getMaxBodySize())
        {
            res.setStatus("413 Request Entity Too Large");
            res.setBody("<html><body><h1>413 Request Entity Too Large</h1></body></html>");
            res.setResponse();
            return;
        }
        outputFile << body;
        outputFile.close();
        res.setStatus("200 OK");
        res.setBody("<html><body><h1>200 OK</h1></body></html>");
        res.setHeaders("Content-Type: ", "text/html");
        res.setHeaders("Content-Length: ", std::to_string(res.getBody().size()));
        res.setResponse();
    }
    else
    {
        // std::cout << "In PostRequest::handlePostRequest   aaaaaaa" << std::endl;
        ExecCgi::executeCgiScript(req, res);
    }

    res.setResponse();
    //cgi の実行の場合
}
