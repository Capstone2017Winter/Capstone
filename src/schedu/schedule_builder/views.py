from django.shortcuts import render
from django.http import HttpResponse

from .models import Image 
# Create your views here.

def index(request):
    return render(request, 'schedule_builder/index.html')


def image(request):
    return render(request, 'schedule_builder/image.html')

def user(request, username):
    return HttpResponse('You are looking at the page for user: {}'.format(username))

def schedule(request, username,  schedule_id):
    return HttpResponse('You are looking at the schedule for user: {} with id: {}'.format(username, schedule_id))
