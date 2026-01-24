#include "shared_memory.h"
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void SharedMemory::_bind_methods() {
	ClassDB::bind_integer_constant("SharedMemory", "Status", "STATUS_UNINITIALIZED", STATUS_UNINITIALIZED, false);
	ClassDB::bind_integer_constant("SharedMemory", "Status", "STATUS_CREATED", STATUS_CREATED, false);
	ClassDB::bind_integer_constant("SharedMemory", "Status", "STATUS_OPEN", STATUS_OPEN, false);
	ClassDB::bind_integer_constant("SharedMemory", "Status", "STATUS_CLOSED", STATUS_CLOSED, false);
	ClassDB::bind_integer_constant("SharedMemory", "Status", "STATUS_ERROR", STATUS_ERROR, false);

	ClassDB::bind_method(D_METHOD("get_name"), &SharedMemory::get_name);
	ClassDB::bind_method(D_METHOD("get_size"), &SharedMemory::get_size);
	ClassDB::bind_method(D_METHOD("get_status"), &SharedMemory::get_status);

	ClassDB::bind_method(D_METHOD("create", "name", "size"), &SharedMemory::create);
	ClassDB::bind_method(D_METHOD("open", "name", "size"), &SharedMemory::open);

	ClassDB::bind_method(D_METHOD("close"), &SharedMemory::close);
}

SharedMemory::SharedMemory() {
	// Initialize any variables here.
}

SharedMemory::~SharedMemory() {
	// Add your cleanup here.
}

StringName SharedMemory::get_name() const {
	return name;
}

int64_t SharedMemory::get_size() const {
	return size;
}

int8_t SharedMemory::get_status() const {
	return status;
}

Error SharedMemory::create(const StringName& p_name, const int64_t p_size) {
	if (p_name.is_empty()) {
		status = STATUS_ERROR;
		return ERR_INVALID_PARAMETER;
	}

	if (p_size <= 0) {
		status = STATUS_ERROR;
		return ERR_INVALID_PARAMETER;
	}
	
	if (get_status() == STATUS_CREATED || get_status() == STATUS_OPEN) {
		status = STATUS_ERROR;
		return ERR_ALREADY_IN_USE;
	}

	name = p_name;
	size = p_size;
	
	status = STATUS_CREATED;
	return OK;
}

Error SharedMemory::open(const StringName& p_name, const int64_t p_size) {
	if (p_name.is_empty()) {
		status = STATUS_ERROR;
		return ERR_INVALID_PARAMETER;
	}

	if (p_size <= 0) {
		status = STATUS_ERROR;
		return ERR_INVALID_PARAMETER;
	}

	if (get_status() == STATUS_CREATED || get_status() == STATUS_OPEN) {
		status = STATUS_ERROR;
		return ERR_ALREADY_IN_USE;
	}

	name = p_name;
	size = p_size;

	status = STATUS_OPEN;
	return OK;
}

void SharedMemory::close() {

}
