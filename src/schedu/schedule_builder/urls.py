from django.conf.urls import url

from . import views
from django.contrib import admin

admin.autodiscover()

urlpatterns = [
    url(r'^$', views.index, name='index'),
    url(r'^(?P<username>[a-zA-Z0-9]+)/$', views.user, name='user'),
    url(r'^(?P<username>[a-zA-Z0-9]+)/(?P<schedule_id>[0-9]+)/$', views.schedule, name='schedule'),
    url(r'^image/$', views.image, name='image'),
    url(r'^test-image/$', views.image, name='image'),
]
