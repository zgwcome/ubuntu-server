FROM ubuntu:22.04

ARG USER_ID=14
ARG GROUP_ID=50

MAINTAINER Sim Fishing <sim.fishing@gmail.com>
LABEL Description="vsftpd Docker image based on Ubuntu 22.04. Supports passive mode and virtual users. Based on fauria/vsftpd image." \
	License="Apache License 2.0" \
	Usage="docker run -d -p [HOST PORT NUMBER]:21 -v [HOST FTP HOME]:/srv/ftp simfishing/vsftpd" \
	Version="1.0"

RUN true \
    && apt-get -y update 

RUN true \
    && apt-get -y install \
    db-util \
    vsftpd \
    iproute2

ENV FTP_USER **String**
ENV FTP_PASS **Random**
ENV PASV_ADDRESS **IPv4**
ENV PASV_ADDR_RESOLVE NO
ENV PASV_ENABLE YES
ENV PASV_MIN_PORT 21100
ENV PASV_MAX_PORT 21110
ENV XFERLOG_STD_FORMAT NO
ENV LOG_STDOUT **Boolean**
ENV FILE_OPEN_MODE 0666
ENV LOCAL_UMASK 077
ENV PASV_PROMISCUOUS NO
ENV PORT_PROMISCUOUS NO

COPY vsftpd_virtual /etc/pam.d/
COPY vsftpd.conf /etc/
COPY run-vsftpd.sh /usr/sbin/

RUN true \
        && mkdir -p /var/run/vsftpd/empty \
        && chmod +x /usr/sbin/run-vsftpd.sh

VOLUME /srv/ftp
VOLUME /var/log/vsftpd

EXPOSE 4220 4221

CMD ["/usr/sbin/run-vsftpd.sh"]
