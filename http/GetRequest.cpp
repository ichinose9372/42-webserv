#include "GetRequest.hpp"

GetRequest::GetRequest() {}

GetRequest::~GetRequest() {}

bool isCgiScript(const std::string &filePath)
{
    return filePath.size() >= 4 && filePath.substr(filePath.size() - 3) == ".sh";
}

int GetRequest::openFile(const std::string &filePath)
{
    // std::cout << "In GetRequest::openFile   file psth = " << filePath << std::endl;

    struct stat buffer;
    if (stat(filePath.c_str(), &buffer) != 0)
    {
        switch (errno)
        {
        // 存在しないファイル・ディレクトリ
        case ENOENT:
            return 404;
        // アクセス権限がない
        case EACCES:
            return 403;
        default:
            return 500;
        }
    }

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
    std::ifstream file(filePath);
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

    // std::cout << "Read content size: " << buffer.str().size() << " characters." << std::endl;
    return buffer.str();
}

void GetRequest::handleGetRequest(Request &req, Response &res)
{
    if (isCgiScript(req.getUri()))
        handleCgiScript(req, res);
    else
        handleRegularFile(req, res);
}

void GetRequest::handleCgiScript(Request &req, Response &res)
{
    std::cout << "In GetRequest::handleGetRequest" << std::endl;
    ExecCgi::executeCgiScript(req, res);
}

void GetRequest::handleRegularFile(Request &req, Response &res)
{
    int statusCode = openFile(req.getUri());
    std::string message = res.getStatusMessage(statusCode);
    res.setStatus(message);

    if (statusCode == 200)
    {
        std::string content = getBody(req.getUri());
        res.setBody(content);
        res.setHeaders("Content-Type: ", "text/html");
        res.setHeaders("Content-Length: ", std::to_string(content.size()));
    }
    else
    {
        // 適切なエラーページをセット
        res.setBody("<html><body><h1>" + message + "</h1></body></html>");
    }

    res.setResponse(); // HTTPレスポンスを設定
}