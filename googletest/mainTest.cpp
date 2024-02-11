#include <gtest/gtest.h>
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <fstream>
#include <stdio.h>

std::string getHttpResponseCode_virtual(const std::string &url, const std::string &method) 
{
    // --resolve オプションを使用して、virtual_server の名前解決を 0.0.0.0 に固定します。
    // 実際には、0.0.0.0 を適切な IP アドレスに置き換える必要があります。
    std::string command = "curl --resolve virtual_server:8081:0.0.0.0 -X " + method + " -o /dev/null -s -w \"%{http_code}\" " + url;
    FILE *fp = popen(command.c_str(), "r");
    if (!fp) {
        std::cerr << "Failed to run curl command" << std::endl;
        return ""; // エラーハンドリングを適切に行います。
    }

    char buf[1024];
    std::string httpCode;
    while (fgets(buf, sizeof(buf), fp) != NULL) {
        httpCode += buf;
    }
    pclose(fp);
    return httpCode;
}

std::string getHttpResponseCode_default(const std::string &url, const std::string &method) 
{
    std::string command = "curl --resolve default_server:8081:0.0.0.0 -X " + method + " -o /dev/null -s -w \"%{http_code}\" " + url;
    FILE *fp = popen(command.c_str(), "r");
    if (!fp) {
        std::cerr << "Failed to run curl command" << std::endl;
        return ""; // エラーハンドリングを適切に行います。
    }

    char buf[1024];
    std::string httpCode;
    while (fgets(buf, sizeof(buf), fp) != NULL) {
        httpCode += buf;
    }
    pclose(fp);
    return httpCode;
}


std::string getHttpResponseCode(const std::string &url, const std::string method)
{
    std::string command = "curl -X " + method + " -o /dev/null -s -w \"%{http_code}\" " + url;
    FILE *fp = popen(command.c_str(), "r");
    char buf[1024];
    std::string httpCode;
    while (fgets(buf, 1024, fp) != NULL)
    {
        httpCode += buf;
    }
    pclose(fp);
    return httpCode;
}

bool fileExists(const std::string &filename)
{
    std::ifstream file(filename);
    return file.good();
}

std::string getHttpResponseUpload(const std::string &url, const std::string method)
{
    // make file
    std::string filename = "./42tokyo.txt";
    std::ofstream ofs(filename);
    ofs << "42tokyo" << std::endl;
    ofs.close();

    std::string command = "curl -X " + method + " -F file=@" + filename + " -o /dev/null -s -w \"%{http_code}\" " + url;
    FILE *fp = popen(command.c_str(), "r");
    char buf[1024];
    std::string httpCode;
    while (fgets(buf, 1024, fp) != NULL)
    {
        httpCode += buf;
    }
    pclose(fp);
    if (httpCode == "200")
    {
        if (fileExists("../../docs/upload/42tokyo.txt"))
        {
            // ファイルが存在する場合
            httpCode = "200";
        }
        else
        {
            // ファイルが存在しない場合
            httpCode = "404";
        }
    }
    remove(filename.c_str());
    return httpCode;
}

std::string getHttpResponseDelete(const std::string &url, const std::string method)
{
    // make file
    std::string filename = "../../docs/upload/42Tokyo.txt";
    std::ofstream ofs(filename);
    ofs << "42tokyo" << std::endl;
    ofs.close();

    std::string command = "curl -X " + method + " -o /dev/null -s -w \"%{http_code}\" " + url;
    FILE *fp = popen(command.c_str(), "r");
    char buf[1024];
    std::string httpCode;
    while (fgets(buf, 1024, fp) != NULL)
    {
        httpCode += buf;
    }
    pclose(fp);
    if (httpCode == "200")
    {
        if (fileExists(filename))
        {
            // ファイルが存在する場合（削除されていないくて正しくない）
            remove(filename.c_str());
            httpCode = "404";
        }
    }
    return httpCode;
}
std::string getHttpResponseDelete2(const std::string &url, const std::string method)
{
    // make file
    std::string filename = "../../docs/upload/42Tokyo.txt";
    std::ofstream ofs(filename);
    ofs << "42tokyo" << std::endl;
    ofs.close();

    system("chmod 000 ../../docs/upload/42Tokyo.txt");

    std::string command = "curl -X " + method + " -o /dev/null -s -w \"%{http_code}\" " + url;
    FILE *fp = popen(command.c_str(), "r");
    char buf[1024];
    std::string httpCode;
    while (fgets(buf, 1024, fp) != NULL)
    {
        httpCode += buf;
    }
    pclose(fp);
    if (httpCode == "200")
    {
        if (fileExists(filename))
        {
            // ファイルが存在する場合（削除されていないくて正しくない）
        }
    }
    system("chmod 777 ../../docs/upload/42Tokyo.txt");
    remove(filename.c_str());
    return httpCode;
}

