#include "RequestParse.hpp"

RequestParse::RequestParse()
{
    contentLength = 0;
}

RequestParse::~RequestParse() {}

// 前後の空白を削除する関数
void trim(std::string &s)
{
    // 文字列の前方の空白を削除
    size_t start = s.find_first_not_of(" \t\n\r\f\v");
    if (start == std::string::npos)
        s.clear(); // 文字列が空白のみの場合
    else
        s = s.substr(start);

    // 文字列の後方の空白を削除
    size_t end = s.find_last_not_of(" \t\n\r\f\v");
    if (end != std::string::npos)
        s = s.substr(0, end + 1);
}

std::string trim(const std::string &str)
{
    const std::string::size_type strBegin = str.find_first_not_of(" \t\n\r\f\v");
    if (strBegin == std::string::npos)
        return ""; // 文字列が空白文字のみの場合

    const std::string::size_type strEnd = str.find_last_not_of(" \t\n\r\f\v");
    const std::string::size_type strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}

std::vector<std::string> split(const std::string &s, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter))
    {
        tokens.push_back(token);
    }
    return tokens;
}

void RequestParse::parseRequest(Request &request, const std::string &rawRequest)
{
    // std::cout << "IN REQUESTPARSE::PARSEREQUEST request size == " << rawRequest.size() << std::endl;
    std::istringstream requestStream(rawRequest);
    std::string line;

    // std::cout << "rawRequest = " << rawRequest << std::endl;
    std::getline(requestStream, line);
    parseRequestLine(line, request);
    if (request.getReturnParameter().first != 0)
        return;
    while (std::getline(requestStream, line) && line != "\r")
        parseHeader(line, request);
    if (request.getHost().empty())
    {
        request.setReturnParameter(400, "");
        return;
    }
    parseBody(requestStream, request);
}

std::string getfilepathtoURI(const std::string &uri, Request &request)
{
    std::string return_uri;
    std::vector<std::string> uriTokens = split(uri, '/');
    if (uriTokens.size() >= 3) // /directory/filename の場合にこの条件に入る
    {
        return_uri = "/" + uriTokens[1] + "/";
        std::string filename;
        for (size_t i = 2; i < uriTokens.size(); i++)
        {
            filename += "/" + uriTokens[i];
        }
        // std::cout << "filename = " << filename << std::endl;
        request.setFilepath(filename);
    }
    else
        return_uri = uri;
    // std::cout << "return_uri = " << return_uri << std::endl;
    return return_uri;
}

bool isMethod(std::string method)
{
    if (method == "GET" || method == "POST" || method == "DELETE" || method == "HEAD")
        return true;
    return false;
}

bool isURL(std::string url)
{
    if (url[0] == '/')
        return true;
    return false;
}

bool isHttpVersion(std::string httpVersion)
{
    trim(httpVersion);
    if (httpVersion == "HTTP/1.1")
        return true;
    return false;
}

void RequestParse::parseRequestLine(const std::string &line, Request &request)
{

    // std::cout << "line.size() = " << line.size() << std::endl;
    if (line.size() >= 814)
    { // return_parameter.first = 400;
        request.setReturnParameter(413, "");
        return;
    }
    // std::cout << "line = " << line << std::endl;
    std::vector<std::string> requestLineTokens = Request::split(line, ' ');
    if (requestLineTokens.size() >= 3)
    {
        if (isMethod(requestLineTokens[0]) && isURL(requestLineTokens[1]) && isHttpVersion(requestLineTokens[2]))
        {
            request.setMethod(requestLineTokens[0]);
            request.setUri(getfilepathtoURI(requestLineTokens[1], request));
            request.setHttpVersion(requestLineTokens[2]);
            return;
        }
    }
    // ここで400のエラーを返す
    request.setReturnParameter(400, "");
}

// location separat filename and path
void RequestParse::parseHeader(const std::string &line, Request &request)
{
    std::vector<std::string> headerTokens = split(line, ':');
    if (headerTokens.size() >= 2)
    {
        std::string key = headerTokens[0];
        std::string value = headerTokens[1];

        // 先頭の空白文字を削除
        size_t start = value.find_first_not_of(' ');
        if (start != std::string::npos)
            value = value.substr(start);
        request.setHeaders(key, value);
        if (key == "Host")
        {
            request.setHost(value);
            if (headerTokens.size() >= 3)
                request.setPort(headerTokens[2]);
        }
        if (key == "Content-Length")
        {
            std::istringstream iss(value);
            if (!(iss >> contentLength))
            {
                request.setReturnParameter(413, "");
                return;
            }
            request.setContentLength(contentLength);
        }
    }
}

void RequestParse::parseBody(std::istringstream &requestStream, Request &request)
{
    int bodySize = request.getContentLength();
    bool isChunked = false;

    for (std::map<std::string, std::string>::const_iterator it = request.getHeaders().begin(); it != request.getHeaders().end(); ++it)
    {
        std::string key = trim(it->first);
        std::string value = trim(it->second);
        if (key == "Transfer-Encoding" && value == "chunked")
        {
            isChunked = true;
            break;
        }
    }
    if (isChunked)
    {
        std::string body;
        std::string line;
        int lineNumber = 0;         // 行番号を追跡するための変数
        unsigned int chunkSize = 1; // 0になるまでループを続けるため初期値を1に

        while (chunkSize > 0)
        {
            std::getline(requestStream, line);
            lineNumber++; // 行番号をインクリメント

            if (lineNumber % 2 == 1)
            {
                // 奇数行（サイズ行）: サイズを解析
                std::stringstream hexStream(line);
                hexStream >> std::hex >> chunkSize; // サイズを16進数から10進数に変換
                if (chunkSize == 0)
                    break; // チャンクの終了
            }
            else
            {
                // 偶数行（データ行）: データをボディに追加
                body += line;
            }
        }
        request.setBody(body);
    }
    else
    {
        if (bodySize > 0)
        {
            std::string body;
            body.resize(bodySize);
            requestStream.read(&body[0], bodySize);
            request.setBody(body);
        }
    }
}
