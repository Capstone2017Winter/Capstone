"""
This file contains the views which handles requests to the web server.
Some requests are simple such as requesting a http file to render, others
are more complex such as requesting class information or saving a schedule.
"""

from django.shortcuts import render, get_object_or_404
from django.http import HttpResponse, HttpResponseRedirect, HttpResponseBadRequest
from django.urls import reverse
import json
from datetime import datetime

import re #regex

from .models import *
from .classtime import *

def index(request):
    return render(request, 'schedule_builder/index.html')

def image(request):
    return render(request, 'schedule_builder/image.html')

def user(request, username):
    """
    handle request to display user page
    """
    user = get_object_or_404(User, pk=username)
    if request.method == "GET": # handle getting a user
        return render(request, 'schedule_builder/user.html', {
            'user':user,
        })
    else: # handle creating the user
        sched_id = None
        if 'year' in request.POST and 'term' in request.POST: # create a new schedule
            new_sched = user.schedule_set.create(year=request.POST['year'], term=request.POST['term'])
            new_sched.save()
            sched_id = new_sched.id
        else:
            for schedule in user.schedule_set.all():
                _id = str(schedule.id)
                if _id in request.POST:
                    sched_id = _id
        return HttpResponseRedirect(reverse('schedule', args=(user.name, sched_id,))) 

def schedule(request, username, schedule_id):
    """
    handle request for rendering a schedule page
    """
    schedule = get_object_or_404(Schedule, user__name=username, id=schedule_id)
    return render(request, 'schedule_builder/schedule.html', {'schedule':schedule,})

def home(request):
    """
    handle request for website home page
    """
    user = None
    try:
        user = User.objects.get(pk=request.POST['user']) #try to get user
    except (KeyError):
        return render(request, 'schedule_builder/home.html') 
    except (User.DoesNotExist): # if user doesn't exist, we create it
        if not User.is_valid_name(request.POST['user']):
            return render(request, 'schedule_builder/home.html', {
                'error_message': "Invalid user name. User names can only contain letters and numbers",
            })
        user = User(name=request.POST['user'], degree=None)
        user.save()
    return HttpResponseRedirect(reverse('user', args=(user.name,)))

def course_info(request):
    """
    handle request for to retrieve course info for
    a specific class name
    """
    if request.method == "GET":
        get = request.GET.copy()
        if 'className' in get:
            className = get['className']
            params = ct_get_class_info(className) # call the classtime methods
            return HttpResponse(json.dumps(params),
                                content_type="application/json") # dump classtime data into json
    return HttpResponseBadRequest('Invalid GET Parmeters')

def course_sections(request):
    """
    handle request to get course section info
    for a specific course id and term
    """
    if request.method == "GET":
        get = request.GET.copy()
        if 'name' in get and 'termName' in get:
            #course_id = get['courseId'] #six digit course identifier used with classtime
            name = get['name']
            term_name = get['termName'] #i.e FALL 2016
            params = ct_get_class_sections(name, term_name)
            return HttpResponse(json.dumps(params), 
                                content_type="application/json")
    return HttpResponseBadRequest('Invalid GET Parameters')

