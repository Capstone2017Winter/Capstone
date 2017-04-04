# -*- coding: utf-8 -*-
# Generated by Django 1.10.5 on 2017-03-24 17:58
from __future__ import unicode_literals

from django.db import migrations, models
import django.db.models.deletion


class Migration(migrations.Migration):

    initial = True

    dependencies = [
    ]

    operations = [
        migrations.CreateModel(
            name='ClassSection',
            fields=[
                ('id', models.AutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('section_number', models.CharField(max_length=10)),
                ('section_type', models.CharField(max_length=20)),
            ],
        ),
        migrations.CreateModel(
            name='Degree',
            fields=[
                ('name', models.CharField(max_length=50, primary_key=True, serialize=False)),
            ],
        ),
        migrations.CreateModel(
            name='Image',
            fields=[
                ('id', models.AutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('image', models.ImageField(upload_to='path/', verbose_name='img')),
            ],
        ),
        migrations.CreateModel(
            name='MyClass',
            fields=[
                ('class_code', models.CharField(max_length=10, primary_key=True, serialize=False)),
                ('short_description', models.CharField(default='', max_length=100)),
                ('long_description', models.CharField(default='', max_length=500)),
                ('degree', models.ManyToManyField(to='schedule_builder.Degree')),
            ],
        ),
        migrations.CreateModel(
            name='Schedule',
            fields=[
                ('id', models.AutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('creation_date', models.DateTimeField(auto_now_add=True, verbose_name='date created')),
                ('modified_date', models.DateTimeField(auto_now=True, verbose_name='date modified')),
                ('year', models.CharField(max_length=10)),
                ('term', models.CharField(max_length=10)),
            ],
        ),
        migrations.CreateModel(
            name='ScheduleImage',
            fields=[
                ('id', models.AutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('creation_date', models.DateTimeField(auto_now_add=True, verbose_name='date created')),
                ('schedule', models.ForeignKey(on_delete=django.db.models.deletion.CASCADE, to='schedule_builder.Schedule')),
            ],
        ),
        migrations.CreateModel(
            name='TimeSlot',
            fields=[
                ('id', models.AutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('start_time', models.TimeField()),
                ('end_time', models.TimeField()),
                ('day', models.CharField(max_length=20)),
                ('class_section', models.ManyToManyField(to='schedule_builder.ClassSection')),
            ],
        ),
        migrations.CreateModel(
            name='User',
            fields=[
                ('name', models.CharField(max_length=100, primary_key=True, serialize=False)),
                ('degree', models.ForeignKey(null=True, on_delete=django.db.models.deletion.CASCADE, to='schedule_builder.Degree')),
            ],
        ),
        migrations.AddField(
            model_name='schedule',
            name='user',
            field=models.ForeignKey(on_delete=django.db.models.deletion.CASCADE, to='schedule_builder.User'),
        ),
        migrations.AddField(
            model_name='classsection',
            name='myclass',
            field=models.ForeignKey(on_delete=django.db.models.deletion.CASCADE, to='schedule_builder.MyClass'),
        ),
        migrations.AddField(
            model_name='classsection',
            name='schedule',
            field=models.ManyToManyField(to='schedule_builder.Schedule'),
        ),
    ]
