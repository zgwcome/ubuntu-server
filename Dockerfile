FROM ubuntu:22.04

LABEL Description="My private Ubuntu 22.04 server." 

RUN true \
    && apt-get -y update \
    && apt-get install -y software-properties-common \
    && add-apt-repository ppa:deadsnakes/ppa \
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
    python3.12 python3-pip python3.12-distutils \
    libcrypto++-dev zlib1g-dev

WORKDIR /app

COPY copyphotos/exiftool /usr/sbin/
COPY copyphotos/lib /usr/sbin/lib

RUN true \
    && chmod +x /usr/sbin/run-server.sh \
    && ln -s /usr/bin/python3.12 /usr/bin/python3 \
    && rm -rf /var/lib/apt/lists/*
VOLUME /ftp
VOLUME /data
VOLUME /config

EXPOSE 80

CMD ["/usr/sbin/run-server.sh"]
