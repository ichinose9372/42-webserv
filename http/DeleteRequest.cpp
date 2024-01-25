#include "DeleteRequest.hpp"

DeleteRequest::DeleteRequest()
{
}

DeleteRequest::~DeleteRequest()
{
}

void DeleteRequest::handleDeleteRequest(Request& req, Response& res)
{
    
    std::string path = req.getUri();
    //先にファイルにアクセス権があるかどうかを確認する
    if (access(path.c_str(), W_OK) != -1)
    {
    // 実行権限がある場合（ファイルにアクセスできる）
    // ファイル削除の試みを行う
        if (std::remove(path.c_str()) == 0)
        {
            res.setStatus("200 OK");
            res.setBody("<html><body><h1>200 OK</h1></body></html>");
            res.setHeaders("Content-Type: ", "text/html");
            res.setHeaders("Content-Length: ", std::to_string(res.getBody().size()));
        }
        else // その他のエラー
        {
            res.setStatus("500 Internal Server Error");
            res.setBody("<html><body><h1>500 Internal Server Error</h1></body></html>");
        }
        res.setHeaders("Content-Type: ", "text/html");
        res.setHeaders("Content-Length: ", std::to_string(res.getBody().size()));
    } 
    else
    {
    // 実行権限がない場合（ファイルにアクセスできない）
    // エラー処理
        if (errno == ENOENT) // ファイルが存在しない
        {
            res.setStatus("404 Not Found");
            res.setBody("<html><body><h1>404 Not Found</h1></body></html>");
        }
        else if (errno == EACCES) // アクセス権がない
        {
            res.setStatus("403 Forbidden");
            res.setBody("<html><body><h1>403 Forbidden</h1></body></html>");
        }
    }
    res.setResponse();
}