#include "shared_memory.h"
#include "windows.h"

using namespace godot;

Error SharedMemory::_create_os(const StringName& p_name, const uint64_t p_size, const uint64_t p_scope) {
	String win_name = String();
	
	if (p_scope == SCOPE_LOCAL) {
		win_name = "Local\\" + String(p_name);
	}
	else if (p_scope == SCOPE_GLOBAL) {
		win_name = "Global\\" + String(p_name);
	}
	else {
		return ERR_INVALID_PARAMETER;
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
				"SharedMemory: OpenFileMappingW failed (Windows error %d).",
				(int)GetLastError()
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
			ERR_CANT_CREATE,
			vformat(
				"SharedMemory: MapViewOfFile failed (Windows error %d).",
				(int)GetLastError()
			)
		);
	}

	os_handle = hmap;
	os_ptr = ptr;

	return OK;
}

Error SharedMemory::_open_os(const StringName& p_name, const uint64_t p_size) {
	const wchar_t* prefixes[] = {
		L"Local\\",
		L"Global\\"
	};
	
	HANDLE hmap = nullptr;
	DWORD last_error = ERROR_SUCCESS;

	for (const wchar_t* prefix : prefixes) {
		String win_name = String(prefix) + String(p_name);

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
				"SharedMemory: OpenFileMappingW failed (Windows error %d).",
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
				"SharedMemory: MapViewOfFile failed (Windows error %d).",
				(int)GetLastError()
			)
		);
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
