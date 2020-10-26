#!/bin/bash
# Easiest way to mount drives from iMac and newton is using SSH filesystem mounts
# I could probably add this to /etc/fstab, but I think it is better to keep them
# here so that they don't hang reboots.
# Note that this should be run as thompsong, not root or sudo
#sshfs thompsong@131.247.212.145:/Users/gt shares/iMac
#sshfs thompsong@newton.rc.usf.edu:/home/t/thompsong shares/newton/home
#sshfs thompsong@newton.rc.usf.edu:/raid/data shares/newton/data
#sshfs thompsong@newton.rc.usf.edu:/raid/shareddrive shares/newton/shareddrive
#sshfs thompsong@131.247.188.131:thompsong shares/prettygoodstorage
#sshfs thompsong@sc.rc.usf.edu:/shares/gly4930.018f19.gt shares/sc
#sshfs thompsong@circe.rc.usf.edu:thompsong shares/circe

# This is an alternative I figured out on 2019/07/11
sudo sshfs -o allow_other,IdentityFile=~/.ssh/id_rsa thompsong@newton.rc.usf.edu:/raid/data /media/data
sudo sshfs -o allow_other,IdentityFile=~/.ssh/id_rsa thompsong@newton.rc.usf.edu:/raid/home/t/thompsong /media/thompsong
sudo sshfs -o allow_other,IdentityFile=~/.ssh/id_rsa thompsong@newton.rc.usf.edu:/raid/shareddrive /media/shareddrive
sudo sshfs -o allow_other,IdentityFile=~/.ssh/id_rsa thompsong@131.247.212.145:/Users/gt/Desktop /media/imacdesktop
sudo sshfs -o allow_other,IdentityFile=~/.ssh/id_rsa thompsong@131.247.212.145:/Users/gt/Documents /media/imacdocuments
#sudo sshfs -o allow_other,IdentityFile=~/.ssh/id_rsa thompsong@sc.rc.usf.edu:/shares/gly4930.018f19.gt /media/sc
#sudo mount -t exfat /dev/sdf2 /media/RocketDrive/


