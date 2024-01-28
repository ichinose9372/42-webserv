#!/usr/bin/python
# -*- coding: utf-8 -*-

import codecs
import os

method = os.getenv('REQUEST_METHOD')

html = "<html><body><p>Unsupported request method.</p></body><html>"
if method == "GET":
	html = codecs.open('./docs/python/get.html', 'r', 'utf-8').read()
elif method == "POST":
	html = codecs.open('./docs/python/post.html', 'r', 'utf-8').read()

# print(method)
print(html)

# import cgi
# form = cgi.FieldStorage()
# 初回ロード時
# if form.list == []:
# html = codecs.open('./docs/py-files/view_001.html', 'r', 'utf-8').read()
# # SUBMITボタン押下時
# else:
#     html = codecs.open('./docs/py-files/view_002.html', 'r', 'utf-8').read()
