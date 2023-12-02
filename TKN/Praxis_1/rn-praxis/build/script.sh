#!/bin/bash
#
{
    echo -ne "GET / HTTP/1.1\r\na: b\r\n"
    sleep 5  # Wait for 1 second
    echo -ne "\r\n"
    # Add more echoes as needed
} | nc 127.0.0.1 1234
