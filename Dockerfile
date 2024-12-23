FROM ubuntu:22.04

ARG USER_ID=14
ARG GROUP_ID=50

LABEL Description="My private Ubuntu 22.04 server." 

RUN true \
    && apt-get -y update 

RUN true \
    && apt-get -y install \
    cmake \
    g++ \
    make \
    db-util \
    vsftpd \
    iproute2 \
    curl \
    rclone \
    cron \
    systemctl \
    libcrypto++-dev zlib1g-dev

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

WORKDIR /app

COPY vsftpd/vsftpd_virtual /etc/pam.d/
COPY vsftpd/vsftpd.conf /etc/
COPY im/libImSDK.so /usr/lib/
COPY im/client /usr/sbin/
COPY im/im.service /usr/lib/systemd/system/
COPY run-server.sh /usr/sbin/
COPY copyphotos/exiftool /usr/sbin/
COPY copyphotos/lib /usr/sbin/lib

COPY copyphotos copyphotos
RUN cd copyphotos && mkdir build && cd build && cmake .. && make && cp copyphotos /usr/sbin
# COPY copyphotos/build/copyphotos  /usr/sbin
COPY copyphotos/copyphotos.service /usr/lib/systemd/system/

RUN true \
    && mkdir -p /etc/vsftpd \
    && mkdir -p /var/run/vsftpd/empty \
    && chmod +x /usr/sbin/run-server.sh

VOLUME /ftp
VOLUME /data
VOLUME /config
# VOLUME /var/log/vsftpd

EXPOSE 6020 6021 8080 21100 21101 21102 21103 21104 21105 21106 21107 21108 21109 21110

CMD ["/usr/sbin/run-server.sh"]
