# Shared Memory - Godot
> [!WARNING]
>
> This addon does not provide synchronization methods. Concurrent access must be coordinated by the user.

This is an addon for **Godot 4.4+** that adds the `SharedMemory` class for creating, opening and directly accessing shared memory segments for **inter-process communication (IPC)**.

### Binary downloads
The official binaries for Godot can be found at [itch.io](https://interdreamsoft.itch.io/shared-memory).

### How to use
#### Write (Godot → Python):
``` gdscript
# Godot Shared Memory Example.
extends Node

const NAME : String = "SharedMemoryExample"
const SIZE : int = 1024

func _ready() -> void:
	var shm : SharedMemory = SharedMemory.new()

	if shm.create(NAME, SIZE, SharedMemory.LOCAL_SCOPE) == OK:
		shm.write(("Hello world from Godot Shared Memory!").to_utf8_buffer())
	
	await get_tree().create_timer(10).timeout
	
	shm.close()
	shm.unlink()
```
#### Read (Python):
```python
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
```

``` bash
Hello world from Godot Shared Memory!
```

##### Write (Python → Godot):
``` python
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
```
##### Read (Godot):
``` gdscript
# Godot Shared Memory Example.
extends Node

const NAME : String = "SharedMemoryExample"
const SIZE : int = 1024

func rstrip(buffer : PackedByteArray) -> PackedByteArray:
	var out : PackedByteArray = buffer.duplicate()
	
	while out.size() > 0 and out[out.size() - 1] == 0:
		out.resize(out.size() - 1)
	
	return out

func _ready() -> void:
	var shm : SharedMemory = SharedMemory.new()
	
	if shm.open(NAME, SIZE) == OK:
		var data : PackedByteArray = shm.read()
		data = rstrip(data)
		print(data.get_string_from_utf8())
	
	shm.close()
```

``` bash
Hello world from Python Shared Memory!
```

## Compiling from source
### Requirements
- [Python 3.8+](https://www.python.org/downloads/)
- [SCons 4.0+](https://github.com/SCons/scons)
- [CMake 3.2+](https://cmake.org/download/)
- [C++ compiler](https://docs.godotengine.org/en/latest/engine_details/development/compiling/)

### How to build
#### Build with SCons
From the repository root, run:
```bash
scons platform=<platform> target=<target> precision=<precision> arch=<architecture>
```

Example:
```bash
scons platform=windows target=template_debug precision=single arch=x86_64
```

#### Build with CMake
From the repository root, run:
> [!NOTE]
>
> The default CMake generator depends on your platform:
>
> - **Windows**: Visual Studio (multi-config).
> - **Linux / macOS**: Makefiles or Ninja (single-config).
>
> Ninja is **recommended** on Linux / macOS for faster builds and better tooling integration but it is **not required**.
>

Basic build (default generator)

```bash
cmake -S ./ -B build -DGODOTCPP_TARGET=<target choice>
cmake --build build
```

Example:
```bash
cmake -S ./ -B build -DGODOTCPP_TARGET=template_debug
cmake --build build
```

Multi-config generators:
```bash
# Debug
cmake -S ./ -B build -A x64 -DGODOTCPP_TARGET=template_debug
cmake --build build --config Debug

# Release
cmake -S ./ -B build -A x64 -DGODOTCPP_TARGET=template_release
cmake --build build --config Release
```

Single-config generators:
```bash
# Debug
cmake -S ./ -B build -G Ninja -DGODOTCPP_TARGET=template_debug -DCMAKE_BUILD_TYPE=Debug
cmake --build build

# Release
cmake -S ./ -B build -G Ninja -DGODOTCPP_TARGET=template_release -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

[More details](https://docs.godotengine.org/en/latest/tutorials/scripting/cpp/build_system/cmake.html)

## Links & Support
- [itch.io](https://interdreamsoft.itch.io/)
- [x.com](https://x.com/ElSuicio)

If you liked it and want to support it, you can donate via [PayPal](https://www.paypal.com/donate/?hosted_button_id=NRD94T2N7XZ6J). Your support is very much appreciated and helps me keep going!


[ Contact Email: interdreamsoft@gmail.com ]
