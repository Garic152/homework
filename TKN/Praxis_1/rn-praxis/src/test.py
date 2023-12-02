import socket
import time


def test_packets():
    port = 1234  # Change to your server's listening port

    with socket.create_connection(('localhost', port), timeout=2) as conn:
        conn.settimeout(30)
        conn.send('GET / HTTP/1.1\r\n\r\n'.encode())
        time.sleep(30)
        conn.send('GET / HTTP/1.1\r\na: b\r\n'.encode())
        time.sleep(30)
        conn.send('\r\n'.encode())
        time.sleep(30)  # Gracefully handle multi-packet replies
        replies = conn.recv(1024).split(b'\r\n\r\n')
        assert replies[0] and replies[1] and not replies[2]


# Run the test
test_packets()
