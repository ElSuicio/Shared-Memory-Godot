# Shared Memory - Godot
> [!WARNING]
>
> This addon does not provide synchronization methods. Concurrent access must be coordinated by the user.

This is an addon for **Godot 4.4+** that adds the `SharedMemory` class for creating, opening, and directly accessing shared memory segments for **inter-process communication (IPC)**.

### How to use
#### Write (Godot → Python):
``` gdscript
# Godot Shared Memory Example.
extends Node

func _ready() -> void:
	var shm : SharedMemory = SharedMemory.new()

    if shm.create("SharedMemoryExample", 1024, SharedMemory.LOCAL_SCOPE) == OK:
        shm.write(("Hello world from Godot Shared Memory!").to_utf8_buffer())

    await get_tree().create_timer(60).timeout
	
	shm.close()
```
#### Read (Python):
```python
# Python Shared Memory Example.
import mmap

SIZE : int = 1024

def main():
    mm = mmap.mmap(-1, SIZE, tagname="SharedMemoryExample")
    print(mm[:].rstrip(b'\x00'))

if __name__ == "__main__":
    main()
```

``` bash
Hello world from Godot Shared Memory!
```

##### Write (Python -> Godot):
``` python
# Python Shared Memory Example.
import mmap
import time

SIZE = 1024

def main():
    mm = mmap.mmap(-1, SIZE, tagname="SharedMemoryExample")

    mm.seek(0)
    mm.write(b"Hello world from Python Shared Memory!")

    time.sleep(60)

if __name__ == "__main__":
    main()
```
##### Read (Godot):
``` gdscript
# Godot Shared Memory Example.
extends Node

func rstrip(buffer : PackedByteArray) -> PackedByteArray:
	var out : PackedByteArray = buffer.duplicate()
	
	while out.size() > 0 and out[out.size() - 1] == 0:
		out.resize(out.size() - 1)
	
	return out

func _ready() -> void:
	var shm : SharedMemory = SharedMemory.new()
	
	if shm.open("SharedMemoryExample", 1024) == OK:
		var buffer : PackedByteArray = shm.read()
		buffer = rstrip(buffer)
		print(buffer.get_string_from_utf8())
	
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
scons platform=<platform> target=<target> arch=<architecture>
```

Example:
```bash
scons platform=windows target=template_debug arch=x86_64
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
cmake -S ./ -B build -A <architecture> -DGODOTCPP_TARGET=<target choice>
cmake --build build
```

Example:
```bash
cmake -S ./ -B build -A x64 -DGODOTCPP_TARGET=template_debug
cmake --build build
```

Multi-config generators:
```bash
# Debug
cmake --build build --config Debug

# Release
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
