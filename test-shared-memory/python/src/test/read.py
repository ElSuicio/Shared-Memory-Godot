# Python Shared Memory Example.
from multiprocessing import shared_memory
from multiprocessing import resource_tracker

NAME : str = "SharedMemoryExample"
SIZE : int = 1024

def main() -> None:
    shm : shared_memory.SharedMemory = shared_memory.SharedMemory(name = NAME, create = False, size = SIZE)
    
    resource_tracker.unregister(shm._name, 'shared_memory')
    
    data : bytes = bytes(shm.buf[:]).rstrip(b'\x00')

    print(data.decode("utf-8"))
    
    shm.close()

if __name__ == "__main__":
    main()
