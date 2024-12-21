#!/bin/bash

# If no env var for FTP_USER has been specified, use 'admin':
if [ "$FTP_USER" = "**String**" ]; then
    export FTP_USER='admin'
fi

# If no env var has been specified, generate a random password for FTP_USER:
if [ "$FTP_PASS" = "**Random**" ]; then
    export FTP_PASS=`cat /dev/urandom | tr -dc A-Z-a-z-0-9 | head -c${1:-16}`
fi

# Do not log to STDOUT by default:
if [ "$LOG_STDOUT" = "**Boolean**" ]; then
    export LOG_STDOUT=''
else
    export LOG_STDOUT='Yes.'
fi

# Create home dir and update vsftpd user db:
# mkdir -p "/srv/ftp/${FTP_USER}"
chown -R ftp:ftp /ftp

echo -e "${FTP_USER}\n${FTP_PASS}" > /etc/vsftpd/virtual_users.txt
/usr/bin/db_load -T -t hash -f /etc/vsftpd/virtual_users.txt /etc/vsftpd/virtual_users.db
chmod 400 /etc/vsftpd/virtual_users.*

# Set passive mode parameters:
if [ "$PASV_ADDRESS" = "**IPv4**" ]; then
    #export PASV_ADDRESS=$(/sbin/ip route|awk '/default/ { print $3 }')
    default_src=$(/sbin/ip route|grep default|grep -oP '(?<=src )\d+\.\d+\.\d+\.\d+')
    default_via=$(/sbin/ip route|grep default|grep -oP '(?<=via )\d+\.\d+\.\d+\.\d+')
    export PASV_ADDRESS=${default_src:-$default_via}
fi

if [ ! -f /etc/vsftpd/.config_lock ]; then
    echo "pasv_address=${PASV_ADDRESS}" >> /etc/vsftpd.conf
    echo "pasv_max_port=${PASV_MAX_PORT}" >> /etc/vsftpd.conf
    echo "pasv_min_port=${PASV_MIN_PORT}" >> /etc/vsftpd.conf
    echo "pasv_addr_resolve=${PASV_ADDR_RESOLVE}" >> /etc/vsftpd.conf
    echo "pasv_enable=${PASV_ENABLE}" >> /etc/vsftpd.conf
    echo "file_open_mode=${FILE_OPEN_MODE}" >> /etc/vsftpd.conf
    echo "local_umask=${LOCAL_UMASK}" >> /etc/vsftpd.conf
    echo "xferlog_std_format=${XFERLOG_STD_FORMAT}" >> /etc/vsftpd.conf
    echo "pasv_promiscuous=${PASV_PROMISCUOUS}" >> /etc/vsftpd.conf
    echo "port_promiscuous=${PORT_PROMISCUOUS}" >> /etc/vsftpd.conf
    touch /etc/vsftpd/.config_lock
fi

# Get log file path
export LOG_FILE=`grep vsftpd_log_file /etc/vsftpd.conf|cut -d= -f2`

# stdout server info:
if [ ! $LOG_STDOUT ]; then
cat << EOB
        SERVER SETTINGS
        ---------------
        · FTP User: $FTP_USER
        · FTP Password: $FTP_PASS
        · Passive address: ${PASV_ADDRESS}
        · Log file: $LOG_FILE
        · Redirect vsftpd log to STDOUT: No.
EOB
else
    /usr/bin/ln -sf /dev/stdout $LOG_FILE
fi

# Run vsftpd:
systemctl start cron.service
systemctl start im.service
systemctl start vsftpd.service
crontab /config/crontab_config.txt
&>/dev/null /bin/bash