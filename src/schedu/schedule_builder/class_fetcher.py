import requests

def get_class(className):
	r = requests.get('https://classtime.herokuapp.com/api/v1/courses?q={"filters":[{"name":"asString","op":"equals","val":"'+className+'"}]}')
	return r.json()
