from django.conf.urls import url

from . import views, models
from django.contrib import admin

admin.autodiscover()

urlpatterns = [
    url(r'^$', views.index, name='index'),
    url(r'^class/$', views.course_info),
    url(r'^section/$', views.course_sections),
    url(r'^save/$', views.save_schedule),
    url(r'^(?P<username>' + models.User.name_regex + r')/$', views.user, name='user'),
    url(r'^(?P<username>' + models.User.name_regex + r')/(?P<schedule_id>[0-9]+)/$', views.schedule, name='schedule'),
    url(r'^image/$', views.image, name='image'),
    url(r'^test-image/$', views.image, name='image'),
]
