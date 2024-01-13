#include "ExecCgi.hpp"

ExecCgi::ExecCgi() {}

ExecCgi::~ExecCgi() {}

// void ExecCgi::executeCgiScript(Request& req, Response& res)
// {
//     if (req.getMethod() == "GET")
//     {
//         std::cout << "GET" << std::endl;
//         std::string path = req.getUri();
//         if (access(path.c_str(), F_OK | X_OK) != 0)
//         {
//             res.setStatus("404 Not Found");
//             res.setBody("<html><body><h1>404 Not Found</h1><p>Requested script not found.</p></body></html>");
//             return;
//         }
//         int pipefd[2];
//         pipe(pipefd);
//         pid_t pid = fork();
//         if (pid == 0)
//         {
//             close(pipefd[0]);
//             dup2(pipefd[1], STDOUT_FILENO);
//             dup2(pipefd[1], STDERR_FILENO);
//             close(pipefd[1]);
//             execve(path.c_str(), NULL, NULL);
//             exit(500);
//         }
//         else if (pid > 0)
//         {
//             close(pipefd[1]);
//             int status;
//             std::string output;
//             char buf[1024];
//             ssize_t len;
//             while ((len = read(pipefd[0], buf, sizeof(buf) - 1)) > 0) {
//                 buf[len] = '\0';
//                 output += buf;
//             }
//             close(pipefd[0]);
//             waitpid(pid, &status, 0);
//             if (WIFEXITED(status))
//             {
//                 int exitStatus = WEXITSTATUS(status);
//                 if (exitStatus == 500)
//                 {
//                     res.setStatus("500 Internal Server Error");
//                     res.setBody("");
//                     return;
//                 }
//             }
//             res.setStatus("200 OK");
//             res.setHeaders("Content-Type: ", "text/html");
//             res.setBody(output);
//             res.setHeaders("Content-Length: ", std::to_string(output.size()));
//             return;
//         }
//     }
//     else if (req.getMethod() == "POST")
//     {
//         std::string path = req.getUri();
//         // std::cout << "path = " << path << std::endl;
//         if (access(path.c_str(), F_OK | X_OK) != 0)
//         {
//             res.setStatus("404 Not Found");
//             res.setBody("<html><body><h1>404 Not Found</h1><p>Requested script not found.</p></body></html>");
//             return;
//         }
//         int pipefd[2];
//         pipe(pipefd);
//         pid_t pid = fork();
//         if (pid == 0)
//         {
//             close(pipefd[0]);
//             dup2(pipefd[1], STDOUT_FILENO);
//             dup2(pipefd[1], STDERR_FILENO);
//             close(pipefd[1]);
//             char methodEnv[] = "REQUEST_METHOD=POST";
//             char* envp[] = { methodEnv, NULL };
//             // std::cout << "methodEnv = " << methodEnv << std::endl;
//             execve(path.c_str(), NULL, envp);
//             exit(500);
//         }
//         else if (pid > 0)
//         {
//             close(pipefd[1]);
//             int status;
//             std::string output;
//             char buf[1024];
//             ssize_t len;
//             while ((len = read(pipefd[0], buf, sizeof(buf) - 1)) > 0) {
//                 buf[len] = '\0';
//                 output += buf;
//             }
//             close(pipefd[0]);
//             waitpid(pid, &status, 0);
//             if (WIFEXITED(status))
//             {
//                 int exitStatus = WEXITSTATUS(status);
//                 if (exitStatus == 500)
//                 {
//                     res.setStatus("500 Internal Server Error");
//                     res.setBody("");
//                     return;
//                 }
//             }
//             // std::cout << "output = " << output << std::endl;
//             res.setStatus("200 OK");
//             res.setHeaders("Content-Type: ", "text/html");
//             res.setBody(output);
//             res.setHeaders("Content-Length: ", std::to_string(output.size()));
//             return;
//         }
//     }
// }

void ExecCgi::executeCgiScript(Request &req, Response &res)
{
    std::string path = req.getUri();
    if (!isScriptAccessible(path))
    {
        res.setStatus("404 Not Found");
        res.setBody("<html><body><h1>404 Not Found</h1><p>Requested script not found.</p></body></html>");
        return;
    }

    // CGI実行のための共通処理
    executeCommonCgiScript(req, res, path);
}

bool ExecCgi::isScriptAccessible(const std::string &path)
{
    struct stat buffer;

    // stat関数を使用してファイルの情報を取得
    if (stat(path.c_str(), &buffer) != 0)
        return false;

    // S_IXUSRは所有者の実行権限があるかをチェック
    if ((buffer.st_mode & S_IXUSR) == 0)
        return false;

    return true;
}

void ExecCgi::executeCommonCgiScript(Request &req, Response &res, const std::string &path)
{
    // パイプのセットアップ
    int pipefd[2];
    pipe(pipefd);

    // CGIスクリプト実行のための子プロセスを作成
    pid_t pid = fork();
    if (pid == 0)
    { // 子プロセス
        // 環境変数の設定
        std::vector<std::string> envVars = buildEnvVars(req);
        std::vector<char *> envp = convertToEnvp(envVars);

        // 標準出力をパイプにリダイレクト
        dup2(pipefd[1], STDOUT_FILENO);
        dup2(pipefd[1], STDERR_FILENO);
        close(pipefd[0]);
        close(pipefd[1]);

        // argvの準備
        const char *scriptName = path.c_str();
        char *argv[] = {const_cast<char *>(scriptName), NULL};

        // CGIスクリプトの実行
        execve(path.c_str(), argv, envp.data());
        exit(EXIT_FAILURE);
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
        // std::cout << "output = " << output << std::endl;
        res.setStatus("200 OK");
        res.setHeaders("Content-Type: ", "text/html");
        res.setBody(output);
        res.setHeaders("Content-Length: ", std::to_string(output.size()));
        return;
    }
}

std::vector<std::string> ExecCgi::buildEnvVars(Request &req)
{
    std::vector<std::string> envVars;
    // リクエストメソッドに基づいて環境変数を設定
    if (req.getMethod() == "GET")
    {
        // GETリクエストの場合の環境変数を設定
        envVars.push_back("REQUEST_METHOD=GET");
        // その他のGETに特有の環境変数設定
    }
    else if (req.getMethod() == "POST")
    {
        // POSTリクエストの場合の環境変数を設定
        envVars.push_back("REQUEST_METHOD=POST");
        // その他のPOSTに特有の環境変数設定
    }
    // 共通の環境変数設定
    // ...
    return envVars;
}

std::vector<char *> ExecCgi::convertToEnvp(const std::vector<std::string> &envVars)
{
    std::vector<char *> envp;
    for (std::vector<std::string>::const_iterator it = envVars.begin(); it != envVars.end(); ++it)
        envp.push_back(const_cast<char *>(it->c_str()));
    envp.push_back(NULL); // 環境変数リストの終端
    return envp;
}
