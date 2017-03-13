from django.test import TestCase
from .models import *
from django.urls import reverse
import json

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

	def test_course_info_endpoint_objects_have_name(self):
		"""
		test that the course info objects returned have a name
		"""
		response = self.client.get(reverse('class'), {'className':'MATH 100'})
		self.assertEqual(response.status_code, 200)
		jsons = response.json()
		course_info = jsons['objects'][0]
		self.assertTrue('asString' in course_info)

	def test_course_info_endpoint_objects_have_description(self):
		"""
		test that the course info objects returned have a description
		"""
		response = self.client.get(reverse('class'), {'className':'MATH 100'})
		self.assertEqual(response.status_code, 200)
		jsons = response.json()
		course_info = jsons['objects'][0]
		self.assertTrue('courseDescription' in course_info)

	def test_course_info_endpoint_objects_have_term(self):
		"""
		test that the course info objects returned have a term
		"""
		response = self.client.get(reverse('class'), {'className':'MATH 100'})
		self.assertEqual(response.status_code, 200)
		jsons = response.json()
		course_info = jsons['objects'][0]
		self.assertTrue('term' in course_info)

class CourseSectionViewTests(TestCase):

	def test_course_section_endpoint_exists(self):
		"""
		test that we can get a response from the course section endpoint
		"""
		response = self.client.get(reverse('section'), {'courseId':'006768', 'termName':'FALL 2016'})
		self.assertEqual(response.status_code, 200)

	def test_course_section_endpoint_has_objects(self):
		"""
		test that the course info endpoint returns some sections
		"""
		response = self.client.get(reverse('section'), {'courseId':'006768', 'termName':'FALL 2016'})
		self.assertEqual(response.status_code, 200)	
		jsons = response.json()
		self.assertTrue(jsons['num_results'] > 0)

	def test_course_section_endpoint_has_sections(self):
		"""
		test that the returned objects contains sections
		"""
		response = self.client.get(reverse('section'), {'courseId':'006768', 'termName':'FALL 2016'})
		self.assertEqual(response.status_code, 200)
		jsons = response.json()
		self.assertTrue('sections' in jsons['objects'][0])

	def test_course_section_endpoint_section_has_component(self):
		"""
		test that the returned sections have a component.
		the component denotes what section type it is such as
		lecture, seminar, or lab.
		"""
		response = self.client.get(reverse('section'), {'courseId':'006768', 'termName':'FALL 2016'})
		self.assertEqual(response.status_code, 200)
		jsons = response.json()
		section = jsons['objects'][0]['sections'][0]
		self.assertTrue('component' in section)

	def test_course_section_endpoint_section_has_times(self):
		"""
		test that the returned sections have start time and end time
		"""
		response = self.client.get(reverse('section'), {'courseId':'006768', 'termName':'FALL 2016'})
		self.assertEqual(response.status_code, 200)
		jsons = response.json()
		section = jsons['objects'][0]['sections'][0]
		self.assertTrue('startTime' in section)
		self.assertTrue('endTime' in section)

	def test_course_section_endpoint_section_has_section_number(self):
		"""
		test that the returned sections have a section number such as 'b1'
		"""
		response = self.client.get(reverse('section'), {'courseId':'006768', 'termName':'FALL 2016'})
		self.assertEqual(response.status_code, 200)
		jsons = response.json()
		section = jsons['objects'][0]['sections'][0]
		self.assertTrue('section' in section)

	def test_course_section_endpoint_section_has_day(self):
		"""
		test that the returned sections have a day such as 'MWF'
		"""
		response = self.client.get(reverse('section'), {'courseId':'006768', 'termName':'FALL 2016'})
		self.assertEqual(response.status_code, 200)
		jsons = response.json()
		section = jsons['objects'][0]['sections'][0]
		self.assertTrue('day' in section)

class SaveScheduleTests(TestCase):
	
	def test_save_schedule_endpoint_exists(self):
		"""
		test that we can get a response from the save schedule endpoint
		"""
		response = self.client.get(reverse('save'))
		self.assertEqual(response.status_code, 200)

	def test_save_schedule_enpoint_can_save_lecture(self):
		"""
		test that the save schedule can save a course with only a lecture section
		"""
		scheduleId = '1'

		courseName = 'test_course'
		courseShortDescription = 'Test Description'
		courseLongDescription = 'This is a test class description'

		lectureCode = 'B1'
		lectureStart = '12:00 PM'
		lectureEnd = '12:50 PM'
		lectureDays = 'MWF'

		course = {'name':courseName, 'short':courseShortDescription, 'long':courseLongDescription}
		lecture = {'code':lectureCode, 'start':lectureStart, 'end':lectureEnd, 'days':lectureDays }

		data = {'scheduleId':scheduleId, 'classArray[]':[json.dumps({'course':course, 'lecture':lecture})]}
		response = self.client.post(reverse('save'), data)
		schedule = Schedule.objects.get(pk=scheduleId)
		
