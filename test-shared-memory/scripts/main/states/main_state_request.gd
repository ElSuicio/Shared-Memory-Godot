extends MainStateBase
##

func start() -> void:
	main.tcp.put_data(PackedByteArray([0x01]))
	self.state_machine.change_to(MainStates.WAIT)
