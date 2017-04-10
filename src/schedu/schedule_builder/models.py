"""
This file defines the django 'models' used in the application.
The models are python classes which will be mapped to and from
entries in database tables. The models represent the objects
which are persistent in the web application.
See: https://docs.djangoproject.com/en/1.10/topics/db/models/
"""
from __future__ import unicode_literals

from django.db import models
from django.contrib import admin

import re #regex

class Degree(models.Model):
	"""
	model representing a user's degree. ie: Computer Engineering
	"""
	name = models.CharField(max_length=50, primary_key=True)
	def __str__(self):
		return self.name

class MyClass(models.Model):
	"""
	model representing a class. A class can be associated with a degree
	"""
	degree = models.ManyToManyField(Degree)
	class_code = models.CharField(max_length=10, primary_key=True)
	short_description = models.CharField(max_length=100, default="")
	long_description = models.CharField(max_length=500, default="")

	def __str__(self):
		return self.class_code

class ElectiveList(models.Model):
	"""
	model representing a group of electives for a certain degree track
	"""
	degree = models.ForeignKey(Degree, null=True)
	name = models.CharField(max_length=50, primary_key=True)
	classList = models.ManyToManyField(MyClass)


class User(models.Model):
	"""
	model representing a user. The user is associated with a degree
	"""
	degree = models.ForeignKey(Degree, null=True)
	name_regex = r'[a-zA-Z0-9]+'
	name = models.CharField(max_length=100, primary_key=True)

	def __str__(self):
		return self.name

	def is_valid_name(s):
		"""
		validate that the user name is valid according to our regex
		"""
		return re.match(r'^' + User.name_regex + r'$', s)

class Schedule(models.Model):
	"""
	model representing a schedule. A schedule has a reference to a user
	"""	
	user = models.ForeignKey(User, on_delete=models.CASCADE)
	creation_date = models.DateTimeField('date created', auto_now_add=True)
	modified_date = models.DateTimeField('date modified', auto_now=True)
	year = models.CharField(max_length=10) #eg 2017
	term = models.CharField(max_length=10) #eg FALL

	def __str__(self):
		return 'user:{}, year:{}, term:{}'.format(self.user, self.year, self.term)



class ClassSection(models.Model):
	"""
	model representing a class section, ie: B1. A section
	belongs to a class and can be associated with a schedule
	"""
	schedule = models.ManyToManyField(Schedule)
	myclass = models.ForeignKey(MyClass, on_delete=models.CASCADE)
	section_number = models.CharField(max_length=10) #eg EB2
	section_type = models.CharField(max_length=20) # eg lecture, seminar, lab

	def __str__(self):
		return 'class:{}, section:{}, section_type:{}'.format(self.myclass, self.section_number, self.section_type)

class TimeSlot(models.Model):
	"""
	model representing a time slot. IE MWF 11:00AM-12:00PM.
	it may be associated with a class section
	"""
	TIME_INPUT_FORMATS = ['%I:%M %p',] #eg 4:30 PM
	class_section = models.ManyToManyField(ClassSection)
	start_time = models.TimeField()
	end_time = models.TimeField()
	day = models.CharField(max_length=20)

	def __str__(self):
		return 'start_time:{}, end_time:{}, day:{}'.format(self.start_time, self.end_time, self.day)

class ScheduleImage(models.Model):
	"""
	model representing a image of a created schedule
	"""
	schedule = models.ForeignKey(Schedule, on_delete=models.CASCADE) #warning, if image is stored on file system, it must be manually deleted
	creation_date = models.DateTimeField('date created', auto_now_add=True)

class Image(models.Model):
	image = models.ImageField('img', upload_to='path/')

	def __str__(self):
		return self.image.name
