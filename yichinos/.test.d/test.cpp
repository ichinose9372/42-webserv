#include <iostream>
#include <string>

int main() {
    // ステータスコードとヘッダーを出力
    std::cout << "Status: 200 OK\n";
    std::cout << "Content-Type: text/html; charset=UTF-8\n\n";

    // 最初の行（Content-Type ヘッダ）を無視する
    std::string line;
    std::getline(std::cin, line);

    // 残りの標準入力からのデータを読み込み、そのまま出力
    while (std::getline(std::cin, line)) {
        std::cout << line << "\n";
    }

    return 0;
}
