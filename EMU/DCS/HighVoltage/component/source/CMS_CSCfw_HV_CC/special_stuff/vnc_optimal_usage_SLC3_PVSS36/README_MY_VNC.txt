


insertioon of the 03/04/2007
this works with RealVNC 

how to use: 
1) put top of this dir to /home/fast/.vnc
2) put home_fast/* to /home/fast
3) put home_fast_bin to /home/fast/bin

4) DCS 
  (to start the VNC server (not the client!!!!))

  a)  the my_xinitrc is called in xstartup
  b) the .Xclients_my is acelld in  my_xinitrc
  c) the emudcs_start is called in .Xclients_my

5)  vncviewer emudcs:1
    (on vnc client machine)


--------------------------------------------

  08/03/2006


see also some details in the /disk1/doc/doc/mysources/vnc/vnc_optimal_usage_RH73

how to 
 start new X desktop and start my X stuff in it:
----------------------
use 
vncpasswd
to set password for clients: I set to $fast$
---------------------

Attention : this is for Scientific Linux CERN version

THE CONTENTS OF THIS DIRECTORY SHOULD BE USED IN THE  /home/username/.vnc
EXCEPT FOR .Xclients_my (IT SHOULD BE USED IN THE  /home/username)

1) when the tightvnc is installed (and vncserver is started in $HOME account) we have the $HOME/.vnc directory
2) cp xstartup xstartup_orig
3) emacs xstartup &

3') cp /etc/X11/xinit/xinitrc my_xinitrc

4) emacs my_xinitrc &

5) cp /etc/X11/xinit/Xclients $HOME/.Xclients_my

6) emacs $HOME/.Xclients_my &

7) use my_vnc_bat to start all (it is killing the vnc server before)

8) use kill_vnc_bat to kill vnc server

9) adjust -geomentry option in my_vnc_bat

 
--------


