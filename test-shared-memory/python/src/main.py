import os
import sys
import json
import base64
import socket

from typing import Any

from multiprocessing import shared_memory

from enum import IntEnum

import lib.example as example

class MainCommands(IntEnum):
    ENVIRONMENT_READY = 0x00
    REQUEST_BUFFER = 0x01
    BUFFER_READY = 0x02

NAME : str = "SharedMemoryExample"
SIZE : int = 1024
ADDRESS : str = "127.0.0.1"
PORT : int = 8080

def main() -> None:
    # Decode arguments.
    raw : bytes = base64.b64decode(sys.argv[1])
    args : list[Any] = json.loads(raw.decode("utf-8"))
    
    # Open shared memory.
    shm : shared_memory.SharedMemory = shared_memory.SharedMemory(name = args[0], create = False, size = args[1])

    if os.name != "nt":
        try:
            from multiprocessing import resource_tracker
            resource_tracker.unregister(shm._name, 'shared_memory')
        except Exception:
            pass

    # Connect to the TCP server.
    tcp : socket.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    tcp.connect((args[2], args[3]))

    # Environment is ready:
    tcp.send(bytes([MainCommands.ENVIRONMENT_READY]))

    while True:
        try:
            data : bytes = tcp.recv(1)

            if not data:
                break

            if data == bytes([MainCommands.REQUEST_BUFFER]):
                buffer : bytes = example.get_png()
                size : int = len(buffer)
                    
                shm.buf[0:8] = size.to_bytes(8, "little")
                shm.buf[8:8+size] = buffer

                tcp.send(bytes([MainCommands.BUFFER_READY]))
            
        except ConnectionError:
            break

    shm.close()
    tcp.close()

if __name__ == "__main__":
    main()
