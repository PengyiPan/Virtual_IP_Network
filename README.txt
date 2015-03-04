————————————————————————————————————————————————————————————————————————————————————————

Name: 
Yubo Tian -yt65
Pengyi (Nick) Pan -pp83

All required functions work as expected.
We implemented split horizon with poison reverse, as well as triggered updates. both our forwarding and routing are fully functional.
All user commands (ifconfig, routes, down, up, send) generate expected output, and trigger corresponding changes/requests/updates.

————————————————————————————————————————————————————————————————————————————————————————

How to run:

 make all
	or
 g++ -o main main.cc ipsum.o -lpthread

It is RECOMMANDED that you turn on the display forwarding table functionaly to test the program.

turn on forwarding table display:	(when running) dft on

————————————————————————————————————————————————————————————————————————————————————————

EXTRA Credit:

1. implemented user command ‘mtu’. e.g., ‘mtu 1 700’ set mtu for interface 1 to 700 byte.
2. created a demo file (test_frag.cc), tried but not fully functional.
3. since demo is not fully functional, we haven't put it into main.cc.

————————————————————————————————————————————————————————————————————————————————————————

User Interface(USER MANUAL):

Possible commands supported by our program:

disinfo - ‘disinfo on’/‘disinfo off’ turn on/off run time info
	info includes: 
		notification when forwarding a msg packet
		notification when cannot send/reach
		notification when received a RIP packet
		
dft - ‘dft on’/‘dft off’ turn on/off display_forwarding_table every 2 sec on the fly 

help - view user manual

ifconfig - prints info about each interface
routes - print info about routes to each known destination
down - ‘down 1’ down interface 1
up - ‘up 1’ up interface 1
send - ‘send 10.10.168.73 hello’ send message ‘hello’ to IP add 10.10.168.73

mtu - ‘mtu 1 700’ set mtu for link 1 to 700 byte"


kill - kill the current process. (= ctr+C) 

————————————————————————————————————————————————————————————————————————————————————————

Network topology file we used to test our program(besides the three-node example provided, input file for six nodes):


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

The test files and a picture showing the node are in the "test_case" folder.
