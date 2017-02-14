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
			$('#search-returns').append(
				'<div class="search-returns">' +
          			'<div class="search-return">' +
            			'<p class="return-field class-title">' + name + '</p>' +
            			'<p class="return-field short-desc">' + courseTitle + '</p>' +
            			'<p class="return-field long-desc"><span>Description: </span>' + description + '</p>' +
            			'<p class="return-field section"><span>Section: </span>' + section.section + '</p>' +
            			'<p class="return-field time "><span>Time: </span>' + section.startTime + '-' + section.endTime + '</p>' +
            			'<p class="return-field days"><span>Days: </span>' + section.day + '</p>' +
            			'<p class="return-field location"><span>Location: </span>' + section.location + '</p>' +
          			'</div>' +
        		'</div>'
        		)
			});
		// add to class list
	}}
	$.ajax(args);
}
