#pragma once

#include <godot_cpp/classes/object.hpp>

namespace godot {

	class SharedMemory : public Object {
		GDCLASS(SharedMemory, Object)

	private:
		enum Status {
			STATUS_UNINITIALIZED = 0,
			STATUS_CREATED = 1,
			STATUS_OPEN = 2,
			STATUS_CLOSED = 3,
			STATUS_ERROR = 4
		};

		StringName name;
		int64_t size = 0;

		int8_t status = STATUS_UNINITIALIZED;

	protected:
		static void _bind_methods();

	public:
		SharedMemory();
		~SharedMemory();

		StringName get_name() const;
		int64_t get_size() const;
		int8_t get_status() const;

		Error create(const StringName& p_name, const int64_t p_size);
		Error open(const StringName& p_name, const int64_t p_size);

		void close();

	};
}
