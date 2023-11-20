#include <iostream>
#include <string>

int main() {
    std::string postData = "42tokyo is the best!";
    std::cout << "<html><body>\n";
    std::cout << "<h1>受け取ったPOSTデータ:</h1>\n";
    std::cout << "<p>" << postData << "</p>\n";
    std::cout << "</body></html>\n";

    return 0;
}
