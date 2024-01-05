FROM debian

RUN apt-get update

WORKDIR /usr/src
COPY ./webserv /usr/src/webserv
COPY ./configrations /usr/src/configrations

# RUN apt-get -y install python3
# RUN pip install --update pip
# RUN pip install -r requirements.txt
# COPY ./requirements.txt /usr/src/requirements.txt

RUN	echo "building for webserv"

CMD	["./webserv configrations/default.conf"]