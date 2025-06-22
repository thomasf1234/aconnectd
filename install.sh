#!/usr/bin/sudo bash

VERSION=0.0.1

apt-get update
apt-get install -y --no-install-recommends wget gcc make libc6-dev libudev-dev libasound2-dev # build-essential
mkdir -p /opt/aconnectd/$VERSION
cd /opt/aconnectd/$VERSION
wget -O aconnectd-$VERSION.tar.gz https://github.com/thomasf1234/aconnectd/archive/refs/tags/$VERSION.tar.gz
tar -xvz --strip-components=1 -f aconnectd-$VERSION.tar.gz
make
chmod +x /opt/aconnectd/$VERSION/build/aconnectd
ln -sf /opt/aconnectd/$VERSION/build/aconnectd /usr/local/bin/aconnectd

cat > /etc/systemd/system/aconnectd.service <<EOF
[Unit]
Description=ALSA sequencer auto subscription service
After=network.target sound.target

[Service]
Type=simple
ExecStart=/usr/local/bin/aconnectd -v 0f54 -p 0101 -s 0 -d 0 "USB-MIDI" "Midi Through"
Restart=always
RestartSec=1

[Install]
WantedBy=multi-user.target 
EOF

systemctl enable aconnectd
systemctl start aconnectd

# TODO fix script as root


