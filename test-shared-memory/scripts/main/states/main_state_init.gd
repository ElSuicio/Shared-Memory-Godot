extends MainStateBase
##

func start_python(interpreter_path : String, script_path : String, arguments : Array, open_console : bool) -> int:
	# Encode arguments.
	var payload : PackedByteArray = JSON.stringify(arguments).to_utf8_buffer()
	var encoded : String = Marshalls.raw_to_base64(payload)
	
	return OS.create_process(interpreter_path, [script_path, encoded], open_console)

func start() -> void:
	main.shm = SharedMemory.new()
	main.tcp_server = TCPServer.new()
	
	main.shm.create(main._ShmName, main._ShmSize)
	main.tcp_server.listen(main._TcpPort, main._TcpAddress)
	
	# Editor only:
	main.python_interpreter_path = ProjectSettings.globalize_path(main._PythonInterpreterPath)
	main.python_script_path = ProjectSettings.globalize_path(main._PythonScriptPath)
	
	await get_tree().create_timer(0.5).timeout
	
	# Start Python: 
	var arguments : Array = [
		main._ShmName,
		main._ShmSize,
		main._TcpAddress,
		main._TcpPort
	]
	
	main.python_pid = start_python(main.python_interpreter_path, main.python_script_path, arguments, false)

func on_process(_delta : float) -> void:
	if main.tcp_server.is_connection_available():
		main.tcp = main.tcp_server.take_connection()
		self.state_machine.change_to(MainStates.WAIT)

func end() -> void:
	main.shm.unlink()
	main.tcp_server.stop()
