#ifndef EXECCGI_HPP
#define EXECCGI_HPP

#include "Controller.hpp"
#include "../socket/Timer.hpp"
#include <signal.h>
#include <fcntl.h>

class ExecCgi
{
public:
    ExecCgi();
    ~ExecCgi();
    static void executeCgiScript(Request &req, Response &res);
    static std::string getBody(const std::string &status, const std::string &filePath);
    static bool isScriptAccessible(const std::string &path);
    static void executeCommonCgiScript(Request &req, Response &res, const std::string &path);
    static std::vector<std::string> buildEnvVars(Request &req);
    static std::vector<char *> convertToEnvp(const std::vector<std::string> &envVars);
};

#endif