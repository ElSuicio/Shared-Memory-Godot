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
