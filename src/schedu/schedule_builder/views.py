from django.shortcuts import render
from django.http import HttpResponse

from .models import Image 
# Create your views here.

def index(request):
    return render(request, 'schedule_builder/index.html')


def image(request):
	return render(request, 'schedule_builder/image.html')
