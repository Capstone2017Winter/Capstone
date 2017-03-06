from django.test import TestCase
from .models import *
from django.urls import reverse

class UserMethodTests(TestCase):

	def test_is_valid_name(self):
		"""
		is_valid_name() should return true if the name consists
		of only alphanumeric characters, and false otherwise
		"""
		self.assertIs(bool(User.is_valid_name('edcarter')), True)
		self.assertIs(bool(User.is_valid_name('edcarter94')), True)
		self.assertIs(bool(User.is_valid_name('')), False)
		self.assertIs(bool(User.is_valid_name(' ')), False)
		self.assertIs(bool(User.is_valid_name('e carter')), False)
		self.assertIs(bool(User.is_valid_name('e_carter')), False)
		self.assertIs(bool(User.is_valid_name('e.carter')), False)

class CourseInfoViewTests(TestCase):

	def test_course_info_endpoint_exists(self):
		"""
		test that we can get a response from the course info endpoint
		"""
		response = self.client.get(reverse('class'), {'className':'MATH 100'})
		self.assertEqual(response.status_code, 200)

	def test_course_info_endpoint_has_objects(self):
		"""
		test that the course info endpoint returns some courses
		"""
		response = self.client.get(reverse('class'), {'className':'MATH 100'})
		self.assertEqual(response.status_code, 200)
		jsons = response.json()
		self.assertTrue(jsons['num_results'] > 0)
