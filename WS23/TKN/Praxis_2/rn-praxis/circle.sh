# Define the terminal command
TERMINAL="foot -e"

# Start Node 1
$TERMINAL /bin/bash -c 'PRED_ID=8000 PRED_IP=127.0.0.1 PRED_PORT=2005 SUCC_ID=2000 SUCC_IP=127.0.0.1 SUCC_PORT=2002 ./build/webserver 127.0.0.1 2001 0; exec bash' &

# Start Node 2
$TERMINAL /bin/bash -c 'PRED_ID=0 PRED_IP=127.0.0.1 PRED_PORT=2001 SUCC_ID=4000 SUCC_IP=127.0.0.1 SUCC_PORT=2003 ./build/webserver 127.0.0.1 2002 2000; exec bash' &

# Start Node 3
$TERMINAL /bin/bash -c 'PRED_ID=2000 PRED_IP=127.0.0.1 PRED_PORT=2002 SUCC_ID=6000 SUCC_IP=127.0.0.1 SUCC_PORT=2004 ./build/webserver 127.0.0.1 2003 4000; exec bash' &

# Start Node 4
$TERMINAL /bin/bash -c 'PRED_ID=4000 PRED_IP=127.0.0.1 PRED_PORT=2003 SUCC_ID=8000 SUCC_IP=127.0.0.1 SUCC_PORT=2005 ./build/webserver 127.0.0.1 2004 6000; exec bash' &

# Start Node 5
$TERMINAL /bin/bash -c 'PRED_ID=6000 PRED_IP=127.0.0.1 PRED_PORT=2004 SUCC_ID=0 SUCC_IP=127.0.0.1 SUCC_PORT=2001 ./build/webserver 127.0.0.1 2005 8000; exec bash' &

