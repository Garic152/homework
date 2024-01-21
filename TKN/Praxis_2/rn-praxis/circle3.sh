#!/bin/bash

# Start the first node in a new foot instance
foot -e /bin/bash -c 'PRED_ID=30000 PRED_IP=127.0.0.1 PRED_PORT=2002 SUCC_ID=30000 SUCC_IP=127.0.0.1 SUCC_PORT=2002 ./build/webserver 127.0.0.1 2001 16384; exec bash' &

# Start the second node in a new foot instance
foot -e /bin/bash -c 'PRED_ID=16384 PRED_IP=127.0.0.1 PRED_PORT=2001 SUCC_ID=16384 SUCC_IP=127.0.0.1 SUCC_PORT=2001 ./build/webserver 127.0.0.1 2002 30000; exec bash' &