def save_schedule(request):
    """
    handle request to save schedule
    """
    if request.method == "POST":
        post = request.POST.copy()
        scheduleId = post['scheduleId'] # get schedule id to save
        classList = post.getlist('classArray[]') # get classes to save
        
        schedule = get_object_or_404(Schedule, pk=scheduleId)
        schedule.classsection_set.clear()
        for classToSaveJSON in classList:
            classToSave = json.loads(classToSaveJSON)
            course = classToSave['course']
            lecture_section = classToSave['lecture'] if 'lecture' in classToSave else None
            seminar_section = classToSave['seminar'] if 'seminar' in classToSave else None
            lab_section = classToSave['lab'] if 'lab' in classToSave else None
            date_time_format = '%I:%M %p'
            myclass = None

            #create class if it doesn't exist
            courseName = course['name']
            short_description = course['short']
            long_description = course['long']
            try:
                myclass = MyClass.objects.get(class_code=courseName)
            except (MyClass.DoesNotExist):
                myclass = MyClass(class_code=courseName, short_description=short_description, long_description = long_description)             
                myclass.save()

            timeSlot = None
            #create lecture section if it doesn't exist and add it to schedule
            mySection = None
            if lecture_section != None:
                lecture_code = lecture_section['code']

                try:
                    mySection = ClassSection.objects.get(myclass=myclass, 
                                                     section_number=lecture_code, section_type='lecture')
                except (ClassSection.DoesNotExist):
                    mySection = myclass.classsection_set.create(section_number=lecture_code, section_type='lecture')
                    mySection.save()
                mySection.schedule.add(schedule)
                startTime = datetime.strptime(lecture_section['start'], date_time_format)
                endTime = datetime.strptime(lecture_section['end'], date_time_format)
                day = lecture_section['days']
                try:
                    timeSlot = TimeSlot.objects.get(start_time=startTime, end_time=endTime, day=day)
                except (TimeSlot.DoesNotExist):
                    timeSlot = mySection.timeslot_set.create(start_time=startTime, end_time=endTime, day=day)
                    timeSlot.save()
                timeSlot.class_section.add(mySection)

            #create seminar section if it doesn't exist and add it to schedule
            if seminar_section != None:
                seminar_code = seminar_section['code']
                try:
                    mySection = ClassSection.objects.get(myclass=myclass, 
                                                     section_number=seminar_code, section_type='seminar')
                except (ClassSection.DoesNotExist):
                    mySection = myclass.classsection_set.create(section_number=seminar_code, section_type='seminar')
                    mySection.save()
                mySection.schedule.add(schedule)
                startTime = datetime.strptime(seminar_section['start'], date_time_format)
                endTime = datetime.strptime(seminar_section['end'], date_time_format)
                day = seminar_section['days']
                try:
                    timeSlot = TimeSlot.objects.get(start_time=startTime, end_time=endTime, day=day)
                except (TimeSlot.DoesNotExist):
                    timeSlot = mySection.timeslot_set.create(start_time=startTime, end_time=endTime, day=day)
                    timeSlot.save()
                timeSlot.class_section.add(mySection)

            #create lab section if it doesn't exist and add it to schedule
            if lab_section != None:
                lab_code = lab_section['code']
                try:
                    mySection = ClassSection.objects.get(myclass=myclass, 
                                                     section_number=lab_code, section_type='lab')
                except (ClassSection.DoesNotExist):
                    mySection = myclass.classsection_set.create(section_number=lab_code, section_type='lab')
                    mySection.save()
                mySection.schedule.add(schedule)
                startTime = datetime.strptime(lab_section['start'], date_time_format)
                endTime = datetime.strptime(lab_section['end'], date_time_format)
                day = lab_section['days']
                try:
                    timeSlot = TimeSlot.objects.get(start_time=startTime, end_time=endTime, day=day)
                except (TimeSlot.DoesNotExist):
                    timeSlot = mySection.timeslot_set.create(start_time=startTime, end_time=endTime, day=day)
                    timeSlot.save()
                timeSlot.class_section.add(mySection)
    return HttpResponse(status=200)

def load_schedule(request):
    """
    handle request to load a schedule that was previously saved
    """
    if request.method == "GET":
        get = request.GET.copy()
        if not 'scheduleId' in get:
            return HttpResponseBadRequest('Missing "scheduleId" parameter')
        scheduleId = get['scheduleId']
        schedule = get_object_or_404(Schedule, pk=scheduleId)
        
        # load all class sections and associate them with the className
        loaded_classes = {}
        for section in schedule.classsection_set.all():
            className = section.myclass.class_code
            sectionType = section.section_type
            sectionNumber = section.section_number
            if not className in loaded_classes:
                loaded_classes[className] = {}
            loaded_classes[className][sectionType] = sectionNumber

        # reduce class sections (lecture, seminar, lab) into a single class object
        class_list = []
        for className, loaded_class in loaded_classes.items():
            loaded_class['courseName'] = className
            class_list.append(loaded_class)

        return HttpResponse(json.dumps(class_list), 
                                content_type="application/json")

def completed_classes(request):
    """
    handle request to get all completed classes for a user
    """
    if request.method == "GET":
        get = request.GET.copy()
        if not 'username' in get:
            return HttpResponseBadRequest('Missing "username" parameter')
        username = get['username']
        completed_classes = []
        schedule_set = Schedule.objects.filter(user__name=username)
        for schedule in schedule_set:
            #schedule = schedule_set[key]
            for section in schedule.classsection_set.all():
                completed_classes.append(section.myclass.class_code)
        return HttpResponse(json.dumps(completed_classes), content_type="application/json")
    return HttpResponseBadRequest('This method only handles GET requests')


