#!/usr/bin/python3
# -*- coding: utf-8 -*-

import cgi
import codecs

form = cgi.FieldStorage()

# 初回ロード時
if form.list == []:
    html = codecs.open('./view_001.html', 'r', 'utf-8').read()
# SUBMITボタン押下時
else:
    html = codecs.open('./view_002.html', 'r', 'utf-8').read()

print("")
print(html)
