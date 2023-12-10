#!/usr/bin/perl
use strict;
use CGI;  # CGI.pmを使用

my $query = CGI->new;  # CGIオブジェクトを作成

# HTTPヘッダを出力
print $query->header('text/html');

# HTMLドキュメントの開始
print "<!DOCTYPE html>\n";
print "<html>\n";
print "<head>\n";
print "<title>hello 42tokyo </title>\n";
print "<meta charset='UTF-8'>\n";  # UTF-8エンコーディングを使用
print "</head>\n";
print "<body>\n";
print "<h1>Hello, world!</h1>\n";

# クエリパラメータを取得して表示
my $name = $query->param('name');
print "<p>Your name is $name</p>\n" if $name;

# HTMLドキュメントの終了
print "</body>\n";
print "</html>\n";
