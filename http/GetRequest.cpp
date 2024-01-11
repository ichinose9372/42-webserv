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
    // 以降の行は削除する。この関数ではファイルのアクセシビリティに関するチェックのみとする
    // std::string content;

    // if (file.is_open())
    // {
    //     content = "200 OK";
    //     file.close();
    // }
    // else
    // {
    //     content = "404 Not Found";
    // }

    // std::cout << "content = " << content << std::endl;
    // return content;
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

    std::cout << "Read content size: " << buffer.str().size() << " characters." << std::endl;
    return buffer.str();
}

void GetRequest::executeCgiScript(Request &req, Response &res)
{
    std::string path = req.getUri();
    if (access(path.c_str(), F_OK | X_OK) != 0)
    {
        res.setStatus("404 Not Found");
        res.setBody("<html><body><h1>404 Not Found</h1><p>Requested script not found.</p></body></html>");
        return;
    }
    int pipefd[2];
    pipe(pipefd);
    pid_t pid = fork();
    if (pid == 0) // child
    {
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        dup2(pipefd[1], STDERR_FILENO);
        close(pipefd[1]);
        execve(path.c_str(), NULL, NULL);
        exit(500);
    }
    else if (pid > 0)
    {
        close(pipefd[1]);
        int status;
        std::string output;
        char buf[1024];
        ssize_t len;
        while ((len = read(pipefd[0], buf, sizeof(buf) - 1)) > 0)
        {
            buf[len] = '\0';
            output += buf;
        }
        close(pipefd[0]);
        waitpid(pid, &status, 0);
        if (WIFEXITED(status))
        {
            int exitStatus = WEXITSTATUS(status);
            if (exitStatus == 500)
            {
                res.setStatus("500 Internal Server Error");
                res.setBody("");
                return;
            }
        }
        res.setStatus("200 OK");
        res.setHeaders("Content-Type: ", "text/html");
        res.setBody(output);
        res.setHeaders("Content-Length: ", std::to_string(output.size()));
        return;
    }
}

void GetRequest::handleGetRequest(Request &req, Response &res)
{
    if (isCgiScript(req.getUri()))
    {
        std::cout << "In GetRequest::handleGetRequest" << std::endl;
        ExecCgi::executeCgiScript(req, res);
    }
    else
    {
        // if (req.getFilepath() != "")
        // {
        //     std::cout << "-------------handleget filepath != 0 --------------- \n";
        //     res.setStatus(openFile(req.getFilepath()));
        // }
        // else
        const std::string message = res.getStatusMessage(openFile(req.getUri()));
        res.setStatus(message);
        res.setHeaders("Content-Type: ", "text/html");
        res.setBody(getBody(req.getUri()));
        res.setHeaders("Content-Length: ", std::to_string(res.getBody().size()));
    }
    res.setResponse();
}
