
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
#include "process_handle_posix.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
//#include <cstring>

using namespace godot;

static constexpr size_t HEADER_SIZE = sizeof(ProcessHandle);



uint64_t SharedMemory::_get_mapped_size_os() const {
    return HEADER_SIZE + size;
}

Error SharedMemory::_create_os(const StringName& p_name, const int64_t p_size, const int64_t p_scope) {
	String posix_name = "/" + String(p_name);
	size_t posix_size = HEADER_SIZE + p_size;

	if (p_scope != LOCAL_SCOPE && p_name != GLOBAL_SCOPE) {
		return _fail(
			ERR_INVALID_PARAMETER,
			"SharedMemory.create(): invalid scope."
		);
	}

    int file_descriptor = ::shm_open(
        posix_name.utf8().get_data(),
        O_CREAT | O_EXCL | O_RDWR,
        0600
    );

    if (file_descriptor == -1) {
        if (errno == EEXIST) {
            return _fail(
                ERR_ALREADY_EXISTS,
                "SharedMemory.create(): object already exists."
            );
        }
        
        return _fail(
            ERR_CANT_CREATE,
			vformat(
				"SharedMemory.create(): shm_open failed (%s).",
				strerror(errno)
			)
        );
    }

	if (::ftruncate(file_descriptor, p_size) == -1) {
		::close(file_descriptor);
		::shm_unlink(posix_name.utf8().get_data());

		return _fail(
			ERR_CANT_CREATE,
			vformat(
				"SharedMemory.create(): ftruncate failed (%s).",
				strerror(errno)
			)
		);
	}

	void* ptr = ::mmap(
		nullptr,
		p_size,
		PROT_READ | PROT_WRITE,
		MAP_SHARED,
		file_descriptor,
		0
	);

	if (ptr == MAP_FAILED) {
		::close(file_descriptor);
		::shm_unlink(posix_name.utf8().get_data());

		return _fail(
			ERR_CANT_CREATE,
			vformat(
				"SharedMemory.create(): mmap failed (%s).",
				strerror(errno)
			)
		);
	}

	::close(file_descriptor);

	os_ptr = ptr;

    return OK;
}

Error SharedMemory::_open_os(const StringName& p_name, int64_t& p_size) {
	return OK;
}

void SharedMemory::_close_os() {
	if (os_ptr) {
		::munmap(os_ptr, size);
		os_ptr = nullptr;
	}

	if (status == STATUS_CREATED) {
		String posix_name = "/" + name;
		::shm_unlink(posix_name.utf8().get_data());
	}

}
