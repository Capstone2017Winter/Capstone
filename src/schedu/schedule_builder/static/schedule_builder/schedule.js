$(document).ready(function(){
	classes = new Array();
	$('#addClassButton').click(function(){
		var className = $("#classSearchTextId").val();
		if (classes.indexOf(className) != -1) {
			alert("This class is already in your list");
			return;
		}
		var data = { className:className, csrfmiddlewaretoken:window.CSRF_TOKEN};
		var args = { type:"GET", url:"/builder/class/", data:data, complete:addClassCallback };
		$.ajax(args);
	});
});

function addClassCallback(response, status) {
	var json = response.responseJSON;
	var first = json.objects[0];
	var name = first.asString;
	var description = first.courseDescription;
	var courseTitle = first.courseTitle;
	var courseId = first.course;
	var termName = term + " " + year;

	var data = { courseId:courseId, termName:termName, csrfmiddlewaretoken:window.CSRF_TOKEN};
	var args = { type:"GET", url:"/builder/section/", data:data, complete:
	function(resp){
		var json1 = resp.responseJSON;
		var o = json1.objects[0];
		var sections = o.sections;

		// add class related stuff here
		sections.forEach(function(section) {
			//add section related stuff here
			$('#classListId').append('<li><p>' + name +'</p><ul><li><p>'+courseTitle+'</p></li><li><p>'+description+'</p></li><li><p>'+ section.section+'</p></li><li><p>'+section.startTime+'</p></li><li><p>'+section.endTime+'</p></li><li><p>'+section.day+'</p></li><li><p>'+section.location+'</p></li></ul></li>');
	

		});
		// add to class list
	}}
	$.ajax(args);
}
