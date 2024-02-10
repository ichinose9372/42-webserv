#include "Controller.hpp"

Controller::Controller()
{
}

Controller::~Controller()
{
}

std::string createNewFilePath(const std::string& originalPath, int number)
{
    std::ostringstream numStr;
    numStr << number; // int型のnumberを文字列に変換

    std::string baseName;
    std::string extension;
    size_t pos = originalPath.find_last_of(".");

    if (pos != std::string::npos && pos != 0) {
        // 拡張子がある場合
        extension = originalPath.substr(pos);
        baseName = originalPath.substr(0, pos);
    } else {
        // 拡張子がない場合
        baseName = originalPath;
    }

    // 既存の数字を取り除く
    std::string::size_type lastDigitPos = baseName.find_last_not_of("0123456789");
    if (lastDigitPos != std::string::npos) {
        baseName = baseName.substr(0, lastDigitPos + 1);
    }

    // 新しいファイル名を生成
    return baseName + numStr.str() + extension;
}

bool fileExists(const std::string& fileName) {
    std::ifstream file(fileName.c_str());
    return file.good();
}


std::string Controller::getFilepath(Request &req)
{
    std::string body = req.getBody();
    std::string filenameKey = "filename=";
    std::size_t filenamePos = body.find(filenameKey);
    if (filenamePos == std::string::npos && body == "")
        return "";
    if (filenamePos == std::string::npos)
    {
        std::string remakeBody;
        remakeBody += "Content-Disposition: form-data; ";
        remakeBody += "name=\"upfile\"; ";
        remakeBody += "filename=\"post.txt\"";
        remakeBody += "\nContent-Type: text/plain";
        remakeBody += "\r\n\r\n";
        remakeBody += req.getBody();
        req.setBody(remakeBody);
        filenamePos = remakeBody.find(filenameKey);
    }
    filenamePos += filenameKey.length() + 1; // Skip past "filename="

    body = req.getBody();
    std::size_t filenameEnd = body.find('\"', filenamePos);
    std::string filename = body.substr(filenamePos , filenameEnd - filenamePos);
    filename = sanitizeFilename(filename);

    std::string path = req.getUri();
    if (path[path.length() - 1] != '/')
        path += '/';
    //check file exist
    path += filename;
    if (fileExists(path)) 
    {
        int copyNumber = 1;
        std::string copyPath = path;

        // 利用可能なコピー先のファイル名を見つける
        do {
            copyPath = createNewFilePath(copyPath, copyNumber);
            copyNumber++;
        } while (fileExists(copyPath));
        path = copyPath;
    }
    return path;
}

std::string Controller::sanitizeFilename(const std::string &filename)
{
    std::string sanitizedFilename;
    for (std::size_t i = 0; i < filename.size(); ++i)
    {
        char ch = filename[i];
        // 無効または危険な文字をスキップ
        if (ch == '/' || ch == '\\' || ch == ':' || ch == '*' || ch == '?' || ch == '\"' || ch == '<' || ch == '>' || ch == '|')
            continue;
        // ディレクトリトラバーサル攻撃を防ぐためのチェック
        if (sanitizedFilename.length() >= 2 && sanitizedFilename.substr(sanitizedFilename.length() - 2) == "..")
            sanitizedFilename.erase(sanitizedFilename.length() - 2);
        sanitizedFilename += ch;
    }
    return sanitizedFilename;
}

void Controller::setReturnCode(Request &req, Response &res)
{
    int returnCode = req.getReturnParameter().first;
    std::string returnPage = req.getReturnParameter().second;
    std::string responseHtml = getResponseHtml(returnCode, req);
    res.setStatus(res.getStatusMessage(returnCode));
    if (returnCode == 301)
        res.setHeaders("Location: ", returnPage);
    if (returnCode == 405)
        res.setHeaders("Allow:", " GET, POST");
    res.setHeaders("Content-Type: ", "text/html");
    res.setBody(responseHtml);
    res.setHeaders("Content-Length: ", Utils::my_to_string(responseHtml.size()));
    res.setResponse();
}

std::string Controller::getResponseHtml(int statusCode, Request req)
{
    switch (statusCode)
    {
    case 301:
        return "<html><body><h1>301 Moved Permanently</h1></body></html>";
    case 400:
        return "<html><body><h1>400 Bad Request</h1></body></html>";
    case 403:
        return "<html><body><h1>403 Forbidden</h1></body></html>";
    case 404:
        return GetRequest::getBody(req.getErrorpage(404));
    case 405:
        return "<html><body><h1>405 Method Not Allowed</h1></body></html>";
    case 500:
        return GetRequest::getBody(req.getErrorpage(500));
    // 他のステータスコードに対するHTMLテキスト
    default:
        return "<html><body><h1>Error</h1><p>Unknown Error Occurred</p></body></html>";
    }
}

void Controller::processFile(Request &req, Response &res)
{
    if (req.getReturnParameter().first != 0)
    {
        setReturnCode(req, res);
        return;
    }
    std::string method = req.getMethod();
    if (method == "GET")
        GetRequest::handleGetRequest(req, res);
    else if (method == "POST")
        PostRequest::handlePostRequest(req, res);
    else if (method == "DELETE")
        DeleteRequest::handleDeleteRequest(req, res);
    else
    {
        res.setStatus(res.getStatusMessage(405));
        res.setBody("<html><body><h1>405 Method Not Allowed</h1></body></html>");
    }
}
