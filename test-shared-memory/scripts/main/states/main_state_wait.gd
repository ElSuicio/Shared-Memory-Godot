extends MainStateBase
##

func on_process(_delta : float) -> void:
	main.tcp.poll()
	
	if main.tcp.get_status() != StreamPeerTCP.STATUS_CONNECTED:
		return
	
	if !main.tcp.get_available_bytes():
		return
	
	match main.tcp.get_8():
		0x00:
			self.state_machine.change_to(MainStates.REQUEST)
		0x01:
			self.state_machine.change_to(MainStates.READ)
		_:
			return
