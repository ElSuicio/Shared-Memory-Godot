# Python Shared Memory Example.
from multiprocessing import shared_memory
import time

NAME : str = "SharedMemoryExample"
SIZE : int = 1024

def main() -> None:
    shm : shared_memory.SharedMemory = shared_memory.SharedMemory(name = NAME, create = True, size = SIZE)

    data : bytes = b'Hello world from Python Shared Memory!'

    shm.buf[0:len(data)] = data

    time.sleep(10)

    shm.close()
    shm.unlink()

if __name__ == "__main__":
    main()
