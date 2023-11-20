#include "Controller.hpp"

Controller::Controller()
{
}

Controller::~Controller()
{
}

bool isCgiScript(const std::string& filePath) {
    // 拡張子が .cgi なら CGI スクリプトとみなす
    return filePath.size() >= 4 && filePath.substr(filePath.size() - 4) == ".cgi";
}

std::string executeCgiScript(const std::string& filePath) 
{
    int pipefd[2];
    pipe(pipefd);  // パイプの作成

    pid_t pid = fork();

    if (pid == 0) {
        // 子プロセス
        close(pipefd[0]);  // 読み込み側を閉じる
        dup2(pipefd[1], STDOUT_FILENO);  // 標準出力をパイプに付け替え
        dup2(pipefd[1], STDERR_FILENO);  // 標準エラーもパイプに付け替え
        close(pipefd[1]);

        // 外部プログラムの実行
        execve(filePath.c_str(), NULL, NULL);
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        // 親プロセス
        close(pipefd[1]);  // 書き込み側を閉じる
        std::string output;
        char buf[1024];
        ssize_t len;
        while ((len = read(pipefd[0], buf, sizeof(buf) - 1)) > 0) {
            buf[len] = '\0';  // 子プロセスの出力を表示
            output += buf;
        }
        close(pipefd[0]);
        waitpid(pid, NULL, 0);
        return output;
    }
    return "";
}

void Controller::processFile(Request& req, Response& res) 
{
    if (isCgiScript(req.getUri())) {
        // CGIスクリプトの実行と出力のキャプチャ
        res.setBody(executeCgiScript(req.getUri()));
        if (res.getBody().empty()) {
            res.setStatus("404 Not Found");
        } else {
            res.setStatus("200 OK");
        }
    } else {
        // ファイルの読み込み
        res.setStatus(openFile(req.getUri()));
        res.setBody(getBody(res.getStatus(), req.getUri()));
    }
    res.setResponse();
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