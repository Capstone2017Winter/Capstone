$(document).ready(function(){

	window.searched_courses = {}
	window.added_courses = {}
	window.event_colors = ["event-8", "event-3", "event-6", "event-5", "event-4", "event-7", "event-2", "event-1"];
	window.objSchedulesPlan = [];

  $('#downloadButton').closest('.form-div').toggle();

	var schedules = $('.cd-schedule'),
    windowResize = false;

	var transitionEnd = 'webkitTransitionEnd otransitionend oTransitionEnd msTransitionEnd transitionend';
  	var transitionsSupported = ($('.csstransitions').length > 0);
  	//if browser does not support transitions - use a different event to trigger them
  	if (!transitionsSupported) transitionEnd = 'noTransition';

  	if (schedules.length > 0) {
    	schedules.each(function() {
      	//create SchedulePlan objects
      	window.objSchedulesPlan.push(new SchedulePlan($(this)));
    	});
  	}

  $(window).on('resize', function() {
    if (!windowResize) {
      windowResize = true;
      (!window.requestAnimationFrame) ? setTimeout(checkResize): window.requestAnimationFrame(checkResize);
    }
  });

  $(window).keyup(function(event) {
    if (event.keyCode == 27) {
      window.objSchedulesPlan.forEach(function(element) {
        element.closeModal(element.eventsGroup.find('.selected-event'));
      });
    }
  });

  loadSchedule();
  
  $('.class-column').on('change', 'select', function() {
    var change_id = $(this).closest('div[class^="added-class"]').attr('id');
    var course = added_courses[change_id];
    var type_changed = $(this).closest('span').attr('class').replace(" custom-dropdown", "");
    var changed_course = $(".class-column").find('#' + course.varname);

    $('.events').find('.single-event.' + change_id + '.' + type_changed).remove();
    colorSyncPlace(course, type_changed);
  });

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
		var args = { type:"GET", url:"/builder/class/", data:data, complete:searchClassCallback };
		$.ajax(args);
	});
	
	$('#saveButton').click(function(){
		saveSchedule();

	});

	$('#downloadButton').click(function(){
		downloadScheduleToBoard();
	});	

	$(".search-returns").on('click', '.expand-button', function() {
    		$(this).closest('div[class^="search-return"]').children('.more').toggle();
  	});

  	$(".class-column").on('click', '.expand-button', function() {
    		$(this).closest('div[class^="added-class"]').children('.more').toggle();
  	});

  	$(".class-column").on('click', '.minus-button', function() {
  		var removed_course = $(this).closest('div[class^="added-class"]');
    
    	window.event_colors.push(removed_course.attr('data-event'));
    	delete window.added_courses[removed_course.attr('id')];
    
    	$(".events-group").find("." + removed_course.attr('id')).remove();
    	removed_course.remove();
      checkConflicts();
  	});

  	$('.search-returns').on('click', '.plus-button', function() {
    	var course = window.searched_courses[$(this).closest('div[class^="search-return"]').attr('id')];

      if(!window.added_courses[course.varname]) {
        addClassToSchedule(course);
        colorSyncPlace(course);
      } 
  	});

  	function checkResize() {
    window.objSchedulesPlan.forEach(function(element) {
      element.scheduleReset();
    });
    windowResize = false;
	}
});

jQuery.fn.extend({
    syncWithDropDowns: function(course, event_color, type_changed = null) {
      var section_types;
      if (type_changed != null) {
        //This is for when a class drop down is changed
        section_types = [type_changed];
      } else {
        //This is for adding a class
        var section_types = ["lectures", "seminars", "labs"];
      }

      for (var section_type in section_types) {
        var dd = $(this).find("." + section_types[section_type]).find("select");
        var section;

        if (section_types[section_type] == "lectures") {
          section = course.lectures[dd.val()];
        } else if (section_types[section_type] == "seminars") {
          section = course.seminars[dd.val()];
        } else if (section_types[section_type] == "labs") {
          section = course.labs[dd.val()];
        }

        if (section == null) {
          continue;
        }

        var schedule_block = buildScheduleBlock(course, section, section_types[section_type], dd.val(), event_color);
        var days_selector = "";

        for (var i = 0, len = section.days.length; i < len; i++) {
          days_selector = days_selector + "#" + section.days[i] + ",";
        }

        days_selector = days_selector.slice(0, -1);
        $(days_selector).each(function() {
          $(this).append(schedule_block);
        });

      }

    }
});

