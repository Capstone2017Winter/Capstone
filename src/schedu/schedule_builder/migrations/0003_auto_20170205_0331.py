# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('schedule_builder', '0002_auto_20170205_0105'),
    ]

    operations = [
        migrations.RemoveField(
            model_name='degree',
            name='id',
        ),
        migrations.RemoveField(
            model_name='myclass',
            name='id',
        ),
        migrations.RemoveField(
            model_name='user',
            name='id',
        ),
        migrations.AlterField(
            model_name='degree',
            name='name',
            field=models.CharField(serialize=False, primary_key=True, max_length=50),
        ),
        migrations.AlterField(
            model_name='myclass',
            name='class_code',
            field=models.CharField(serialize=False, primary_key=True, max_length=10),
        ),
        migrations.AlterField(
            model_name='user',
            name='name',
            field=models.CharField(serialize=False, primary_key=True, max_length=100),
        ),
    ]
