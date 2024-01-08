#include <gtest/gtest.h>
#include <iostream>
#include <cstdlib>
#include <unistd.h>

TEST(WebServerTest, Response200OK) 
{
    // Webサーバーを起動
    system("../webserv ../configurations/default.conf &");

    // curlコマンドを実行し、レスポンスを取得
    std::string command = "curl -o /dev/null -s -w \"%{http_code}\" http://localhost:8080";
    //commandを実行し、結果を取得
    FILE* fp = popen(command.c_str(), "r");
    char buf[1024];
    std::string httpCode;
    while (fgets(buf, 1024, fp) != NULL) {
        httpCode += buf;
    }
    pclose(fp);

    // レスポンスコードが200かどうか確認
    EXPECT_EQ(httpCode, "200");

    // Webサーバーを終了
    system("pkill webserv");
}


int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

