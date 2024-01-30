**curl -i localhost:8080/index/を実行するための説明**

動的にosからディレクトリ構造を取得する必要があり、pythonのFlaskフレームワークを使用している。
校舎macにはFlaskがインストールされていないため、仮想環境を用意する必要がある。

make
- (venv)仮想環境の作成やフレームワークのインストールが実行される

c1r14s8% source venv/bin/activate
- sourceコマンドにより仮想環境をアクティベートされる

(venv) c1r14s8% ./webserv configrations/default.conf
- (venv)環境下だとFlaskが使用できるので、/index/を実行可能

(venv) c1r14s8% deactivate
- 通常の環境に戻る

補足
c1r14s8% ./webserv configrations/default.conf
- 通常の環境では/index/以外のlocationは実行できます


# Webserv 

## 概要
webserverを実装した

## セットアップ方法
1. リポジトリをクローンします：`git clone git@github.com:ichinose9372/42-webserv.git webserv`
2. リポジトリのディレクトリに移動します：`cd webserv`
3. `make` コマンドを実行して実行ファイルを作成します。

## 実行方法
実行ファイル `webserv` を起動します。　　
引数としてconfiglationsフォルダのファイルを渡します：`./webserv configrations/default.conf`
