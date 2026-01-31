extends Node

const SHM_NAME : StringName = "SharedMemoryExample"
const SHM_SIZE : int = 10000000

const SERVER_IP : String = "127.0.0.1"
const SERVER_PORT : int = 8080

@onready var shm : SharedMemory = SharedMemory.new()
@onready var client : StreamPeerTCP = StreamPeerTCP.new()

@export var texture_rect : TextureRect

var python_interpreter_path : String = ProjectSettings.globalize_path("res://python/.venv/Scripts/python")
var python_script_path : String = ProjectSettings.globalize_path("res://python/src/read.py")

func start_python(interpreter_path : String, script_path : String, arguments : Array, open_console : bool) -> int:
	var payload : PackedByteArray = JSON.stringify(arguments).to_utf8_buffer()
	var encoded = Marshalls.raw_to_base64(payload)
	
	return OS.create_process(interpreter_path, [script_path, encoded], open_console)

func _ready() -> void:
	pass
	var arguments : Array = [
		SHM_NAME,
		SHM_SIZE,
		SERVER_IP,
		SERVER_PORT
	]
	
	start_python(python_interpreter_path, python_script_path, arguments, true)
	
	await get_tree().create_timer(0.5).timeout
	
	client.connect_to_host(SERVER_IP, SERVER_PORT)
	

func _process(_delta: float) -> void:
	client.poll()
	
	if(client.get_status() != StreamPeerTCP.STATUS_CONNECTED):
		return
	
	if(!client.get_available_bytes()):
		return
	
	var cmd : int = client.get_u8()
	
	if(cmd != 0x01):
		return
	
	shm.open(SHM_NAME, SHM_SIZE)
	var size : int = shm.read(8).decode_u64(0)
	var buffer : PackedByteArray = shm.read(size, 8)
	
	var img = Image.new()
	img.load_png_from_buffer(buffer)
	texture_rect.texture = ImageTexture.create_from_image(img)
	
