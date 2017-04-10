$(document).ready(function(){
	var userName = getUserName();
	var data = { username:userName, crsfmiddlewaretoken:window.CSRF_TOKEN };
	var args = { type:"GET", url:"/builder/completedclasses/", data:data, complete:completedClassesCallback };
	$.ajax(args);

});

function getUserName() {
	var url = window.location.href.split("/");
	url.pop(); // pop off trailing forward slash
	return url.pop();
}

function completedClassesCallback(response, status) {
	var requirements = document.getElementsByClassName("charttext");
	var json = response.responseJSON;
	for (var index in json) {
		var completedclass = json[index];
		for (var index2 in requirements) {
			var requirement = requirements[index2];
			var text = requirement.textContent;
			if (text == completedclass) {
				requirement.style.background = "rgba(255, 128, 128, 0.5)";
			}
		}
	}
}