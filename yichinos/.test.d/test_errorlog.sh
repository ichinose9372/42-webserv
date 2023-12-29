#!/bin/bash

# 実行ディレクトリとファイル名の設定
EXEC_DIR="./"
CONF_DIR="./cnf.d/conf"
EXEC_NAME="a.out"

# 色の設定
COLOR_GREEN="\033[0;32m"
COLOR_YELLOW="\033[0;33m"
COLOR_RESET="\033[0m"

# プログラムのビルド
make

# ビルドの成功を確認
if [ $? -ne 0 ]; then
    echo -e "${COLOR_YELLOW}Build failed.${COLOR_RESET}"
    exit 1
fi

# .confファイルのリストを取得
conf_files=($(find "$CONF_DIR" -name '*.conf'))

# 各.confファイルについてループ
for conf_file in "${conf_files[@]}"; do
    # プログラムを実行し、出力を変数に格納
    output=$("$EXEC_DIR$EXEC_NAME" "$conf_file" 2>&1)

    # 出力があるかチェック
    if [[ -n "$output" ]]; then
        printf "%s ${COLOR_GREEN}[ OK ]${COLOR_RESET}\n" "$conf_file"
        echo "$output"  # 出力内容を表示
    else
        printf "%s ${COLOR_YELLOW}[ NG ]${COLOR_RESET}\n" "$conf_file"
    fi
done

echo "All tests completed."

make clean
