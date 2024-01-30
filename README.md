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
```
git clone git@github.com:ichinose9372/42-webserv.git webserv
cd webserv
make
```

## 実行方法
引数としてconfiglationsフォルダのファイルを渡します。  
```
 ./webserv configrations/default.conf
```
 
