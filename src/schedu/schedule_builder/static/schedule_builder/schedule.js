$(document).ready(function(){

	window.searched_courses = {}
	window.added_courses = {}

	classes = new Array();
	$('#addClassButton').click(function(){
		//set up for a new search
		window.searched_courses = {}
		$(".search-returns").empty();
		
		var className = $("#classSearchTextId").val();
		if (classes.indexOf(className) != -1) {
			alert("This class is already in your list");
			return;
		}
		var data = { className:className, csrfmiddlewaretoken:window.CSRF_TOKEN};
		var args = { type:"GET", url:"/builder/class/", data:data, complete:addClassCallback };
		$.ajax(args);
	});

	$(".search-returns").on('click', '.expand-button', function() {
    	$(this).closest('div[class^="search-return"]').children('.more').toggle();
  	});

  	$(".class-column").on('click', '.expand-button', function() {
    	$(this).closest('div[class^="added-class"]').children('.more').toggle();
  	});

  	$(".class-column").on('click', '.minus-button', function() {
    	delete window.added_courses[$(this).closest('div[class^="added-class"]').attr('id')];
    	$(this).closest('div[class^="added-class"]').remove();
  	});

  	$('.search-returns').on('click', '.plus-button', function() {

    	var course = window.searched_courses[$(this).closest('div[class^="search-return"]').attr('id')];

    	if (!window.added_courses[course.varname]) {
      		window.added_courses[course.varname] = course;
    	} 
    	else {
      		return;
    	}

    	var lectures = ""
    	var seminars = ""
   		var labs = ""

    	Object.keys(course.lectures).forEach(function(key, index) {
      		lectures = lectures + '<option>' + key + '</option>'
    	});

    	Object.keys(course.seminars).forEach(function(key, index) {
      		seminars = seminars + '<option>' + key + '</option>'
    	});

    	Object.keys(course.labs).forEach(function(key, index) {
      		labs = labs + '<option>' + key + '</option>'
    	});

    $(".class-column").append(
      '<div class="added-class" id="' + course.varname + '">' +
      '<div class="top">' +
      '<div>' +
      '<p class="return-field class-title">' + course.name + '</p>' +
      '<div class="buttons">' +
      '<div class="button minus-button">&#8722;</div>' +
      '<div class="button expand-button">&#8595;</div>' +
      '</div>' +
      '</div>' +
      '<p class="return-field short-desc">' + course.short + '</p>' +
      '<div class="choices-div">' +
      '<div class="choice">' +
      '<p>Lecture:</p>' +
      '<span class="lectures custom-dropdown">' +
      '<select id="' + course.varname + '_lecture' + '">' +
      lectures +
      '</select>' +
      '</span>' +
      '</div>' +
      '<div class="choice">' +
      '<p>Seminar:</p> ' +
      '<span class="lectures custom-dropdown">' +
      '<select id="' + course.varname + '_seminar' + '">' +
      seminars +
      '</select>' +
      '</span>' +
      '</div>' +
      '<div class="choice">' +
      '<p>Lab:</p> ' +
      '<span class="lectures custom-dropdown">' +
      '<select id="' + course.varname + '_lab' + '">' +
      labs +
      '</select>' +
      '</span>' +
      '</div>' +
      '</div>' +
      '</div>' +
      '<div class="more" style="display:none;">' +
      '<p class="return-field long-desc"><span>Description: </span>' +
      course.long +
      '</p>' +
      '</div>' +
      '</div>'
    );
    saveSchedule();
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

		var course = {
    		varname: name.replace(/\s/g,'').toLowerCase(),
    		name: name,
    		courseId: courseId,
    		short: courseTitle,
    		lectures: {},
    		seminars: {},
    		labs: {},
    		long: description
    	}

		sections.forEach(function(section) {
			//parse sections
			if(/^B/.test(section.section)){
				course.lectures[section.section] = {
					start: section.startTime,
					end: section.endTime,
					days: section.day,
					location: section.location
				}
			}
			else if(/^S/.test(section.section)){
				course.seminars[section.section] = {
					start: section.startTime,
					end: section.endTime,
					days: section.day,
					location: section.location
				}
			}
			else if(/^H/.test(section.section)){
				course.labs[section.section] = {
					start: section.startTime,
					end: section.endTime,
					days: section.day,
					location: section.location
				}
			}
			else{
				course.lectures[section.section] = {
					start: section.startTime,
					end: section.endTime,
					days: section.day,
					location: section.location
				}
			}	
		});

		window.searched_courses[course.varname] = course

		//Generate some info,  this can probably be simplified or replaced.  Mostly for testing
		var lectures = ""
    	var seminars = ""
    	var labs = ""

    	Object.keys(course.lectures).forEach(function(key, index) {
      		var section = course.lectures[key]
      		lectures = key + ", " + section.start + "-" + section.end + ", " + section.days + ", " + section.location + ";  "
   		 });

    	Object.keys(course.seminars).forEach(function(key, index) {
      		var section = course.seminars[key]
      		seminars = key + ", " + section.start + "-" + section.end + ", " + section.days + ", " + section.location + ";  "
   		 });

    	Object.keys(course.labs).forEach(function(key, index) {
      		var section = course.labs[key]
      		labs = key + ", " + section.start + "-" + section.end + ", " + section.days + ", " + section.location + ";  "
    	});

    $(".search-returns").append(
      '<div class="search-return" id="' + course.varname + '">' +
      '<div class="top">' +
      '<div>' +
      '<p class="return-field class-title">' + course.name + '</p>' +
      '<div class="buttons">' +
      '<div class="button plus-button">&#43;</div>' +
      '<div class="button expand-button">&#8595;</div>' +
      '</div>' +
      '</div>' +
      '<p class="return-field short-desc">' + course.short + '</p>' +
      '</div>' +
      '<div class="more" style="display:none;">' +
      '<p class="return-field long-desc"><span>Description: </span>' + course.long + '</p>' + '<p class="return-field section"><span>Lectures: </span>' + lectures + '</p>' +
      '<p class="return-field time "><span>Seminars: </span>' + seminars + '</p>' +
      '<p class="return-field days"><span>Labs: </span>' + labs + '</p>' +
      '</div>' +
      '</div>'
    );

	}}
	$.ajax(args);
}

function saveSchedule() {
	for (key in window.added_courses) {
		var course = window.added_courses[key];
		var lecture = $("#" + course.varname + "_lecture").val();
		var seminar = $("#" + course.varname + "_seminar").val();
		var lab = $("#" + course.varname + "_lab").val();

		// get last element from URL, should be schedule ID
		var url = window.location.href.split("/");
		url.pop(); // pop of trailing forward slash
		var scheduleId = url.pop();
		var data = { courseName:course.varname, lecture:lecture, seminar:seminar, lab:lab,
				 scheduleId:scheduleId, csrfmiddlewaretoken:window.CSRF_TOKEN };
		var args = { type:"POST", url:"/builder/save/", data:data};
		$.ajax(args);

	}
}
