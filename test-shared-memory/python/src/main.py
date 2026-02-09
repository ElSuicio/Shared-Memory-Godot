import os
import sys
import json
import base64
import socket

from typing import Any

from multiprocessing import shared_memory
from multiprocessing import resource_tracker

NAME : str = "SharedMemoryExample"
SIZE : int = 1024
ADDRESS : str = "127.0.0.1"
PORT : int = 8080

def main() -> None:
    # Decode arguments.
    raw : bytes = base64.b64decode(sys.argv[1])
    args : list[Any] = json.loads(raw.decode("utf-8"))
    
    shm : shared_memory.SharedMemory = shared_memory.SharedMemory(name = args[0], create = False, size = args[1])

    if os.name != "nt":
        try:
            from multiprocessing import resource_tracker
            resource_tracker.unregister(shm._name, 'shared_memory')
        except Exception:
            pass

    tcp : socket.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    tcp.connect((args[2], args[3]))

    

    os.system("pause")
    shm.close()
    tcp.close()

if __name__ == "__main__":
    main()
