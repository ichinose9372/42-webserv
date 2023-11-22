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

void executeCgiScript(Request& req, Response& res) 
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
    if (pid == 0) //child
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
        while ((len = read(pipefd[0], buf, sizeof(buf) - 1)) > 0) {
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
        res.setBody(output);
        return;
    }
}

void Controller::processFile(Request& req, Response& res) 
{
    if (isCgiScript(req.getUri())) {
        executeCgiScript(req, res);
    } else {
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