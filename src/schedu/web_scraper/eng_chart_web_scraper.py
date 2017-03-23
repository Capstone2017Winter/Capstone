from urllib.request import urlopen
from bs4 import BeautifulSoup
import sys


if(len(sys.argv) != 2):
	print("incorrect number of arguments")
	sys.exit(1)
if(str(sys.argv[1]) == "chart1"):
	html = urlopen("https://www.registrar.ualberta.ca/calendar-archive/calendar-2015-2016/Undergrad/Engineering/chart1.html")
	fileName = str(sys.argv[1])
elif(str(sys.argv[1]) == "chart2"):
		html = urlopen("https://www.registrar.ualberta.ca/calendar-archive/calendar-2015-2016/Undergrad/Engineering/chart2.html")
else:
	print("invalid chart name")
	sys.exit(1)
bsObj = BeautifulSoup(html.read())
fileName = str(sys.argv[1]) + ".txt"

file = open(fileName, 'w')

tds = bsObj.findAll("td")
for td in tds:
	if(td.find("p", {"class": "Charthead"}) is not None):
		file.write(td.getText())
		continue
	if(td.find("p", {"class": "Chartyear"})):
		file.write(td.getText())		
		continue
	if(td.find("p", {"class": "charttext"} is not None)):
		if(td.find("p", {"class":"Chartnotes"}) is not None):
			continue
		if(td.find("p", {"class": "Chartnotesrestart"}) is not None):
			continue
		if(td.find("a") is not None):
			continue
		if(td.find("span", {"style": "font-weight: bold"}) is not None):
			continue
		file.write (td.getText())
