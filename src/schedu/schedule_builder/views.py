from django.shortcuts import render, get_object_or_404
from django.http import HttpResponse

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
    return HttpResponse('You are at the home page!')
