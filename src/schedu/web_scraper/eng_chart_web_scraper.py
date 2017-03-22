from urllib.request import urlopen
from bs4 import BeautifulSoup
html = urlopen("https://www.registrar.ualberta.ca/calendar-archive/calendar-2015-2016/Undergrad/Engineering/chart1.html")
bsObj = BeautifulSoup(html.read())
tables = bsObj.findAll("table")
tableData = open("TableData.txt",'w')
filter(None,tables)

for line in tables:
	if(line is not None):
		tableData.write(str(line))

tableData.close()