# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('schedule_builder', '0001_initial'),
    ]

    operations = [
        migrations.CreateModel(
            name='ClassSection',
            fields=[
                ('id', models.AutoField(primary_key=True, serialize=False, verbose_name='ID', auto_created=True)),
                ('section_number', models.CharField(max_length=10)),
            ],
        ),
        migrations.CreateModel(
            name='Degree',
            fields=[
                ('id', models.AutoField(primary_key=True, serialize=False, verbose_name='ID', auto_created=True)),
                ('name', models.CharField(unique=True, max_length=50)),
            ],
        ),
        migrations.CreateModel(
            name='MyClass',
            fields=[
                ('id', models.AutoField(primary_key=True, serialize=False, verbose_name='ID', auto_created=True)),
                ('class_code', models.CharField(unique=True, max_length=10)),
                ('class_description', models.CharField(max_length=500)),
                ('degree', models.ManyToManyField(to='schedule_builder.Degree')),
            ],
        ),
        migrations.CreateModel(
            name='Schedule',
            fields=[
                ('id', models.AutoField(primary_key=True, serialize=False, verbose_name='ID', auto_created=True)),
                ('creation_date', models.DateTimeField(verbose_name='date created', auto_now_add=True)),
                ('modified_date', models.DateTimeField(verbose_name='date modified', auto_now=True)),
                ('year', models.CharField(max_length=10)),
                ('term', models.CharField(max_length=10)),
            ],
        ),
        migrations.CreateModel(
            name='ScheduleImage',
            fields=[
                ('id', models.AutoField(primary_key=True, serialize=False, verbose_name='ID', auto_created=True)),
                ('creation_date', models.DateTimeField(verbose_name='date created', auto_now_add=True)),
                ('schedule', models.ForeignKey(to='schedule_builder.Schedule')),
            ],
        ),
        migrations.CreateModel(
            name='TimeSlot',
            fields=[
                ('id', models.AutoField(primary_key=True, serialize=False, verbose_name='ID', auto_created=True)),
                ('time', models.TimeField()),
                ('day', models.CharField(max_length=20)),
                ('duration', models.DurationField()),
                ('class_section', models.ManyToManyField(to='schedule_builder.ClassSection')),
            ],
        ),
        migrations.CreateModel(
            name='User',
            fields=[
                ('id', models.AutoField(primary_key=True, serialize=False, verbose_name='ID', auto_created=True)),
                ('name', models.CharField(unique=True, max_length=100)),
                ('degree', models.ForeignKey(to='schedule_builder.Degree')),
            ],
        ),
        migrations.AddField(
            model_name='schedule',
            name='user',
            field=models.ForeignKey(to='schedule_builder.User'),
        ),
        migrations.AddField(
            model_name='classsection',
            name='myclass',
            field=models.ForeignKey(to='schedule_builder.MyClass'),
        ),
        migrations.AddField(
            model_name='classsection',
            name='schedule',
            field=models.ManyToManyField(to='schedule_builder.Schedule'),
        ),
    ]
