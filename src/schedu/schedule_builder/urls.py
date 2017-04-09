"""
The urls.py file defines what website urls
map to the corresponding views in views.py which
handles requests.
See: https://docs.djangoproject.com/en/1.10/topics/http/views/
"""
from django.conf.urls import url

from . import views, models
from django.contrib import admin

admin.autodiscover()

urlpatterns = [
    url(r'^$', views.index, name='index'),
    url(r'^class/$', views.course_info, name='class'),
    url(r'^section/$', views.course_sections, name='section'),
    url(r'^save/$', views.save_schedule, name='save'),
    url(r'^load/$', views.load_schedule, name = 'load'),
    url(r'^image/$', views.image, name='image'),
    url(r'^completedclasses/$', views.completed_classes, name='completedclasses'),
    url(r'^(?P<username>' + models.User.name_regex + r')/$', views.user, name='user'),
    url(r'^(?P<username>' + models.User.name_regex + r')/(?P<schedule_id>[0-9]+)/$', views.schedule, name='schedule'),
    url(r'^test-image/$', views.image, name='image'),
]
