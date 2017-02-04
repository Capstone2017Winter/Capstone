from __future__ import unicode_literals

from django.db import models

# Create your models here.
class Image(models.Model):
	image = models.ImageField('img', upload_to='path/')

	def __str__(self):
		return image.name
