FROM ubuntu:22.04
ENV DEBIAN_FRONTEND=noninteractive

# 2. 设置期望的时区（可选，但推荐）
ENV TZ=Asia/Shanghai

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
    iproute2 \
    curl \
    rclone \
    cron \
    systemctl \
    python3.12 python3-pip \
    libcrypto++-dev zlib1g-dev

RUN ln -fs /usr/share/zoneinfo/$TZ /etc/localtime && \
    echo $TZ > /etc/timezone && \
    dpkg-reconfigure -f noninteractive tzdata

WORKDIR /app

COPY run-server.sh /usr/sbin/
COPY copyphotos/exiftool /usr/sbin/
COPY copyphotos/lib /usr/sbin/lib

RUN true \
    && chmod +x /usr/sbin/run-server.sh 

VOLUME /ftp
VOLUME /data
VOLUME /config

EXPOSE 80

CMD ["/usr/sbin/run-server.sh"]
