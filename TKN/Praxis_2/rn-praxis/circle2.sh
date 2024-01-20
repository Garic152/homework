#!/bin/bash

# Start the first node in a new foot instance
foot -e /bin/bash -c 'PRED_ID=8192 PRED_IP=127.0.0.1 PRED_PORT=4712 SUCC_ID=4096 SUCC_IP=127.0.0.1 SUCC_PORT=4711 ./build/webserver 127.0.0.1 4710 0; exec bash' &

# Start the second node in a new foot instance
foot -e /bin/bash -c 'PRED_ID=0 PRED_IP=127.0.0.1 PRED_PORT=4710 SUCC_ID=8192 SUCC_IP=127.0.0.1 SUCC_PORT=4712 ./build/webserver 127.0.0.1 4711 4096; exec bash' &

# Start the third node in a new foot instance
foot -e /bin/bash -c 'PRED_ID=4096 PRED_IP=127.0.0.1 PRED_PORT=4711 SUCC_ID=0 SUCC_IP=127.0.0.1 SUCC_PORT=4710 ./build/webserver 127.0.0.1 4712 8192; exec bash' &

