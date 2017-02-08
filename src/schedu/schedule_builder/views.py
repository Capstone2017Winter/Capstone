from django.shortcuts import render, get_object_or_404
from django.http import HttpResponse, HttpResponseRedirect
from django.urls import reverse

from .models import * 
# Create your views here.

def index(request):
    return render(request, 'schedule_builder/index.html')

def image(request):
    return render(request, 'schedule_builder/image.html')

def user(request, username):
    user = get_object_or_404(User, pk=username)
    return HttpResponse('You are looking at the page for user: {}'.format(user.name))

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
        #todo, validate username is valid characters
        user = User(name=request.POST['user'], degree=None)
        user.save()
    return HttpResponseRedirect(reverse('user', args=(user.name,)))
