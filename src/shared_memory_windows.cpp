
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

#include "shared_memory.hpp"
#include "windows.h"
#include "string"
#include "vector"

using namespace godot;

Error SharedMemory::_create_os(const StringName& p_name, int64_t p_size, int64_t p_scope) {
	std::wstring shm_name = String(p_name).wide_string();
	
	switch (p_scope) {
		case LOCAL_SCOPE:
			shm_name = L"Local\\" + shm_name;
			break;
		case GLOBAL_SCOPE:
			shm_name = L"Global\\" + shm_name;
			break;
		default:
			return _fail(ERR_INVALID_PARAMETER, "SharedMemory.create(): invalid scope.");
	}
		
	HANDLE hmap = CreateFileMappingW(
		INVALID_HANDLE_VALUE,
		nullptr,
		PAGE_READWRITE,
		DWORD(p_size >> 32),
		DWORD(p_size & 0xFFFFFFFF),
		shm_name.c_str()
	);

	if (!hmap) {
		DWORD error = GetLastError();
		return _fail(ERR_CANT_CREATE, vformat("SharedMemory.create(): CreateFileMappingW failed (Windows error %d).", (int)error));
	}

	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		CloseHandle(hmap);
		return _fail(ERR_ALREADY_EXISTS, "SharedMemory.create(): Object already exists.");
	}

	void* ptr = MapViewOfFile(
		hmap,
		FILE_MAP_ALL_ACCESS,
		0,
		0,
		p_size
	);

	if (!ptr) {
		DWORD error = GetLastError();
		CloseHandle(hmap);
		return _fail(ERR_CANT_CREATE, vformat("SharedMemory.create(): MapViewOfFile failed (Windows error %d).", (int)error));
	}

	MEMORY_BASIC_INFORMATION mbi;
	SIZE_T mbi_size = VirtualQuery(ptr, &mbi, sizeof(mbi));

	if (!mbi_size) {
		DWORD error = GetLastError();
		UnmapViewOfFile(ptr);
		CloseHandle(hmap);
		return _fail(ERR_CANT_CREATE, vformat("SharedMemory.create(): VirtualQuery failed (Windows error %d).", (int)error));
	}

	mapped_size = mbi.RegionSize;

	os_handle = hmap;
	os_ptr = ptr;

	return OK;
}

Error SharedMemory::_open_os(const StringName& p_name, int64_t* p_size) {
	const std::vector<std::wstring> prefixes = { L"Local\\", L"Global\\" };

	HANDLE hmap = nullptr;
	DWORD last_error = ERROR_SUCCESS;

	for (const std::wstring& prefix : prefixes) {
		std::wstring shm_name = String(p_name).wide_string();
		
		shm_name = prefix + shm_name;

		hmap = OpenFileMappingW(
			FILE_MAP_ALL_ACCESS,
			FALSE,
			shm_name.c_str()
		);

		if (hmap) {
			break;
		}

		last_error = GetLastError();
	}
	
	if (!hmap) {
		return _fail(ERR_CANT_OPEN, vformat("SharedMemory.open(): OpenFileMappingW failed (Windows error %d).", (int)last_error));
	}

	void* ptr = MapViewOfFile(
		hmap,
		FILE_MAP_ALL_ACCESS,
		0,
		0,
		0
	);

	if (!ptr) {
		DWORD error = GetLastError();
		CloseHandle(hmap);
		return _fail(ERR_CANT_OPEN, vformat("SharedMemory.open(): MapViewOfFile failed (Windows error %d).", (int)error));
	}

	MEMORY_BASIC_INFORMATION mbi;
	SIZE_T mbi_size = VirtualQuery(ptr, &mbi, sizeof(mbi));
	
	if (!mbi_size) {
		DWORD error = GetLastError();
		UnmapViewOfFile(ptr);
		CloseHandle(hmap);
		return _fail(ERR_CANT_OPEN, vformat("SharedMemory.open(): VirtualQuery failed (Windows error %d).", (int)error));
	}

	mapped_size = mbi.RegionSize;

	if (*p_size == 0) {
		*p_size = mapped_size;
	}
	else if (static_cast<uint64_t>(*p_size) > mapped_size) {
		UnmapViewOfFile(ptr);
		CloseHandle(hmap);
		return _fail(ERR_INVALID_PARAMETER, "SharedMemory.open(): requested size exceeds shared memory size.");
	}

	os_handle = hmap;
	os_ptr = ptr;

	return OK;
}

void SharedMemory::_close_os() {
	if (os_ptr) {
		UnmapViewOfFile(os_ptr);
		os_ptr = nullptr;
	}

	if (os_handle) {
		CloseHandle((HANDLE)os_handle);
		os_handle = nullptr;
	}

}

void SharedMemory::_unlink_os() {
	
}

uint64_t SharedMemory::_get_mapped_size_os() const {
	return mapped_size;
}
