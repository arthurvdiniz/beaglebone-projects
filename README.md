# Beaglebone Projects
Some small projects on the beaglebone board for Real Time Systems lecture.

## How to connect

- SSH
```shell
ssh debian@192.168.7.2
# password = tempwd
```
- Adding the default gateway (on Board)
```shell
sudo ifconfig usb0 192.168.7.2
sudo route add default gw 192.168.7.1
sudo su
echo "nameserver 8.8.8.8" >> /etc/resolv.conf
exit
```
- Routing Internet to the beagle from computer (on your pc)
```shell
sudo su
ifconfig enx78a504eddfe6 192.168.7.1
iptables --table nat --append POSTROUTING --out-interface wlps2s0 -j MASQUERADE
iptables --append FORWARD --in-interface enx78a504eddfe6 -j ACCEPT
echo 1 > /proc/sys/net/ipv4/ip_forward
exit
```
You'll need to change **enx78a504eddfe6** for the interface that you computer is connect to the board. And change the interface **wlps2s0** to the interface that your computer is connect with the internet. To find out run this command:
```shell
ifconfig
```
To test if everything went out okay, run on the board:
```shell
ping 8.8.8.8
# or
ping www.google.com
```
If you see some pings on terminal then you're ready to go!