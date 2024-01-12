#include <gtest/gtest.h>
#include <iostream>
#include <cstdlib>
#include <unistd.h>


std::string getHttpResponseCode(const std::string& url) {
    std::string command = "curl -o /dev/null -s -w \"%{http_code}\" " + url;
    FILE* fp = popen(command.c_str(), "r");
    char buf[1024];
    std::string httpCode;
    while (fgets(buf, 1024, fp) != NULL) {
        httpCode += buf;
    }
    pclose(fp);
    return httpCode;
}


TEST(WebServerTest, Response200OK) {

    // 正常なページにアクセスして200 OKを確認
    std::string httpCode = getHttpResponseCode("http://localhost:8080");
    EXPECT_EQ(httpCode, "200");
}

TEST(WebServerTest, Response404NotFound) {

    // 存在しないページにアクセスして404 Not Foundを確認
    std::string httpCode = getHttpResponseCode("http://localhost:8080/nonexistentpage");
    EXPECT_EQ(httpCode, "404");
}

int main(int argc, char **argv) 
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

