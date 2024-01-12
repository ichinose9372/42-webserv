#include "ExecCgi.hpp"

ExecCgi::ExecCgi(){}

ExecCgi::~ExecCgi(){}

void ExecCgi::executeCgiScript(Request& req, Response& res)
{
    const char *argv[] = {"/usr/bin/python", "./docs/py-files/app.py", NULL};

    std::string path = req.getUri();
    std::cout << "pwd = " << system("pwd") << std::endl;
    std::cout << "path = " << path << std::endl;

    if (req.getMethod() == "GET")
    {
        std::cout << "GET" << std::endl;
        if (access(path.c_str(), F_OK | X_OK) != 0) 
        {
            res.setStatus("404 Not Found");
            res.setBody("<html><body><h1>404 Not Found</h1><p>Requested script not found.</p></body></html>");
            return;
        }
        int pipefd[2];
        pipe(pipefd);
        pid_t pid = fork();
        if (pid == 0) 
        {
            close(pipefd[0]);
            dup2(pipefd[1], STDOUT_FILENO); 
            dup2(pipefd[1], STDERR_FILENO);
            close(pipefd[1]);
            char methodEnv[] = "REQUEST_METHOD=GET";
            char* envp[] = { methodEnv, NULL };
            execve("/usr/bin/python", const_cast<char *const *>(argv), envp);
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
            res.setHeaders("Content-Type: ", "text/html");
            res.setBody(output);
            res.setHeaders("Content-Length: ", std::to_string(output.size()));
            return;
        }
    }
    else if (req.getMethod() == "POST")
    {
        std::cout << "POST" << std::endl;
        if (access(path.c_str(), F_OK | X_OK) != 0) 
        {
            res.setStatus("404 Not Found");
            res.setBody("<html><body><h1>404 Not Found</h1><p>Requested script not found.</p></body></html>");
            return;
        }
        int pipefd[2];
        pipe(pipefd);
        pid_t pid = fork();
        if (pid == 0) 
        {
            close(pipefd[0]);
            dup2(pipefd[1], STDOUT_FILENO); 
            dup2(pipefd[1], STDERR_FILENO);
            close(pipefd[1]);
            char methodEnv[] = "REQUEST_METHOD=POST";
            char* envp[] = { methodEnv, NULL };
            execve("/usr/bin/python", const_cast<char *const *>(argv), envp);
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
            // std::cout << "output = " << output << std::endl;
            res.setStatus("200 OK");
            res.setHeaders("Content-Type: ", "text/html");
            res.setBody(output);
            res.setHeaders("Content-Length: ", std::to_string(output.size()));
            return;
        }
    }
}