function buildScheduleBlock(course, section, section_type, section_number, event_color) {
    var start = timeTo24(section.start)
    var end = timeTo24(section.end);

    var schedule_section = '<li class="single-event ' +
      course.varname + ' ' + section_type +
      '" data-start="' + start.trim() +
      '" data-end="' + end.trim() +
      '" data-content="" data-event="' + event_color +'">' +
      '<a href="#0">' +
      '<em class="event-name">' + course.name + '</em>' +
      '<p>' + section_type.slice(0, -1).capitalize() +
      " " + section_number + '</p>' +
      '<p>' + section.location + '</p>' +
      '</a>' +
      '</li>';

    return schedule_section;
}

function buildAddedClassBlock(course){
    var added_course = '<div class="added-class" id="' + course.varname + '">' +
      '<div class="top">' +
      '<div>' +
      '<p class="return-field class-title">' + course.name + '</p>' +
      '<div class="buttons">' +
      '<div class="button minus-button">&#8722;</div>' +
      '<div class="button expand-button">&#8595;</div>' +
      '</div>' +
      '</div>' +
      '<p class="return-field short-desc">' + course.short + '</p>' +
      '<div class="choices-div">' ;

    if(!jQuery.isEmptyObject(course.lectures)){
    	var lectures = ""

    	Object.keys(course.lectures).forEach(function(key, index) {
    		lectures += '<option>' + key + '</option>'
		});

		added_course += '<div class="choice">' +
      	'<p>Lecture:</p>' +
      	'<span class="lectures custom-dropdown">' +
      	'<select id="' + course.varname + '_lecture' + '">' +
      	lectures +
      	'</select>' +
      	'</span>' +
      	'</div>' ;
    }

    if(!jQuery.isEmptyObject(course.seminars)){
    	var seminars = ""

    	Object.keys(course.seminars).forEach(function(key, index) {
    		seminars += '<option>' + key + '</option>'
		});

		added_course += '<div class="choice">' +
      	'<p>Seminar:</p>' +
      	'<span class="seminars custom-dropdown">' +
      	'<select id="' + course.varname + '_seminar' + '">' +
      	seminars +
      	'</select>' +
      	'</span>' +
      	'</div>' ;
    }

    if(!jQuery.isEmptyObject(course.labs)){
    	var labs = ""

    	Object.keys(course.labs).forEach(function(key, index) {
    		labs += '<option>' + key + '</option>'
		});

		added_course += '<div class="choice">' +
      	'<p>Lab:</p>' +
      	'<span class="labs custom-dropdown">' +
      	'<select id="' + course.varname + '_lab' + '">' +
      	labs +
      	'</select>' +
      	'</span>' +
      	'</div>' ;
    }
    added_course += '</div></div>' //close choices-div and top-div

    added_course += '<div class="more" style="display:none;">' +
      '<p class="return-field long-desc"><span>Description: </span>' +
      course.long +
      '</p>' +
      '</div>' +
      '</div>'
    
    return added_course;
  }

String.prototype.capitalize = function() {
    return this.replace(/(?:^|\s)\S/g, function(a) {
      return a.toUpperCase();
    });
};


function checkConflicts(){
  $('#M,#T,#W,#R,#F').each(function(){
    var day_events = $(this).find('.single-event');
     
    $(day_events).each(function(){
      $(this).removeClass('conflicted');
    });
      
    $(day_events).each(function(){
      var event = this;
      var conflicted = [];
      $(day_events).not(this).each(function(){
        if(isConflict(event, this)){
          conflicted.push(this);
        }
      });
       
      if(conflicted.length != 0){
        conflicted.push(event);
        conflictPlace(conflicted);
      }
        
      if(!$(this).hasClass('conflicted')){
        $(this).width('100%');
        $(this).css('margin-left', '0px');
      }
    });
  });
}
  
  function conflictPlace(conflicted_array){
    var width_percent = (100/conflicted_array.length);
    var offset_percent = 0;
    var offset_increment = width_percent;
    
    for(var i in conflicted_array){
      var event = conflicted_array[i];
      $(event).addClass('conflicted');
      $(event).width(width_percent + '%');
      $(event).css('margin-left', offset_percent + '%');
      offset_percent += offset_increment;
    }
  }
  
  function isConflict(event1, event2){
    var start_1 = $(event1).attr('data-start').replace(":", "");
    var end_1 = $(event1).attr('data-end').replace(":", "");
    var start_2 = $(event2).attr('data-start').replace(":", "");
    var end_2 = $(event2).attr('data-end').replace(":", "");

    
    if((end_1 < start_2) || (start_1 > end_2)){
        return false;
    }
    //Added ends as the next event starts
    else if((end_1 == start_2) && (start_1 < start_2)){
        return false;
    }
    //Added starts as previous event ends
    else if((start_1 == end_2) && (end_1 > end_2)){
        return false;
    }
    return true;
  }

