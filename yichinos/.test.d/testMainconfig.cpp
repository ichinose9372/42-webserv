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
    config.tokenSearchandSet(); // エラーが発生すべきではない
    TearDownTestFile("single_line.cnf");
}

TEST_F(MainConfigTest, TokenSearchUnexpectedEOF) {
    // ファイルの終わりが予期せずに到達したときのテスト
    SetUpTestFile("unexpected_eof.cnf", "server {");
    MainConfig config("unexpected_eof.cnf");
    config.parseLine();
    EXPECT_THROW(config.tokenSearchandSet(), std::runtime_error);
    TearDownTestFile("unexpected_eof.cnf");
}

TEST_F(MainConfigTest, TokenSearchExpectedBraceAfterServer) {
    // serverキーワードの後にブレースが必要な場合のテスト
    SetUpTestFile("expected_brace.cnf", "server listen 80;");
    MainConfig config("expected_brace.cnf");
    config.parseLine();
    EXPECT_THROW(config.tokenSearchandSet(), std::runtime_error);
    TearDownTestFile("expected_brace.cnf");
}

TEST_F(MainConfigTest, TokenSearchServerBlockNotClosed) {
    // サーバーブロックが閉じられていない場合のテスト
    SetUpTestFile("server_not_closed.cnf", "server { listen 80;");
    MainConfig config("server_not_closed.cnf");
    config.parseLine();
    EXPECT_THROW(config.tokenSearchandSet(), std::runtime_error);
    TearDownTestFile("server_not_closed.cnf");
}

TEST_F(MainConfigTest, TokenSearchDoubleSemicolon) {
    // 二重のセミコロンが存在する場合のテスト
    SetUpTestFile("double_semicolon.cnf", "server { listen 80;; }");
    MainConfig config("double_semicolon.cnf");
    config.parseLine();
    EXPECT_THROW(config.tokenSearchandSet(), std::runtime_error);
    TearDownTestFile("double_semicolon.cnf");
}

TEST_F(MainConfigTest, TokenSearchDuplicatePort) {
    // 同じポート番号が2回設定されている場合のテスト
    SetUpTestFile("duplicate_port.cnf", 
        "server { listen 80; }\n"
        "server { listen 80; }");
    MainConfig config("duplicate_port.cnf");
    config.parseLine();
    EXPECT_THROW(config.tokenSearchandSet(), std::runtime_error);
    TearDownTestFile("duplicate_port.cnf");
}

TEST_F(MainConfigTest, DuplicateServerNameThrowsException) {
    // 重複する server_name を含む設定ファイルを用意
    SetUpTestFile("duplicate_server_name.cnf", 
        "server { server_name example.com; }\n"
        "server { server_name example.com; }");

    // 設定ファイルを読み込んだときに std::runtime_error が投げられることを期待
    EXPECT_THROW({
        MainConfig config("duplicate_server_name.cnf");
        config.parseLine();
        config.tokenSearchandSet();
    }, std::runtime_error);

    // テスト用ファイルを削除
    TearDownTestFile("duplicate_server_name.cnf");
}
