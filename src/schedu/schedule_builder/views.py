from django.shortcuts import render, get_object_or_404
from django.http import HttpResponse, HttpResponseRedirect
from django.urls import reverse

import re #regex

from .models import * 
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
    return HttpResponse('You are looking at the schedule for user: {} with id: {}'.format(schedule.user.name, schedule.id))

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
