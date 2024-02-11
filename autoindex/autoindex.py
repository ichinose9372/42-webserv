#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
from flask import Flask, render_template

# docs_path = os.getcwd()
# docs_path = os.getcwd() + "/docs"
# print(os.environ.get('AUTOINDEX'))
docs_path = os.environ.get('AUTOINDEX')

app = Flask(__name__)

# @app.route('/show_file/<path:path>')
# def show_file(path):
# 		full_path = os.path.join(docs_path, path)
# 		return render_template(full_path)

# @app.route('/')
def get_docs_contents():
	try:
		list_files, list_pathes = get_entries(docs_path)
		combined_data = list(zip(list_files, list_pathes))
		with app.app_context():
			return render_template("autoindex.html", combined_data=combined_data)
	except FileNotFoundError:
		with app.app_context():
			return render_template("404.html")

def get_entries(docs_path):
	list_files = []
	list_pathes = []

	def process_entry(entry):
		full_path = os.path.join(docs_path, entry)
		if (os.path.isdir(full_path)):
			dir_entries = os.listdir(full_path)
			for dir_entry in dir_entries:
				process_entry(os.path.join(entry, dir_entry))
		else:
			list_files.append(entry)
			list_pathes.append(full_path)

	entries = os.listdir(docs_path)
	for entry in entries:
		process_entry(entry)
	return list_files, list_pathes

if __name__ == '__main__':
	print(get_docs_contents()) 
	# app.run(host='localhost', port=8080)




# app.template_folder = os.path.join(os.path.dirname(__file__), "templates")
# def get_docs_contents():
# 	try:
# 		docs_path = os.getcwd()
# 		docs_path += "/docs"
# 		entries = os.listdir(docs_path)
# 		list_files = []
# 		list_pathes = []

# 		for entry in entries:
# 			full_path = os.path.join(docs_path, entry)
# 			if os.path.isdir(full_path):
# 				dir_entries = os.listdir(full_path)
# 				for entry in dir_entries:
# 					list_files.append(entry)
# 					list_pathes.append(os.path.join(full_path, entry))
# 			else:
# 				list_files.append(entry)
# 				list_pathes.append(full_path)
		
# 		with app.app_context():
# 			return render_template("autoindex.html",
# 						  			files=list_files,
# 									pathes=list_pathes)
				
# 	except OSError as e:
# 		print("Error reading directory: {}".format(e))
# 		return []

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

# from flask_autoindex import Autoindex

# Autoindex(app, browse_root = os.path.curdir)

# #---test---
# target = "/autoindex"
# if (path.endswith(target)):
# 	path = path[:-len(target)]
# print(os.environ.get('PATH'))
# print(os.environ.get('PYTHONPATH'))
# import sys
# print(sys.path)
# files, directories = getDirectoryContents()
# print("Files: ")
# for file in files:
# 	print(file)
# print("\nDirectories: ")
# for directory in directories:
# 	print(directory)
