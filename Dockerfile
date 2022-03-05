from ubuntu:20.04

RUN apt-get update -y; \
    DEBIAN_FRONTEND="noninteractive" apt-get install -y tzdata; \
    ln -fs /usr/share/America/Chicago /etc/localtime ; \
    apt-get upgrade -y; \
    apt-get install -y \
        g++ \
        make \
        time
