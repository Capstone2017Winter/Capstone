import requests

def ct_get_class_info(className):
	r = requests.get('https://classtime.herokuapp.com/api/v1/courses?q={"filters":[{"name":"asString","op":"equals","val":"'+className+'"}]}')
	return r.json()

def ct_get_class_sections(classId, termName):
	term_code = ct_term_to_code_map(termName)
	#python multi-line strings with replacement sucks... its going on one line
	q = '{"institution": "ualberta","term": "'+term_code +'","courses": ["'+classId+'"],"busy-times":[],"electives": [],"preferences": {"start-early": 0,"no-marathons": 0,"day-classes": 0,"current-status": false,"obey-status": true}}'
	print(q)
	r = requests.get('https://classtime.herokuapp.com/api/v1/generate-schedules?q='+q)
	return r.json()

#this is a necessary evil, there is no easy way to convert from term to code
def ct_term_to_code_map(termName):
	m = {   'FALL 2014':'1490',
		'WINTER 2015':'1500',
		'SPRING 2015':'1510',
		'SUMMER 2015':'1520',
		'FALL 2015':'1530',
		'WINTER 2016':'1540',
		'SPRING 2016':'1550',
		'SUMMER 2016':'1560',
		'FALL 2016':'1570',
		'WINTER 2017':'1580',
		'SPRING 2017':'1590',
		'SUMMER 2017':'1600'   }
	return m[termName]	