function timeTo24(time) {
    if (/AM/.test(time)) {
      time = time.replace(/AM/, "");
      return time;
    } else {
      time = time.replace(/PM/, "");
      if(/12:/.test(time)){return time;}
      time = time.split(/:/);
      time[0] = (+time[0] + 12).toString();
      return time[0] + ":" + time[1];
    }
}

function addClassToSchedule(course) {
   	if (!window.added_courses[course.varname]) {
  		window.added_courses[course.varname] = course;
   	} 
    else {
      return;
    } 

    var event_color = window.event_colors.pop();
    var added_course = buildAddedClassBlock(course);

    $(".class-column").append(added_course);
    $(".class-column").find('#' + course.varname).attr('data-event', event_color); 
}

function colorSyncPlace(course, type_changed=null){
	
	var event_color = $('.class-column').find('#' + course.varname).attr('data-event');
	$(".class-column").find('#' + course.varname).find('.choices-div').syncWithDropDowns(course, event_color, type_changed);

	for (var plan in window.objSchedulesPlan) {
   		window.objSchedulesPlan[plan].updateSchedule();
    }
  checkConflicts();
}

function searchClassCallback(response, status) {
	var json = response.responseJSON;
	var first = json.objects[0];
	var name = first.asString;
	var description = first.courseDescription;
	var courseTitle = first.courseTitle;
	var courseId = first.course;
	var termName = term + " " + year;

	var data = { courseId:courseId, termName:termName, name:name,csrfmiddlewaretoken:window.CSRF_TOKEN};
	var args = { type:"GET", url:"/builder/section/", data:data, complete:
	function(resp){
		var json1 = resp.responseJSON;
    var sections = JSON.parse(json1);

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


    for(var i in sections){
      //parse sections
      var section = sections[i];

      if(section.component == "LEC"){
        course.lectures[section.section] = {
          start: section.startTime,
          end: section.endTime,
          days: section.day,
          location: section.location
        }
      }
      else if(section.component == "SEM"){
        course.seminars[section.section] = {
          start: section.startTime,
          end: section.endTime,
          days: section.day,
          location: section.location
        }
      }
      else if(section.component == "LAB"){
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
    }

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
		'</div>');

	} /*end function*/    } /*end args*/
	$.ajax(args);
}

