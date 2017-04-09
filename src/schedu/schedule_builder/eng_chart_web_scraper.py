from urllib.request import urlopen
from bs4 import BeautifulSoup
from django.db import models
import sys
import re

class tempDegree:
	def __init__(self):
		self.coreClasses = []
		self.name = "NULL"
		self.electives = {}
def determineClass(string, degree):
	# Regex rules to determine whether it is a class, grab all data before first '(' or add an elective list
	ClassRules = r'.+?(?=\s\()|(.*$)'
	termList = string.split('\n')
	for line in termList:
		if "Term" in line:
			continue
		if line is "":
			continue
		if re.match(r'^\*[a-zA-Z]',line) is not None:
			continue
		#Parsing out Elective Lists
		if ("Complementary" in line) or ("or" in line) or ("Elective" in line) or ("Group" in line) or (len(line.split(',')) > 1):
			if(line in degree.electives):
				continue
			elif line.strip() == "or":
				continue
			else:
				degree.electives[line.strip()] = []
			continue
		course = re.match(ClassRules,line)
		degree.coreClasses.append(course.group(0))
		#print(course.group(0))

			
	return degree
"""
if(len(sys.argv) != 2):
	print("incorrect number of arguments")
	sys.exit(1)
if(str(sys.argv[1]) == "chart1"):
	# Update this link to extract newer/older charts
"""
html = urlopen("https://www.registrar.ualberta.ca/calendar-archive/calendar-2015-2016/Undergrad/Engineering/chart1.html")
"""
	fileName = str(sys.argv[1])

elif(str(sys.argv[1]) == "chart2"):
		# Update this link to extract newer/older charts
		html = urlopen("https://www.registrar.ualberta.ca/calendar-archive/calendar-2015-2016/Undergrad/Engineering/chart2.html")
else:
	print("invalid chart name")
	sys.exit(1)
"""
bsObj = BeautifulSoup(html.read())
degreeList = []

tds = bsObj.findAll("td")
for td in tds:

	if(td.find("p", {"class": "Charthead"}) is not None):
		#Create a new Degree class for every Charthead found
		degree = tempDegree()

		# No way to extract the Chart Year, Has to be added manually!!!
		degree.name = "2015-16 " + td.getText()
		continue
	if(td.find("p", {"class": "Chartyear"})):
		continue
	if(td.find("p", {"class": "charttext"} is not None)):
		# Want Only Class Data
		if(td.find("p", {"class":"Chartnotes"}) is not None):
			continue
		if(td.find("p", {"class": "Chartnotesrestart"}) is not None):
			continue
		if(td.find("a") is not None):
			continue
		if(td.find("span", {"style": "font-weight: bold"}) is not None):
			continue

		degree = determineClass(td.getText(), degree)
		if(degree not in degreeList):
			degreeList.append(degree)



for degree in degreeList:
	djangoDegree = None
	try:
		djangoDegree = Degree.objects.get(name=degree.name)
	except(Degree.DoesNotExist):
		djangoDegree = Degree()
		djangoDegree.name = degree.name
	for course in degree.coreClasses:
		myClass = None
		try:
			myClass = MyClass.objects.get(class_code=course) 
		except(MyClass.DoesNotExist):
			myClass = MyClass()
			myClass.class_code = course
		myClass.degree.add(djangoDegree)
		myClass.save()

	for elective in degree.electives:
		djangoElective = None
		try:
			djangoElective = ElectiveList.objects.get(name=elective)
		except(ElectiveList.DoesNotExist):
			djangoElective = ElectiveList()
			djangoElective.name = elective
		djangoElective.save()

	djangoDegree.save()