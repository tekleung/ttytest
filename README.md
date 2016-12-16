# ttytest
For test serial port, loop back and send/receive between client and server

Make:
	
	pull this ttytest/ under android SDK external/ , mmm external/ttytest for make file (cli_tty/ser_tty)

if 2 machine send&receive test , have to run ser_tty on server machine first , then run cli_tty on client machine.

if test loop back , only shorted tx/rx , and run cli_tty .

