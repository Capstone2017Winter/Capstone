$(document).ready(function(){
	classes = new Array();
	$('#addClassButton').click(function(){
		var className = $("#classSearchTextId").val();
		if (classes.indexOf(className) != -1) {
			alert("This class is already in your list");
			return;
		}
		var data = { className:className, csrfmiddlewaretoken:window.CSRF_TOKEN};
		var args = { type:"POST", url:"/builder/class/", data:data, complete:addClassCallback };
		$.ajax(args);
	});
});

function addClassCallback(response, status) {
	var json = response.responseJSON;
	var first = json.objects[0];
	var name = first.asString;
	var description = first.courseDescription;
	var courseTitle = first.courseTitle;

	var myClass = {name:name, description:description, title:courseTitle};
	classes.push(myClass);

	$('#classListId').append('<li><p>' + name +'</p><ul></li><li><p>'+courseTitle+'</p></li><li><p>'+description+'</p></li></ul></li>')
}
