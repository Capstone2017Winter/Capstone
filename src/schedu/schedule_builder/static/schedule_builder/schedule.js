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
		classes.push(className);
		$('#classListId').append('<li><p>' + className + "</p></li>");
	});
});

function addClassCallback(response, status) {
	var json = response.responseJSON;
	var a = json.a;
	var b = json.b;
}
