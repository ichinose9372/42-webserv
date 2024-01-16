import os

def getDirectoryContents():
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

# 		return files, directories

		files_contents = "<html><body><h1>Files:</h1><ul>"
		directories_contents = "<html><body><h1>nDirectories:</h1><ul>"

		for file in files:
			files_contents += f"<li>{file}</li>"
		
		for directory in directories:
			directories_contents += f"<li>{directory}</li>"
		
		files_contents += "</ul></body></html>"
		directories_contents += "</ul></body></html>"

		return files_contents, directories_contents

	except OSError as e:
		print("Error reading directory: {}".format(e))
		return [],[]

# #---test---
# files, directories = getDirectoryContents()
# print("Files: ")
# for file in files:
# 	print(file)

# print("\nDirectories: ")
# for directory in directories:
# 	print(directory)