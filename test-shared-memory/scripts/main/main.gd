extends Node
##
class_name Main

@export var _TextureRect : TextureRect ##

# Editor only:
# In Windows: res://python/.venv/Scripts/python
# In Linux/macOS: res://python/.venv/bin/python
@export var _PythonInterpreterPath : String ## Python interpreter path.
@export var _PythonScriptPath : String ## Main Python script path.

# Export only:
@export var _PythonExecutablePath : String ## Your Python app.

@export var _ShmName : String ## Shared memory name.
@export var _ShmSize : int ## Shared memory size.
@export var _TcpAddress : String ## TCP address.
@export var _TcpPort : int ## TCP port.

var python_interpreter_path : String
var python_script_path : String

var python_executable_path : String

var shm : SharedMemory
var tcp_server : TCPServer
var tcp : StreamPeerTCP

var python_pid : int
