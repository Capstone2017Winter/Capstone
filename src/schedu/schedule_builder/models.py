from __future__ import unicode_literals

from django.db import models
from django.contrib import admin

import re #regex

class Degree(models.Model):
	name = models.CharField(max_length=50, primary_key=True)

	def __str__(self):
		return self.name

class User(models.Model):
	degree = models.ForeignKey(Degree, null=True)
	name_regex = r'[a-zA-Z0-9]+'
	name = models.CharField(max_length=100, primary_key=True)

	def __str__(self):
		return self.name

	def is_valid_name(s):
		return re.match(r'^' + User.name_regex + r'$', s)

class Schedule(models.Model):
	user = models.ForeignKey(User, on_delete=models.CASCADE)
	creation_date = models.DateTimeField('date created', auto_now_add=True)
	modified_date = models.DateTimeField('date modified', auto_now=True)
	year = models.CharField(max_length=10) #eg 2017
	term = models.CharField(max_length=10) #eg FALL

	def __str__(self):
		return 'user:{}, year:{}, term:{}'.format(self.user, self.year, self.term)

class MyClass(models.Model):
	degree = models.ManyToManyField(Degree)
	class_code = models.CharField(max_length=10, primary_key=True)
	class_description = models.CharField(max_length=500)

	def __str__(self):
		return self.class_code

class ClassSection(models.Model):
	schedule = models.ManyToManyField(Schedule)
	myclass = models.ForeignKey(MyClass, on_delete=models.CASCADE)
	section_number = models.CharField(max_length=10) #eg EB2

	def __str__(self):
		return 'class:{}, section:{}'.format(self.myclass, self.section_number)

class TimeSlot(models.Model):
	class_section = models.ManyToManyField(ClassSection)
	time = models.TimeField()
	day = models.CharField(max_length=20)
	duration = models.DurationField()

	def __str__(self):
		return 'time:{}, day:{}, duration:{}'.format(self.time, self.day, self.duration)

class ScheduleImage(models.Model):
	schedule = models.ForeignKey(Schedule, on_delete=models.CASCADE) #warning, if image is stored on file system, it must be manually deleted
	creation_date = models.DateTimeField('date created', auto_now_add=True)

class Image(models.Model):
	image = models.ImageField('img', upload_to='path/')

	def __str__(self):
		return self.image.name
