Name: 
Yubo Tian -yt65
Pengyi (Nick) Pan -pp83

We implemented split horizon with poison reverse, as well as triggered updates.both our forwarding and routing worked.
All user commands (ifconfig, routes, down, up, send) generate expected output, and trigger corresponding changes/requests/updates.



Network topology file we used to test our program(input file for six nodes):

localhost:17001
localhost:17002 191.10.10.4 196.10.10.1
localhost:17006 191.10.10.3 196.10.10.1
localhost:17005 191.10.10.2 195.10.10.1
localhost:17004 191.10.10.1 194.10.10.3

localhost:17002
localhost:17001 192.10.10.1 191.10.10.4
localhost:17006 192.10.10.2 196.10.10.2
localhost:17003 192.10.10.3 193.10.10.1

localhost:17003
localhost:17002 193.10.10.1 192.10.10.3
localhost:17006 193.10.10.2 196.10.10.3
localhost:17005 193.10.10.3 195.10.10.3
localhost:17004 193.10.10.4 194.10.10.1

localhost:17004
localhost:17003 194.10.10.1 193.10.10.4
localhost:17005 194.10.10.2 195.10.10.2
localhost:17001 194.10.10.3 191.10.10.1

localhost:17005
localhost:17001 195.10.10.1 191.10.10.2
localhost:17004 195.10.10.2 194.10.10.2
localhost:17003 195.10.10.3 193.10.10.3

localhost:17006
localhost:17001 196.10.10.1 191.10.10.3
localhost:17002 196.10.10.2 192.10.10.2
localhost:17003 196.10.10.3 193.10.10.2
