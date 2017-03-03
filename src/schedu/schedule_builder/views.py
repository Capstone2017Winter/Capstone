from django.shortcuts import render, get_object_or_404
from django.http import HttpResponse, HttpResponseRedirect, HttpResponseBadRequest
from django.urls import reverse
import json

import re #regex

from .models import *
from .classtime import * 
# Create your views here.

def index(request):
    return render(request, 'schedule_builder/index.html')

def image(request):
    return render(request, 'schedule_builder/image.html')

def user(request, username):
    user = get_object_or_404(User, pk=username)
    if request.method == "GET":
        return render(request, 'schedule_builder/user.html', {
            'user':user,
        })
    else:
        sched_id = None
        if 'year' in request.POST and 'term' in request.POST:
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
    schedule = get_object_or_404(Schedule, user__name=username, id=schedule_id)
    return render(request, 'schedule_builder/schedule.html', {'schedule':schedule,})

#work in progress, this will reload a saved schedule
def load_schedule(request):
    get = request.GET.copy()
    if 'username' in get and 'year' in get and 'term' in get:
        schedule = get_object_or_404(Schedule, user__name=get['username'], year='year', term='term')
        
    else:
        raise HttpResponseBadRequest('Invalid GET Parameters')

def home(request):
    user = None
    try:
        user = User.objects.get(pk=request.POST['user'])
    except (KeyError):
        return render(request, 'schedule_builder/home.html')
    except (User.DoesNotExist):
        if not User.is_valid_name(request.POST['user']):
            return render(request, 'schedule_builder/home.html', {
                'error_message': "Invalid user name. User names can only contain letters and numbers",
            })
        user = User(name=request.POST['user'], degree=None)
        user.save()
    return HttpResponseRedirect(reverse('user', args=(user.name,)))

def course_info(request):
    if request.method == "GET":
        get = request.GET.copy()
        if 'className' in get:
            className = get['className']
            params = ct_get_class_info(className)
            #create class and do stuff with params
            return HttpResponse(json.dumps(params),
                                content_type="application/json")
    return HttpResponseBadRequest('Invalid GET Parmeters')

def course_sections(request):
    if request.method == "GET":
        get = request.GET.copy()
        if 'courseId' in get and 'termName' in get:
            course_id = get['courseId'] #six digit course identifier used with classtime
            term_name = get['termName'] #i.e FALL 2016
            params = ct_get_class_sections(course_id, term_name)
            return HttpResponse(json.dumps(params), 
                                content_type="application/json")
    return HttpResponseBadRequest('Invalid GET Parameters')

def save_schedule(request):
    if request.method == "POST":
        post = request.POST.copy()
        scheduleId = post['scheduleId']
        courseName = post['courseName']
        lecture = post['lecture']
      
        schedule = get_object_or_404(Schedule, pk=scheduleId)
        myclass = None
        try:
            myclass = MyClass.objects.get(class_code=courseName)
        except (MyClass.DoesNotExist):
            myclass = MyClass(class_code=courseName, class_description="")
            myclass.save()
        mySection = None
        try:
            mySection = ClassSection.objects.get(schedule=schedule, myclass=myclass, 
                                            section_number=lecture)
        except (ClassSection.DoesNotExist):
            mysection = myclass.classsection_set.create(section_number=lecture)
            mysection.save()
