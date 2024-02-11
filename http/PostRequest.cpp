#include "PostRequest.hpp"

PostRequest::PostRequest() {}

PostRequest::~PostRequest() {}

std::string extractFileContent(const std::string &body)
{
    // ヘッダーの終わりを表す文字列（ここでは空行を仮定）
    std::string headerEnd = "\r\n\r\n";

    std::size_t startPos = body.find("\r\n\r\n");
    if (startPos == std::string::npos)
    {
        return "";
    }
    startPos += headerEnd.length();

    std::size_t endPos = body.find("\r\n-");
    if (endPos == std::string::npos)
    {
        endPos = body.length();
    }

    return body.substr(startPos, endPos - startPos);
}

void PostRequest::handlePostRequest(Request &req, Response &res)
{
    // cgi の実行なのかファイルのアップロードなのかを判定して処理を分岐する
    // ファイルアップロードの場合
    std::string path = Controller::getFilepath(req);
    if (path != "")
    {
        std::ofstream outputFile(path.c_str());
        if (!outputFile.is_open())
        {
            res.setStatus("404 Not Found");
            res.setBody(GetRequest::getBody(req.getErrorpage(404)));
            res.setResponse();
            return;
        }
        std::string body = req.getBody();
        body = extractFileContent(body);
        if (req.getClientMaxBodySize() != 0 && body.size() > req.getClientMaxBodySize())
        {
            res.setStatus("413 Request Entity Too Large");
            res.setBody("<html><body><h1>413 Request Entity Too Large</h1></body></html>");
            res.setResponse();
            return;
        }
        outputFile << body;
        outputFile.close();
        res.setStatus("201 OK");
        res.setBody("<html><body><h1>201 OK</h1></body></html>");
        res.setHeaders("Content-Type: ", "text/html");
        res.setHeaders("Content-Length: ", Utils::my_to_string(res.getBody().size()));
        res.setResponse();
    }
    else
    {
        ExecCgi::executeCgiScript(req, res);
    }
    res.setResponse();
}
