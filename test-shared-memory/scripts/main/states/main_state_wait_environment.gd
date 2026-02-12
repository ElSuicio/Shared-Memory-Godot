extends MainStateBase
##

func on_process(_delta : float) -> void:
	main.tcp.poll()
	
	if main.tcp.get_status() != StreamPeerTCP.STATUS_CONNECTED:
		return
	
	if !main.tcp.get_available_bytes():
		return
	
	if main.tcp.get_8() == MainCommands.ENVIRONMENT_READY:
		self.state_machine.change_to(MainStates.PROCESS_AND_REQUEST)

func end() -> void:
	main.tcp.put_data(PackedByteArray([MainCommands.REQUEST_BUFFER]))
