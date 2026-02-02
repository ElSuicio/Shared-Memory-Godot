
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

#include "shared_memory.h"
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

Error SharedMemory::_fail(Error p_error, const String& p_message) {
	status = STATUS_ERROR;
	ERR_PRINT(p_message);
	return p_error;
}

void SharedMemory::_bind_methods() {
	ClassDB::bind_integer_constant("SharedMemory", "Status", "STATUS_UNINITIALIZED", STATUS_UNINITIALIZED, false);
	ClassDB::bind_integer_constant("SharedMemory", "Status", "STATUS_CREATED", STATUS_CREATED, false);
	ClassDB::bind_integer_constant("SharedMemory", "Status", "STATUS_OPEN", STATUS_OPEN, false);
	ClassDB::bind_integer_constant("SharedMemory", "Status", "STATUS_CLOSED", STATUS_CLOSED, false);
	ClassDB::bind_integer_constant("SharedMemory", "Status", "STATUS_ERROR", STATUS_ERROR, false);

	ClassDB::bind_integer_constant("SharedMemory", "Scope", "LOCAL_SCOPE", LOCAL_SCOPE, false);
	ClassDB::bind_integer_constant("SharedMemory", "Scope", "GLOBAL_SCOPE", GLOBAL_SCOPE, false);

	ClassDB::bind_integer_constant("SharedMemory", "Access", "USER_ACCESS", USER_ACCESS, false);
	ClassDB::bind_integer_constant("SharedMemory", "Access", "GROUP_ACCESS", GROUP_ACCESS, false);
	ClassDB::bind_integer_constant("SharedMemory", "Access", "PUBLIC_ACCESS", PUBLIC_ACCESS, false);

	ClassDB::bind_method(D_METHOD("get_name"), &SharedMemory::get_name);
	ClassDB::bind_method(D_METHOD("get_size"), &SharedMemory::get_size);
	ClassDB::bind_method(D_METHOD("get_mapped_size"), &SharedMemory::get_mapped_size);
	ClassDB::bind_method(D_METHOD("get_status"), &SharedMemory::get_status);

	ClassDB::bind_method(D_METHOD("create", "name", "size", "scope", "access"), &SharedMemory::create, DEFVAL(0), DEFVAL(0));
	ClassDB::bind_method(D_METHOD("open", "name", "size"), &SharedMemory::open, DEFVAL(0));

	ClassDB::bind_method(D_METHOD("read", "size", "offset"), &SharedMemory::read, DEFVAL(0), DEFVAL(0));
	ClassDB::bind_method(D_METHOD("write", "data", "offset"), &SharedMemory::write, DEFVAL(0));

	ClassDB::bind_method(D_METHOD("close"), &SharedMemory::close);
}

SharedMemory::SharedMemory() {
	// Initialize any variables here.
}

SharedMemory::~SharedMemory() {
	close();
}

StringName SharedMemory::get_name() const {
	return name;
}

uint64_t SharedMemory::get_size() const {
	return size;
}

uint64_t SharedMemory::get_mapped_size() const {
	if (!os_ptr || !os_handle) {
		ERR_PRINT("SharedMemory.get_mapped_size(): unmapped memory.");
		return 0;
	}
	
	if (status == STATUS_CREATED || status == STATUS_OPEN) {
		return _get_mapped_size_os();
	}
	
	ERR_PRINT("SharedMemory.get_mapped_size(): invalid state.");
	return 0;
}

uint8_t SharedMemory::get_status() const {
	return status;
}

Error SharedMemory::create(const StringName& p_name, const int64_t p_size, const int64_t p_scope, const int64_t p_access) {	
	if (p_name.is_empty()) {
		return _fail(
			ERR_INVALID_PARAMETER,
			"SharedMemory.create(): name cannot be empty."
		);
	}

	if (p_size <= 0) {
		return _fail(
			ERR_INVALID_PARAMETER,
			"SharedMemory.create(): size must be greater than zero."
		);
	}

	if (get_status() == STATUS_CREATED || get_status() == STATUS_OPEN) {
		ERR_PRINT("SharedMemory.create(): already created or open.");
		return ERR_ALREADY_IN_USE;
	}

	Error create_error = _create_os(p_name, p_size, p_scope, p_access);
	
	if (create_error != OK) {
		return create_error;
	}

	name = p_name;
	size = p_size;

	status = STATUS_CREATED;
	return OK;
}

