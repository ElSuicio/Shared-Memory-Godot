import sys
import time
import json
import mmap
import base64
import socket

from typing import Any

from lib import example

def main() -> None:
    raw : bytes = base64.b64decode(sys.argv[1])
    args : list[Any] = json.loads(raw.decode("utf-8"))
    
    mm = mmap.mmap(-1, args[1], tagname=args[0])

    print("Python Shared Memory ready")

    server : socket.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server.bind((args[2], args[3]))
    server.listen(1)

    print("Python TCP server ready")

    connection : socket.socket
    address : tuple[str, int]
    
    connection, address = server.accept()

    print("Godot connection: " + str(address))

    buffer : bytes = example.get_png()
    size : int = len(buffer)

    mm.seek(0)
    mm.write(size.to_bytes(8, "little"))

    mm.seek(8)
    mm.write(buffer)

    connection.sendall(b'\x01')

    print("Send")

    time.sleep(1)

    print("cerrando...")
    connection.close()
    server.close()
    sys.exit(0)

if __name__ == "__main__":
    main()