TEST(WebServerTest, Response200OK)
{
    // 正常なページにアクセスして200 OKを確認
    std::string httpCode = getHttpResponseCode("http://localhost:8080", "GET");
    EXPECT_EQ(httpCode, "200");
}

TEST(WebServerTest, Response404NotFound)
{
    // 存在しないページにアクセスして404 Not Foundを確認
    std::string httpCode = getHttpResponseCode("http://localhost:8080/nonexistentpage", "GET");
    EXPECT_EQ(httpCode, "200");
}

TEST(WebServerTest, Response405MethodNotAllowed)
{
    // POSTメソッドでアクセスして405 Method Not Allowedを確認
    std::string httpCode = getHttpResponseCode("http://localhost:8080", "POST");
    EXPECT_EQ(httpCode, "405");
}

TEST(WebServerTest, Response200Port8081_virtual)
{
    // 8081ポートにアクセスして200 OKを確認
    std::string httpCode = getHttpResponseCode_virtual("http://virtual_server:8081/", "GET");
    EXPECT_EQ(httpCode, "200");
}

TEST(WebServerTest, Response200Port8081_default)
{
    // 8081ポートにアクセスして200 OKを確認
    std::string httpCode = getHttpResponseCode_default("http://default_server:8081/", "GET");
    EXPECT_EQ(httpCode, "200");
}

TEST(WebServerTest, FileUpload201)
{
    //8080ポートにアクセスしてファイルをアップロードできるのかを確認
    std::string httpCode = getHttpResponseUpload("http://localhost:8080/upload/", "POST");
    EXPECT_EQ(httpCode, "201");
}

TEST(WebServerTest, FileUpload404)
{
    //8080ポートにアクセスしてファイルをアップロードできるのかを確認
    std::string httpCode = getHttpResponseUpload("http://localhost:8080/upload/notexist/", "POST");
    EXPECT_EQ(httpCode, "404");
}

TEST(WebServerTest, FileDelete204)
{
    // 8080ポートにアクセスしてファイルをアップロードできるのかを確認
    std::string httpCode = getHttpResponseDelete("http://localhost:8080/delete/42Tokyo.txt", "DELETE");
    EXPECT_EQ(httpCode, "204");
}

TEST(WebServerTest, FileDelete404)
{
    // 8080ポートにアクセスしてファイルをアップロードできるのかを確認
    std::string httpCode = getHttpResponseDelete("http://localhost:8080/delete/NotFileExist", "DELETE");
    EXPECT_EQ(httpCode, "404");
}

TEST(WebServerTest, FileDelete403)
{
    // 8080ポートにアクセスしてファイルをアップロードできるのかを確認
    std::string httpCode = getHttpResponseDelete2("http://localhost:8080/delete/42Tokyo.txt", "DELETE");
    EXPECT_EQ(httpCode, "403");
}

TEST(WebServerTest, Response200PythonGET)
{
    std::string httpCode = getHttpResponseCode("http://localhost:8080/python/", "GET");
    EXPECT_EQ(httpCode, "200");
}

TEST(WebServerTest, Response200PythonPOST)
{
    std::string httpCode = getHttpResponseCode("http://localhost:8080/python/", "POST");
    EXPECT_EQ(httpCode, "200");
}

TEST(WebServerTest, Response200ShellGET)
{
    std::string httpCode = getHttpResponseCode("http://localhost:8080/shell/", "GET");
    EXPECT_EQ(httpCode, "200");
}

TEST(WebServerTest, Response200ShellPOST)
{
    std::string httpCode = getHttpResponseCode("http://localhost:8080/shell/", "POST");
    EXPECT_EQ(httpCode, "200");
}

TEST(WebServerTest, Response504ShellInfiniteGET)
{
    std::string httpCode = getHttpResponseCode("http://localhost:8080/shell-infinite/", "GET");
    EXPECT_EQ(httpCode, "504");
}

TEST(WebServerTest, Response504PythonInfiniteGET)
{
    std::string httpCode = getHttpResponseCode("http://localhost:8080/python-infinite/", "GET");
    EXPECT_EQ(httpCode, "504");
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
