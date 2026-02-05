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
