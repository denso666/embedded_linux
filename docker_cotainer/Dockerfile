FROM debian:buster

RUN apt-get update
RUN apt-get install -y --no-install-recommends build-essential
RUN apt-get install -y gcc-arm-linux-gnueabihf
RUN apt-get install -y openssh-client openssh-server

ENV QEMU_LD_PREFIX=/usr/arm-linux-gnueabihf

WORKDIR /cc_project
COPY . .

# CMD ["arm-linux-gnueabihf-gcc", "main.c", "-o", "/home/cross_main"]
# CMD ["scp", "/home/cross_main", "debian@192.168.7.2:/home/debian/"]
