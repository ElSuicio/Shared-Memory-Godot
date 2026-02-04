# Godot Shared Memory Example.
extends Node

func _ready() -> void:
	var shm : SharedMemory = SharedMemory.new()

	if shm.create("SharedMemoryExample", 1024) == OK:
		shm.write(("Hello world from Godot Shared Memory!").to_utf8_buffer())
	
	await get_tree().create_timer(10).timeout
	
	shm.unlink()
	print("unlink")
	shm.close()
	print("close")
