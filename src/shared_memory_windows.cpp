#include "shared_memory.h"
#include "windows.h"

using namespace godot;

uint64_t SharedMemory::_get_mapped_size_os() const {
	MEMORY_BASIC_INFORMATION mbi;
	if (VirtualQuery(os_ptr, &mbi, sizeof(mbi))) {
		return mbi.RegionSize;
	}

	ERR_PRINT(vformat(
		"SharedMemory.get_mapped_size(): VirtualQuery failed (Windows error %d).",
		(int)GetLastError()
	));

	return 0;
}

Error SharedMemory::_create_os(const StringName& p_name, const int64_t p_size, const int64_t p_scope) {
	String win_name = String();
	
	switch (p_scope) {
		case SCOPE_LOCAL:
			win_name = "Local\\" + String(p_name);
			break;
		case SCOPE_GLOBAL:
			win_name = "Global\\" + String(p_name);
			break;
		default:
			return _fail(
				ERR_INVALID_PARAMETER,
				"SharedMemory.create(): invalid scope."
			);
	}
		
	HANDLE hmap = CreateFileMappingW(
		INVALID_HANDLE_VALUE,
		nullptr,
		PAGE_READWRITE,
		DWORD(p_size >> 32),
		DWORD(p_size & 0xFFFFFFFF),
		(LPCWSTR)win_name.wide_string().get_data()
	);

	if (!hmap) {
		return _fail(
			ERR_CANT_CREATE,
			vformat(
				"SharedMemory.create(): CreateFileMappingW failed (Windows error %d).",
				(int)GetLastError()
			)
		);
	}

	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		CloseHandle(hmap);
		return _fail(
			ERR_ALREADY_EXISTS,
			"SharedMemory.create(): Object already exists."
		);
	}

	void* ptr = MapViewOfFile(
		hmap,
		FILE_MAP_ALL_ACCESS,
		0,
		0,
		p_size
	);

	if (!ptr) {
		CloseHandle(hmap);
		return _fail(
			ERR_CANT_CREATE,
			vformat(
				"SharedMemory.create(): MapViewOfFile failed (Windows error %d).",
				(int)GetLastError()
			)
		);
	}

	os_handle = hmap;
	os_ptr = ptr;

	return OK;
}

Error SharedMemory::_open_os(const StringName& p_name, int64_t& p_size) {
	const PackedStringArray prefixes = {
		"Local\\",
		"Global\\"
	};

	HANDLE hmap = nullptr;
	DWORD last_error = ERROR_SUCCESS;

	for (const String& prefix : prefixes) {
		String win_name = prefix + String(p_name);

		hmap = OpenFileMappingW(
			FILE_MAP_ALL_ACCESS,
			FALSE,
			(LPCWSTR)win_name.wide_string().get_data()
		);

		if (hmap) {
			break;
		}

		last_error = GetLastError();
	}
	
	if (!hmap) {
		return _fail(
			ERR_CANT_OPEN,
			vformat(
				"SharedMemory.open(): OpenFileMappingW failed (Windows error %d).",
				(int)last_error
			)
		);
	}

	void* ptr = MapViewOfFile(
		hmap,
		FILE_MAP_ALL_ACCESS,
		0,
		0,
		p_size
	);

	if (!ptr) {
		CloseHandle(hmap);
		return _fail(
			ERR_CANT_OPEN,
			vformat(
				"SharedMemory.open(): MapViewOfFile failed (Windows error %d).",
				(int)GetLastError()
			)
		);
	}

	if (p_size == 0) {
		// Returns the actual size of the mapped area.
		MEMORY_BASIC_INFORMATION mbi;
		if (VirtualQuery(ptr, &mbi, sizeof(mbi))) {
			p_size = mbi.RegionSize;
		}
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
