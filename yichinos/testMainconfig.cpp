#include <gtest/gtest.h>
#include <fstream>
#include <sstream>
#include "MainConfig.hpp"

class MainConfigTest : public ::testing::Test {
protected:
    MainConfig config;

    // テスト用のファイルを作成するヘルパー関数
    void SetUpTestFile(const std::string& filename, const std::string& content) {
        std::ofstream file(filename);
        ASSERT_TRUE(file.is_open());
        file << content;
        file.close();
    }

    // テストが終わった後にテスト用のファイルを削除するヘルパー関数
    void TearDownTestFile(const std::string& filename) {
        std::remove(filename.c_str());
    }
};

TEST_F(MainConfigTest, ConstructorFileNotFound) {
    // ファイルが存在しない場合のテスト
    EXPECT_THROW(MainConfig config("nonexistent_file.cnf"), std::runtime_error);
}

TEST_F(MainConfigTest, ParseLineWithEmptyFile) {
    // 空のファイルを処理するテスト
    SetUpTestFile("empty_file.cnf", "");
    MainConfig config("empty_file.cnf");//ここでエラーが発生しないでそのまま象っこうされる。
    TearDownTestFile("empty_file.cnf");
}

TEST_F(MainConfigTest, LineToTokenSingleLine) {
    // 単一行の解析をテスト
    SetUpTestFile("single_line.cnf", "server { listen 80; }");
    MainConfig config("single_line.cnf");
    config.parseLine();
    config.tokenSearch(); // エラーが発生すべきではない
    TearDownTestFile("single_line.cnf");
}

TEST_F(MainConfigTest, TokenSearchUnexpectedEOF) {
    // ファイルの終わりが予期せずに到達したときのテスト
    SetUpTestFile("unexpected_eof.cnf", "server {");
    MainConfig config("unexpected_eof.cnf");
    config.parseLine();
    EXPECT_THROW(config.tokenSearch(), std::runtime_error);
    TearDownTestFile("unexpected_eof.cnf");
}

TEST_F(MainConfigTest, TokenSearchExpectedBraceAfterServer) {
    // serverキーワードの後にブレースが必要な場合のテスト
    SetUpTestFile("expected_brace.cnf", "server listen 80;");
    MainConfig config("expected_brace.cnf");
    config.parseLine();
    EXPECT_THROW(config.tokenSearch(), std::runtime_error);
    TearDownTestFile("expected_brace.cnf");
}

TEST_F(MainConfigTest, TokenSearchServerBlockNotClosed) {
    // サーバーブロックが閉じられていない場合のテスト
    SetUpTestFile("server_not_closed.cnf", "server { listen 80;");
    MainConfig config("server_not_closed.cnf");
    config.parseLine();
    EXPECT_THROW(config.tokenSearch(), std::runtime_error);
    TearDownTestFile("server_not_closed.cnf");
}