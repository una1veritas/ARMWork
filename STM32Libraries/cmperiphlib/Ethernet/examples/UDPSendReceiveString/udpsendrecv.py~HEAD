# udpsendrecv.py
# 2012/02 created by audin.
# UDP server demo 
#  Just print out received data.
# Using python 2.6 

import sys,socket,time

pc_ip       = '192.168.0.4'		# My desktop PC's IP address
dest_ip     = '192.168.0.177'	# Target micon's IP address
port        = 8888
udpServer   = socket.socket( socket.AF_INET, socket.SOCK_DGRAM )
udpServer.bind( ( pc_ip, port ) )
udpServer.setblocking( 0 )	# set none blocking mode
recvmsg     = ''
msg         = 'from pc app'
print 'UDP Server.  Press CTL-C key for exit.'
print 'Waiting packets on <%d> port ...' % port
count = 0
while True:
	time.sleep( 0.1 )		# needed several msec sleep reason to reduce cpu time
	count   += 1
	send_msg = '[ %s %4d ]' % ( msg, count )
	udpServer.sendto( send_msg, ( dest_ip, port ) )
	try:
		recvmsg, client_addrs = udpServer.recvfrom( 1024 )
	except:
		pass	
	if len( recvmsg ) > 0:
		print '[%s]: %s' % ( client_addrs[ 0 ], recvmsg )
		recvmsg = ''

