The AWS instance contains this Git repo found at /home/ubuntu/Capstone

How to run the gunicorn server:
  
  - Within the /home/ubuntu/Capstone/src/schedu directory is "start_server.sh"
  - To run the server `sudo ./start_server.sh`
  - To stop the server `sudo ./start_server.sh`
  - Server runs on localhost:8000
  - Gunicorn server set up via this reference http://agiliq.com/blog/2014/08/deploying-a-django-app-on-amazon-ec2-instance/
  - Nginx set up via this reference http://agiliq.com/blog/2013/08/minimal-nginx-and-gunicorn-configuration-for-djang/
    - Nginx listens on 127.0.0.1:8001 for serving static files such as images and css
