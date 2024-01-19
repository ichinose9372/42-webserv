#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
# print(os.environ.get('PATH'))
# print(os.environ.get('PYTHONPATH'))

from flask import Flask, render_template

app = Flask(__name__)
app.template_folder = os.path.join(os.path.dirname(__file__), "templates")

def get_directory_contents():
	try:
		path = os.getcwd()
		target = "/autoindex"

		if (path.endswith(target)):
			path = path[:-len(target)]

		files = []
		directories = []
		entries = os.listdir(path)

		for entry in entries:
			full_path = os.path.join(path, entry)
			if os.path.isdir(full_path):
				directories.append(entry)
			else:
				files.append(entry)
		
		with app.app_context():
			return render_template("autoindex.html", files=files, directories=directories)
				
	except OSError as e:
		print("Error reading directory: {}".format(e))
		return []

		# # return files, directories

		# files_contents = "<html><body><h1>Files:</h1><ul>"
		# directories_contents = "<html><body><h1>nDirectories:</h1><ul>"

		# for file in files:
		# 	files_contents += f"<li>{file}</li>"
		
		# for directory in directories:
		# 	directories_contents += f"<li>{directory}</li>"
		
		# files_contents += "</ul></body></html>"
		# directories_contents += "</ul></body></html>"

		# # return files_contents, directories_contents

		# ft_render_template("autoindex.html", files=files_contents, directories=directories_contents)

# def ft_render_template(template, **context):
# 	try:
# 		path = os.path.join(os.path.dirname(__file__), template)

# 		with open(path, 'r', encoding='utf-8') as template_file:
# 			template_content = template_file.read()
		
# 		rendered_content = template_content.format(**context)
# 		return rendered_content

# 	except OSError as e:
# 		print("Error reading directory: {}".format(e))

# #---test---
# files, directories = getDirectoryContents()
# print("Files: ")
# for file in files:
# 	print(file)
# print("\nDirectories: ")
# for directory in directories:
# 	print(directory)
# print(get_directory_contents())