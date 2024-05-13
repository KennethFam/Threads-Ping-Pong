# Threads-Ping-Pong
This program is a C Linux program where two threads play ping-pong with each other using pthread mutex conditional variables. 

The ping-pong sequence will go like this:
"thread 1: ping thread 2\n"
"thread 2: pong! thread 1 ping received\n"
"thread 2: ping thread 1\n"
"thread 1: pong! thread 2 ping received\n"

This ping-pong sequence will go on forever until a SIGINT is sent to the process.
