extends MainStateBase
##

func on_process(_delta : float) -> void:
	main.tcp.poll()
	
	if main.tcp.get_status() != StreamPeerTCP.STATUS_CONNECTED:
		return
	
	if !main.tcp.get_available_bytes():
		return
	
	if main.tcp.get_8() == MainCommands.BUFFER_READY:
		var size : int = main.shm.read(8).decode_u64(0)
		var data : PackedByteArray = main.shm.read(size, 8)
		
		var img = Image.new()
		img.load_png_from_buffer(data)
		main._TextureRect.texture = ImageTexture.create_from_image(img)
		
		main.tcp.put_data(PackedByteArray([MainCommands.REQUEST_BUFFER]))
	
