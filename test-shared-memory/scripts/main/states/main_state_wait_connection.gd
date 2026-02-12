extends MainStateBase

func on_process(_delta : float) -> void:
	if main.tcp_server.is_connection_available():
		# Save connection:
		main.tcp = main.tcp_server.take_connection()
		
		# Change state (wait environment):
		self.state_machine.change_to(MainStates.WAIT_ENVIRONMENT)

func end() -> void:
	# Early unlink of shared memory:
	main.shm.unlink()
	# Stop listening to connections:
	main.tcp_server.stop()
