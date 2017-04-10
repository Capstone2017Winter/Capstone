import subprocess, sys, json, re, ast, os

def getSections(term, subject_catalog):
	matchObj = re.match( r'(\D+)(\d+)', subject_catalog.strip(), re.M|re.I)
	subject = matchObj.group(1).strip().upper()
	catalog = matchObj.group(2).strip()
	path = os.path.abspath("schedule_builder/ldap2.py")

	process = subprocess.Popen(["python", path, term, subject, catalog], stdout=subprocess.PIPE)
	output, error = process.communicate()
	print(subprocessToJSON(output))
	return subprocessToJSON(output)

def getInfo(term, subject_catalog):
	matchObj = re.match( r'(\D+)(\d+)', subject_catalog.strip(), re.M|re.I)
	subject = matchObj.group(1).strip().upper()
	catalog = matchObj.group(2).strip()
	path = os.path.abspath("schedule_builder/ldap2.py")

	process = subprocess.Popen(["python", path, term, subject, catalog], stdout=subprocess.PIPE)
	output, error = process.communicate()
	print(output)
	return subprocessToJSON(output)


def subprocessToJSON(sub):
	sub = ast.literal_eval(sub.decode("utf-8"))
	js_output = json.dumps(sub)
	return js_output
