
/*

Copyright 2026 Giovanni Gutiérrez Ramírez

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

*/

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
			STATUS_ERROR = 3
		};

		enum Scope {
			LOCAL_SCOPE = 0,
			GLOBAL_SCOPE = 1
		};

		StringName name;
		StringName original_name;
		uint64_t size = 0;
		uint64_t mapped_size = 0;
		uint8_t status = STATUS_UNINITIALIZED;

		void* os_handle = nullptr;
		void* os_ptr = nullptr;

		Error _fail(Error p_error, const String& p_message);

		uint64_t _get_mapped_size_os() const;

		Error _create_os(const StringName& p_name, const int64_t p_size, const int64_t p_scope);
		Error _open_os(const StringName& p_name, int64_t& p_size);

		void _close_os();
		void _unlink_os();

	protected:
		static void _bind_methods();

	public:
		SharedMemory();
		~SharedMemory();

		StringName get_name() const;
		uint64_t get_size() const;
		uint64_t get_mapped_size() const;
		uint8_t get_status() const;

		Error create(const StringName& p_name, const int64_t p_size, const int64_t p_scope);
		Error open(const StringName& p_name, int64_t p_size);

		PackedByteArray read(int64_t p_size, const int64_t p_offset) const;
		Error write(const PackedByteArray& p_data, const int64_t p_offset);

		void close();
		void unlink();
	};
}