function searchClassLoadCallback(response, status, lecture, seminar, lab) {
	var json = response.responseJSON;
	var first = json.objects[0];
	var name = first.asString;
	var description = first.courseDescription;
	var courseTitle = first.courseTitle;
	var courseId = first.course;
	var termName = term + " " + year;

	var data = { courseId:courseId, termName:termName, name:name, csrfmiddlewaretoken:window.CSRF_TOKEN};
	var args = { type:"GET", url:"/builder/section/", data:data, complete:
	function(resp){
		var json1 = resp.responseJSON;
    var sections = JSON.parse(json1);

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

    for(var i in sections){
      //parse sections
      var section = sections[i];

      if(section.component == "LEC"){
        course.lectures[section.section] = {
          start: section.startTime,
          end: section.endTime,
          days: section.day,
          location: section.location
        }
      }
      else if(section.component == "SEM"){
        course.seminars[section.section] = {
          start: section.startTime,
          end: section.endTime,
          days: section.day,
          location: section.location
        }
      }
      else if(section.component == "LAB"){
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
    }

		window.searched_courses[course.varname] = course
		addClassToSchedule(course);
		if (lecture != '')
			$("#" + course.varname + "_lecture").val(lecture);
		if (seminar != '')
			$("#" + course.varname + "_seminar").val(seminar);
		if (lab != '')
			$("#" + course.varname + "_lab").val(lab);

		colorSyncPlace(course);
	} /*end function*/    
	} /*end args*/
	$.ajax(args);
}

function loadScheduleCallback(response, status) {
  if (status != "success") {
		alert("loadScheduleCallback got back a bad status");
		return;
	}
	var json = response.responseJSON;
	for (var index in json) {
		var loadedClass = json[index];
		var name = loadedClass['courseName'];
		var lecture = ('lecture' in loadedClass) ? loadedClass['lecture'] : '';
		var seminar = ('seminar' in loadedClass) ? loadedClass['seminar'] : '';
		var lab = ('lab' in loadedClass) ? loadedClass['lab'] : '';
		var data = { className:name, csrfmiddlewaretoken:window.CSRF_TOKEN};
		var args = { type:"GET", url:"/builder/class/", data:data, complete: 
			function (lecture, seminar, lab) {
				return function (response, status) {
					searchClassLoadCallback(response, status, lecture, seminar, lab);
				};
			}(lecture, seminar, lab)
		};
		$.ajax(args);
	}
}

function saveSchedule() {
	var classArray = new Array(); // add with .push()
	for (var key in window.added_courses) {
		var course = window.added_courses[key];
		var lectureCode = $("#" + course.varname + "_lecture").val();
		var seminarCode = $("#" + course.varname + "_seminar").val();
		var labCode = $("#" + course.varname + "_lab").val();
		var lecture;
		var seminar;
		var lab;
		if (lectureCode != null) {
			lecture = course.lectures[lectureCode];
			lecture['code'] = lectureCode;
	        }
		if (seminarCode != null) {
			seminar = course.seminars[seminarCode];
			seminar['code'] = seminarCode;	
		}
		if (labCode != null) {
			lab = course.labs[labCode];
			lab['code'] = labCode;
		}
		var myClass = { course:course, lecture:lecture, seminar:seminar, lab:lab };
		classArray.push(JSON.stringify(myClass));
	}
	var scheduleId = getScheduleId();	
	var data = { classArray:classArray, scheduleId:scheduleId, csrfmiddlewaretoken:window.CSRF_TOKEN };
	var args = { type:"POST", url:"/builder/save/", data:data};
	$.ajax(args);
	alert("Schedule Saved!");
}

function downloadScheduleToBoard() {
	var scheduleNode = document.getElementById("scheduleDiv")
	var desiredWidth = 320;
 	var desiredHeight = 240;
 	var scalex = desiredWidth / scheduleNode.clientWidth;
 	var scaley = desiredHeight / scheduleNode.clientHeight; 
	domtoimage.toBmp(scheduleNode, {scalex:scalex, scaley:scaley, bgcolor:'white'})
		.then(function (dataUrl) {
			//owencm, http://stackoverflow.com/questions/3916191/download-data-url-file
			var newImg = '<img src="' + dataUrl + '"/>';
                        $('body').append(newImg);

			var link = document.createElement("a");
			link.download = "schedule.bmp";
			link.href = dataUrl;
			document.body.appendChild(link);
			link.click();
			document.body.removeChild(link);
			delete link;
		});
}

function loadSchedule() {
	var scheduleId = getScheduleId();
	var data = { scheduleId:scheduleId, crsfmiddlewaretoken:window.CSRF_TOKEN };
	var args = { type:"GET", url:"/builder/load/", data:data, complete:loadScheduleCallback };
	$.ajax(args);
}

function getScheduleId() {
	var url = window.location.href.split("/");
	url.pop(); // pop off trailing forward slash
	return url.pop();
}


/*Codes used from
 *https://codyhouse.co/gem/schedule-template/
*/
function SchedulePlan(element) {
    this.element = element;
    this.timeline = this.element.find('.timeline');
    this.timelineItems = this.timeline.find('li');
    this.timelineItemsNumber = this.timelineItems.length;
    this.timelineStart = getScheduleTimestamp(this.timelineItems.eq(0).text());
    //need to store delta (in our case half hour) timestamp
    this.timelineUnitDuration = getScheduleTimestamp(this.timelineItems.eq(1).text()) - getScheduleTimestamp(this.timelineItems.eq(0).text());

    this.eventsWrapper = this.element.find('.events');
    this.eventsGroup = this.eventsWrapper.find('.events-group');
    this.singleEvents = this.eventsGroup.find('.single-event');
    //this.eventSlotHeight = this.eventsGroup.eq(0).children('.top-info').outerHeight();
    this.eventSlotHeight = this.timelineItems.eq(0).outerHeight();

    this.modal = this.element.find('.event-modal');
    this.modalHeader = this.modal.find('.header');
    this.modalHeaderBg = this.modal.find('.header-bg');
    this.modalBody = this.modal.find('.body');
    this.modalBodyBg = this.modal.find('.body-bg');
    this.modalMaxWidth = 800;
    this.modalMaxHeight = 480;

    this.animating = false;

    this.initSchedule();
}

SchedulePlan.prototype.initSchedule = function() {
	this.scheduleReset();
    this.initEvents();
}

SchedulePlan.prototype.updateSchedule = function() {
    this.singleEvents = this.eventsGroup.find('.single-event');
    this.placeEvents();
}

SchedulePlan.prototype.scheduleReset = function() {
    var mq = this.mq();
    if (mq == 'desktop' && !this.element.hasClass('js-full')) {
      //in this case you are on a desktop version (first load or resize from mobile)
      //this.eventSlotHeight = this.eventsGroup.eq(0).children('.top-info').outerHeight();
      this.eventSlotHeight = this.timelineItems.eq(0).outerHeight();
      this.element.addClass('js-full');
      this.placeEvents();
      this.element.hasClass('modal-is-open') && this.checkEventModal();
    } else if (mq == 'mobile' && this.element.hasClass('js-full')) {
      //in this case you are on a mobile version (first load or resize from desktop)
      this.element.removeClass('js-full loading');
      this.eventsGroup.children('ul').add(this.singleEvents).removeAttr('style');
      this.eventsWrapper.children('.grid-line').remove();
      this.element.hasClass('modal-is-open') && this.checkEventModal();
    } else if (mq == 'desktop' && this.element.hasClass('modal-is-open')) {
      //on a mobile version with modal open - need to resize/move modal window
      this.checkEventModal('desktop');
      this.element.removeClass('loading');
    } else {
      this.element.removeClass('loading');
    }
}

SchedulePlan.prototype.initEvents = function() {
    var self = this;

    this.singleEvents.each(function() {
      //create the .event-date element for each event
      //var durationLabel = '<span class="event-date">'+$(this).data('start')+' - '+$(this).data('end')+'</span>';
      //$(this).children('a').prepend($(durationLabel));

      //detect click on the event and open the modal
      $(this).on('click', 'a', function(event) {
        event.preventDefault();
        if (!self.animating) self.openModal($(this));
      });
    });

    //close modal window
    this.modal.on('click', '.close', function(event) {
      event.preventDefault();
      if (!self.animating) self.closeModal(self.eventsGroup.find('.selected-event'));
    });
    this.element.on('click', '.cover-layer', function(event) {
      if (!self.animating && self.element.hasClass('modal-is-open')) self.closeModal(self.eventsGroup.find('.selected-event'));
    });
}

SchedulePlan.prototype.placeEvents = function() {
    var self = this;
    this.singleEvents.each(function() {
      //place each event in the grid -> need to set top position and height
      var start = getScheduleTimestamp($(this).attr('data-start')),
        duration = getScheduleTimestamp($(this).attr('data-end')) - start;

      var eventTop = self.eventSlotHeight * (start - self.timelineStart) / self.timelineUnitDuration,
        eventHeight = self.eventSlotHeight * duration / self.timelineUnitDuration;

      $(this).css({
        top: (eventTop - 1) + 'px',
        height: (eventHeight + 1) + 'px'
      });
    });

    this.element.removeClass('loading');
}

SchedulePlan.prototype.openModal = function(event) {
    var self = this;
    var mq = self.mq();
    this.animating = true;

    //update event name and time
    this.modalHeader.find('.event-name').text(event.find('.event-name').text());
    this.modalHeader.find('.event-date').text(event.find('.event-date').text());
    this.modal.attr('data-event', event.parent().attr('data-event'));

    //update event content
    this.modalBody.find('.event-info').load(event.parent().attr('data-content') + '.html .event-info > *', function(data) {
      //once the event content has been loaded
      self.element.addClass('content-loaded');
    });

    this.element.addClass('modal-is-open');

    setTimeout(function() {
      //fixes a flash when an event is selected - desktop version only
      event.parent('li').addClass('selected-event');
    }, 10);

    if (mq == 'mobile') {
      self.modal.one(transitionEnd, function() {
        self.modal.off(transitionEnd);
        self.animating = false;
      });
    } else {
      var eventTop = event.offset().top - $(window).scrollTop(),
        eventLeft = event.offset().left,
        eventHeight = event.innerHeight(),
        eventWidth = event.innerWidth();

      var windowWidth = $(window).width(),
        windowHeight = $(window).height();

      var modalWidth = (windowWidth * .8 > self.modalMaxWidth) ? self.modalMaxWidth : windowWidth * .8,
        modalHeight = (windowHeight * .8 > self.modalMaxHeight) ? self.modalMaxHeight : windowHeight * .8;

      var modalTranslateX = parseInt((windowWidth - modalWidth) / 2 - eventLeft),
        modalTranslateY = parseInt((windowHeight - modalHeight) / 2 - eventTop);

      var HeaderBgScaleY = modalHeight / eventHeight,
        BodyBgScaleX = (modalWidth - eventWidth);

      //change modal height/width and translate it
      self.modal.css({
        top: eventTop + 'px',
        left: eventLeft + 'px',
        height: modalHeight + 'px',
        width: modalWidth + 'px',
      });
      transformElement(self.modal, 'translateY(' + modalTranslateY + 'px) translateX(' + modalTranslateX + 'px)');

      //set modalHeader width
      self.modalHeader.css({
        width: eventWidth + 'px',
      });
      //set modalBody left margin
      self.modalBody.css({
        marginLeft: eventWidth + 'px',
      });

      //change modalBodyBg height/width ans scale it
      self.modalBodyBg.css({
        height: eventHeight + 'px',
        width: '1px',
      });
      transformElement(self.modalBodyBg, 'scaleY(' + HeaderBgScaleY + ') scaleX(' + BodyBgScaleX + ')');

      //change modal modalHeaderBg height/width and scale it
      self.modalHeaderBg.css({
        height: eventHeight + 'px',
        width: eventWidth + 'px',
      });
      transformElement(self.modalHeaderBg, 'scaleY(' + HeaderBgScaleY + ')');

      self.modalHeaderBg.one(transitionEnd, function() {
        //wait for the  end of the modalHeaderBg transformation and show the modal content
        self.modalHeaderBg.off(transitionEnd);
        self.animating = false;
        self.element.addClass('animation-completed');
      });
    }

    //if browser do not support transitions -> no need to wait for the end of it
    if (!transitionsSupported) self.modal.add(self.modalHeaderBg).trigger(transitionEnd);
}

SchedulePlan.prototype.closeModal = function(event) {
    var self = this;
    var mq = self.mq();

    this.animating = true;

    if (mq == 'mobile') {
      this.element.removeClass('modal-is-open');
      this.modal.one(transitionEnd, function() {
        self.modal.off(transitionEnd);
        self.animating = false;
        self.element.removeClass('content-loaded');
        event.removeClass('selected-event');
      });
    } else {
      var eventTop = event.offset().top - $(window).scrollTop(),
        eventLeft = event.offset().left,
        eventHeight = event.innerHeight(),
        eventWidth = event.innerWidth();

      var modalTop = Number(self.modal.css('top').replace('px', '')),
        modalLeft = Number(self.modal.css('left').replace('px', ''));

      var modalTranslateX = eventLeft - modalLeft,
        modalTranslateY = eventTop - modalTop;

      self.element.removeClass('animation-completed modal-is-open');

      //change modal width/height and translate it
      this.modal.css({
        width: eventWidth + 'px',
        height: eventHeight + 'px'
      });
      transformElement(self.modal, 'translateX(' + modalTranslateX + 'px) translateY(' + modalTranslateY + 'px)');

      //scale down modalBodyBg element
      transformElement(self.modalBodyBg, 'scaleX(0) scaleY(1)');
      //scale down modalHeaderBg element
      transformElement(self.modalHeaderBg, 'scaleY(1)');

      this.modalHeaderBg.one(transitionEnd, function() {
        //wait for the  end of the modalHeaderBg transformation and reset modal style
        self.modalHeaderBg.off(transitionEnd);
        self.modal.addClass('no-transition');
        setTimeout(function() {
          self.modal.add(self.modalHeader).add(self.modalBody).add(self.modalHeaderBg).add(self.modalBodyBg).attr('style', '');
        }, 10);
        setTimeout(function() {
          self.modal.removeClass('no-transition');
        }, 20);

        self.animating = false;
        self.element.removeClass('content-loaded');
        event.removeClass('selected-event');
      });
    }

    //browser do not support transitions -> no need to wait for the end of it
    if (!transitionsSupported) self.modal.add(self.modalHeaderBg).trigger(transitionEnd);
}

SchedulePlan.prototype.mq = function() {
    //get MQ value ('desktop' or 'mobile') 
    var self = this;
    return window.getComputedStyle(this.element.get(0), '::before').getPropertyValue('content').replace(/["']/g, '');
}

SchedulePlan.prototype.checkEventModal = function(device) {
    this.animating = true;
    var self = this;
    var mq = this.mq();

    if (mq == 'mobile') {
      //reset modal style on mobile
      self.modal.add(self.modalHeader).add(self.modalHeaderBg).add(self.modalBody).add(self.modalBodyBg).attr('style', '');
      self.modal.removeClass('no-transition');
      self.animating = false;
    } else if (mq == 'desktop' && self.element.hasClass('modal-is-open')) {
      self.modal.addClass('no-transition');
      self.element.addClass('animation-completed');
      var event = self.eventsGroup.find('.selected-event');

      var eventTop = event.offset().top - $(window).scrollTop(),
        eventLeft = event.offset().left,
        eventHeight = event.innerHeight(),
        eventWidth = event.innerWidth();

      var windowWidth = $(window).width(),
        windowHeight = $(window).height();

      var modalWidth = (windowWidth * .8 > self.modalMaxWidth) ? self.modalMaxWidth : windowWidth * .8,
        modalHeight = (windowHeight * .8 > self.modalMaxHeight) ? self.modalMaxHeight : windowHeight * .8;

      var HeaderBgScaleY = modalHeight / eventHeight,
        BodyBgScaleX = (modalWidth - eventWidth);

      setTimeout(function() {
        self.modal.css({
          width: modalWidth + 'px',
          height: modalHeight + 'px',
          top: (windowHeight / 2 - modalHeight / 2) + 'px',
          left: (windowWidth / 2 - modalWidth / 2) + 'px',
        });
        transformElement(self.modal, 'translateY(0) translateX(0)');
        //change modal modalBodyBg height/width
        self.modalBodyBg.css({
          height: modalHeight + 'px',
          width: '1px',
        });
        transformElement(self.modalBodyBg, 'scaleX(' + BodyBgScaleX + ')');
        //set modalHeader width
        self.modalHeader.css({
          width: eventWidth + 'px',
        });
        //set modalBody left margin
        self.modalBody.css({
          marginLeft: eventWidth + 'px',
        });
        //change modal modalHeaderBg height/width and scale it
        self.modalHeaderBg.css({
          height: eventHeight + 'px',
          width: eventWidth + 'px',
        });
        transformElement(self.modalHeaderBg, 'scaleY(' + HeaderBgScaleY + ')');
      }, 10);

      setTimeout(function() {
        self.modal.removeClass('no-transition');
        self.animating = false;
      }, 20);
    }
}

function checkResize() {
    window.objSchedulesPlan.forEach(function(element) {
      element.scheduleReset();
    });
    windowResize = false;
}

function getScheduleTimestamp(time) {
    //accepts hh:mm format - convert hh:mm to timestamp
    time = time.replace(/ /g, '');
    var timeArray = time.split(':');
    var timeStamp = parseInt(timeArray[0]) * 60 + parseInt(timeArray[1]);
    return timeStamp;
}

function transformElement(element, value) {
    element.css({
      '-moz-transform': value,
      '-webkit-transform': value,
      '-ms-transform': value,
      '-o-transform': value,
      'transform': value
    });
}
