extends MainStateBase
##

func start() -> void:
	var size : int = main.shm.read(8).decode_u64(0)
	var data : PackedByteArray = main.shm.read(size, 8)
	
	var img = Image.new()
	img.load_png_from_buffer(data)
	main._TextureRect.texture = ImageTexture.create_from_image(img)
	
	self.state_machine.change_to(MainStates.REQUEST)
