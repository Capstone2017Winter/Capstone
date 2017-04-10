from urllib.request import urlopen
from bs4 import BeautifulSoup
#from .models import * 
import sys, re


if(len(sys.argv) != 2):
	print("incorrect number of arguments")
	sys.exit(1)
if(str(sys.argv[1]) == "chart1"):
	# Update this link to extract newer/older charts
	html = urlopen("https://www.registrar.ualberta.ca/calendar-archive/calendar-2015-2016/Undergrad/Engineering/chart1.html")
	fileName = str(sys.argv[1])
elif(str(sys.argv[1]) == "chart2"):
		# Update this link to extract newer/older charts
		html = urlopen("https://www.registrar.ualberta.ca/calendar-archive/calendar-2015-2016/Undergrad/Engineering/chart2.html")
else:
	print("invalid chart name")
	sys.exit(1)
bsObj = BeautifulSoup(html.read())

fileName = "table.txt"
file = open(fileName, 'w')

table = bsObj.find("table")
file.write(str(table))