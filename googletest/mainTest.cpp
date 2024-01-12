#include <gtest/gtest.h>
#include <iostream>
#include <cstdlib>
#include <unistd.h>


std::string getHttpResponseCode(const std::string& url, const std::string method) 
{
    std::string command = "curl -X " + method + " -o /dev/null -s -w \"%{http_code}\" " + url;
    FILE* fp = popen(command.c_str(), "r");
    char buf[1024];
    std::string httpCode;
    while (fgets(buf, 1024, fp) != NULL) {
        httpCode += buf;
    }
    pclose(fp);
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
    EXPECT_EQ(httpCode, "404");
}

TEST(WebServerTest, Response405MethodNotAllowed) 
{
    // POSTメソッドでアクセスして405 Method Not Allowedを確認
    std::string httpCode = getHttpResponseCode("http://localhost:8080", "POST");
    EXPECT_EQ(httpCode, "405");
}

int main(int argc, char **argv) 
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}