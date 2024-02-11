#!/bin/bash

echo "<html>"
echo "<head>"
echo "<title>Simple Shell Script</title>"
echo "</head>"
echo "<body>"

echo "<h1>Simple Script to check the method</h1>"

# HTTPリクエストのメソッドを取得
request_method="$REQUEST_METHOD"

# メソッドに応じてメッセージを表示
if [ "$request_method" = "GET" ]; then
    echo "<p>This is a script for handling GET requests.</p>"
elif [ "$request_method" = "POST" ]; then
    echo "<p>This is a script for handling POST requests.</p>"
else
    echo "<p>Unsupported request method.</p>"
fi

echo "</body>"
echo "</html>"
