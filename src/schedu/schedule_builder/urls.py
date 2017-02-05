from django.conf.urls import url

from . import views
from django.contrib import admin

admin.autodiscover()

urlpatterns = [
    url(r'^$', views.index, name='index'),
    url(r'^image/$', views.image, name='image'),
    url(r'^test-image/$', views.image, name='image'),
]