Error SharedMemory::open(const StringName& p_name, int64_t p_size) {
	if (p_name.is_empty()) {
		return _fail(
			ERR_INVALID_PARAMETER,
			"SharedMemory.open(): name cannot be empty."
		);
	}

	if (p_size < 0) {
		return _fail(
			ERR_INVALID_PARAMETER,
			"SharedMemory.open(): size must not be negative."
		);
	}

	if (get_status() == STATUS_CREATED || get_status() == STATUS_OPEN) {
		ERR_PRINT("SharedMemory.open(): already created or open.");
		return ERR_ALREADY_IN_USE;
	}

	Error open_error = _open_os(p_name, p_size);

	if (open_error != OK) {
		return open_error;
	}

	name = p_name;
	size = p_size;

	status = STATUS_OPEN;
	return OK;
}

PackedByteArray SharedMemory::read(int64_t p_size, const int64_t p_offset) const {
	PackedByteArray buffer = PackedByteArray();
	
	if (status != STATUS_CREATED && status != STATUS_OPEN) {
		ERR_PRINT("SharedMemory.read(): invalid state.");
		return buffer;
	}
	
	if (!os_ptr) {
		ERR_PRINT("SharedMemory.read(): invalid memory pointer.");
		return buffer;
	}

	if (p_offset < 0) {
		ERR_PRINT("SharedMemory.read(): offset must be greater than or equal to zero.");
		return buffer;
	}

	if (p_offset >= size) {
		ERR_PRINT("SharedMemory.read(): offset out of bounds.");
		return buffer;
	}

	if (p_size == 0) {
		p_size = size - p_offset;
	}

	if (p_size < 0) {
		ERR_PRINT("SharedMemory.read(): size must be greater than or equal to zero.");
		return buffer;
	}

	if (p_offset > size - p_size) {
		ERR_PRINT("SharedMemory.read(): size out of bounds.");
		return buffer;
	}

	buffer.resize(p_size);

	memcpy(
		buffer.ptrw(),
		static_cast<const uint8_t*>(os_ptr) + p_offset,
		p_size
	);

	return buffer;
}

Error SharedMemory::write(const PackedByteArray& p_data, const int64_t p_offset) {
	if (status != STATUS_CREATED && status != STATUS_OPEN) {
		ERR_PRINT("SharedMemory.write(): invalid state.");
		return ERR_UNCONFIGURED;
	}

	if (!os_ptr) {
		ERR_PRINT("SharedMemory.write(): invalid memory pointer.");
		return ERR_INVALID_DATA;
	}
	
	const int64_t data_size = p_data.size();

	if (data_size <= 0) {
		ERR_PRINT("SharedMemory.write(): data is empty.");
		return ERR_INVALID_PARAMETER;
	}

	if (p_offset < 0) {
		ERR_PRINT("SharedMemory.write(): offset must be greater than or equal to zero.");
		return ERR_INVALID_PARAMETER;
	}

	if (data_size > size) {
		ERR_PRINT("SharedMemory.write(): data exceeds shared memory size.");
		return ERR_INVALID_PARAMETER;
	}

	if (p_offset > size - data_size) {
		ERR_PRINT("SharedMemory.write(): read exceeds shared memory bounds.");
		return ERR_INVALID_PARAMETER;
	}

	memcpy(
		static_cast<uint8_t*>(os_ptr) + p_offset,
		p_data.ptr(),
		data_size
	);

	return OK;
}

void SharedMemory::close() {
	if (status == STATUS_CREATED || status == STATUS_OPEN) {
		_close_os();
		status = STATUS_CLOSED;
	}

	name = StringName();
	size = 0;

}
