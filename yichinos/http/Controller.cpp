#include "Controller.hpp"

Controller::Controller()
{
}

Controller::~Controller()
{
}

bool isCgiScript(const std::string& filePath) {
    // 拡張子が .cgi なら CGI スクリプトとみなす
    return filePath.size() >= 4 && filePath.substr(filePath.size() - 3) == ".sh";
}

void executeCgiScript(Request& req, Response& res) 
{
    std::string path = req.getUri();
    std::cout << "path = " << path << std::endl;
    if (access(path.c_str(), F_OK | X_OK) != 0) 
    {
        res.setStatus("404 Not Found");
        res.setBody("<html><body><h1>404 Not Found</h1><p>Requested script not found.</p></body></html>");
        return;
    }
    std::cout << "INININN\n";
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
    if (req.getHeaders().find("returnCode") != req.getHeaders().end())
    {
        std::string returnCode = req.getHeaders().find("returnCode")->second;
        std::cout << returnCode << std::endl;
        res.setStatus(returnCode);
        std::string returnPage =  req.getHeaders().find("returnPage")->second;
        std::cout << returnPage << std::endl;
        res.setBody(returnPage);
        res.setResponse();
        return;
    }
    std::string method = req.getMethod();
    if (method == "GET")
    {
        std::cout << "IN GET" << std::endl;
        if (isCgiScript(req.getUri())) //cgiの場合
        {
            executeCgiScript(req, res);
        }
        else //htmlの場合
        {
            res.setStatus(openFile(req.getUri()));
            res.setBody(getBody(res.getStatus(), req.getUri()));
        }
        res.setResponse();
    }
    if (method == "POST")
    {   
        std::cout << "IN POST" << std::endl;
        std::map<std::string, std::string> headers = req.getHeaders();
        //all headers display
        for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); ++it) 
        {
            std::cout << it->first << ": " << it->second << std::endl;
        }
        if (headers.find("filename") != headers.end()) 
        {
            std::string filename = headers["filename"];
            std::cout << "filename = " << filename << std::endl;

            std::string path = req.getUri() + filename;
            std::cout << "path = " << path << std::endl;

            std::ofstream outputFile(path);
            std::string body = req.getBody();
            outputFile << body;
            outputFile.close();

            res.setStatus("200 OK");
            res.setBody("<html><body><h1>200 OK</h1></body></html>");
            res.setResponse();
        } 
        else
        {
        // "filename" キーが存在しない場合の処理
            res.setStatus("400 Bad Request");
            res.setBody("<html><body><h1>400 Bad Request</h1></body></html>");
            res.setResponse();
       }
    }
    if (method == "DELETE")
    {
        std::cout << "IN DELETE" << std::endl;
        // std::cout << "DELETE" << std::endl;
        std::map<std::string, std::string> headers = req.getHeaders();
        std::string filename = headers["filename"];
        std::cout << "filename = " << filename << std::endl;
        std::string path = req.getUri() + filename;

        std::cout << "path = " << path << std::endl;
        if (std::remove(path.c_str()) == 0)
        {
            res.setStatus("200 OK");
            res.setBody("<html><body><h1>200 OK</h1></body></html>");
        }
        else
        {
            res.setStatus("404 Not Found");
            res.setBody("<html><body><h1>404 Not Found</h1></body></html>");
        }
        res.setResponse();
    }
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