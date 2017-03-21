from urllib.request import urlopen
from bs4 import BeautifulSoup
html = urlopen("https://www.registrar.ualberta.ca/calendar-archive/calendar-2015-2016/Undergrad/Engineering/chart1.html")
bsObj = BeautifulSoup(html.read())
print(bsObj.div.table)