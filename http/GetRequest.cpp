#include "GetRequest.hpp"

GetRequest::GetRequest() {}

GetRequest::~GetRequest() {}

bool isCgiScript(const std::string &filePath)
{
    if (filePath.size() >= 4 && filePath.substr(filePath.size() - 3) == ".sh")
        return (true);
    if (filePath.size() >= 4 && filePath.substr(filePath.size() - 3) == ".py")
        return (true);
    if (filePath.size() >= 4 && filePath.substr(filePath.size() - 1) == "/")
        return (true);
    return (false);
}

int GetRequest::openFile(const std::string &filePath)
{
    struct stat buffer;
    if (stat(filePath.c_str(), &buffer) != 0)
        return 404;

    // 読み取り可能か確認
    if ((buffer.st_mode & S_IRUSR) == 0)
        return 403;

    // ファイルを開いて内容を読み込む
    std::ifstream file(filePath.c_str());
    if (!file)
        return 500;
    return 200;
}

std::string GetRequest::getBody(const std::string &filePath)
{
    std::ifstream file(filePath.c_str());
    if (!file)
    {
        // ファイルが開けない場合のエラーハンドリング
        std::cerr << "Error: Unable to open file: " << filePath << std::endl;
        return "<html><body><h1>404 Not Found</h1><p>Unable to open file.</p></body></html>";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    if (file.fail() || file.bad())
    {
        // ファイルの読み込み中にエラーが発生した場合のハンドリング
        std::cerr << "Error: Unable to read file: " << filePath << std::endl;
        file.close();
        return "<html><body><h1>500 Internal Server Error</h1><p>Error reading file.</p></body></html>";
    }
    file.close();
    return buffer.str();
}

void GetRequest::handleGetRequest(Request &req, Response &res)
{
    if (isCgiScript(req.getUri()))
    {
        handleCgiScript(req, res);
        return ;
    }
    else
        handleRegularFile(req, res);
    res.setResponse(); // HTTPレスポンスを設定
}

void GetRequest::handleCgiScript(Request &req, Response &res)
{
    ExecCgi::executeCgiScript(req, res);
}

void GetRequest::handleRegularFile(Request &req, Response &res)
{
    // std::cout << req.getUri() << std::endl;
    int statusCode = openFile(req.getUri());
    std::string message = res.getStatusMessage(statusCode);
    res.setStatus(message);
    std::string content;
    if (statusCode == 200)
    {
        content = getBody(req.getUri());
    }
    else if (statusCode == 404)
    {
        content = getBody(req.getErrorpage(404));
    }
    res.setBody(content);
    res.setHeaders("Content-Type: ", "text/html");
    res.setHeaders("Content-Length: ", Utils::my_to_string(content.size()));
}