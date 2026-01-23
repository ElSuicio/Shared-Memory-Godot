#pragma once

#include <godot_cpp/classes/object.hpp>

namespace godot {
	class SharedMemory : public Object {
		GDCLASS(SharedMemory, Object)

	private:


	protected:
		static void _bind_methods();
	
	public:
		SharedMemory();
		~SharedMemory();
	
	};
}
