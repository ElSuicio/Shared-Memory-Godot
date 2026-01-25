#include "shared_memory.h"
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void SharedMemory::_bind_methods() {
	ClassDB::bind_integer_constant("SharedMemory", "Status", "STATUS_UNINITIALIZED", STATUS_UNINITIALIZED, false);
	ClassDB::bind_integer_constant("SharedMemory", "Status", "STATUS_CREATED", STATUS_CREATED, false);
	ClassDB::bind_integer_constant("SharedMemory", "Status", "STATUS_OPEN", STATUS_OPEN, false);
	ClassDB::bind_integer_constant("SharedMemory", "Status", "STATUS_CLOSED", STATUS_CLOSED, false);
	ClassDB::bind_integer_constant("SharedMemory", "Status", "STATUS_ERROR", STATUS_ERROR, false);

	ClassDB::bind_integer_constant("SharedMemory", "Scope", "SCOPE_LOCAL", SCOPE_LOCAL, false);
	ClassDB::bind_integer_constant("SharedMemory", "Scope", "SCOPE_GLOBAL", SCOPE_GLOBAL, false);

	ClassDB::bind_method(D_METHOD("get_name"), &SharedMemory::get_name);
	ClassDB::bind_method(D_METHOD("get_size"), &SharedMemory::get_size);
	ClassDB::bind_method(D_METHOD("get_status"), &SharedMemory::get_status);

	ClassDB::bind_method(D_METHOD("create", "name", "size", "scope"), &SharedMemory::create, DEFVAL(0));

	ClassDB::bind_method(D_METHOD("open", "name", "size"), &SharedMemory::open);
	ClassDB::bind_method(D_METHOD("close"), &SharedMemory::close);
}

SharedMemory::SharedMemory() {
	// Initialize any variables here.
}

SharedMemory::~SharedMemory() {
	close();
}

Error SharedMemory::_fail(Error p_error, const String& p_message) {
	status = STATUS_ERROR;
	ERR_PRINT(p_message);
	return p_error;
}

StringName SharedMemory::get_name() const {
	return name;
}

uint64_t SharedMemory::get_size() const {
	return size;
}

uint8_t SharedMemory::get_status() const {
	return status;
}

Error SharedMemory::create(const StringName& p_name, const int64_t p_size, const int64_t p_scope) {	
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

	if (p_scope < 0 || p_scope >= SCOPE_MAX) {
		return _fail(
			ERR_INVALID_PARAMETER,
			"SharedMemory.create(): invalid scope."
		);
	}

	if (get_status() == STATUS_CREATED || get_status() == STATUS_OPEN) {
		ERR_PRINT("SharedMemory.create(): already created or open.");
		return ERR_ALREADY_IN_USE;
	}

	Error create_error = _create_os(p_name, p_size, p_scope);
	
	if (create_error != OK) {
		return create_error;
	}

	name = p_name;
	size = p_size;

	status = STATUS_CREATED;
	return OK;
}

Error SharedMemory::open(const StringName& p_name, const int64_t p_size) {
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

	Error open_error = _open_os(p_name, p_size);

	if (open_error != OK) {
		return open_error;
	}

	name = p_name;
	size = p_size;

	status = STATUS_OPEN;
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
