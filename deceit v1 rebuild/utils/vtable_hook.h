#pragma once
#include <Windows.h>
#include <stdio.h>
#include <assert.h>
class vtable_hook {
public:
	vtable_hook() {  };
	~vtable_hook() {
		free(original_vtable);
	}

	void setup(void* _class) {
		assert(_class && "_class is null are you out of your mind ? ");
		pVtable = (void**) * (void**)_class;
		vtable_size = get_vtable_size() - 1;
		original_vtable = (void**)malloc(vtable_size * sizeof(void*));
		memcpy(original_vtable, pVtable, vtable_size * sizeof(void*));
		printf("vtable_size : %d \n", vtable_size);
		printf("original_vtable : %p \n", original_vtable);
	}

	bool hook_virtual_function(int _index, void* to_hook) {
		assert(pVtable && "you forgot to Initialize class (this.setup(class)) you baka !!!");
		assert(_index <= vtable_size && "index is out of range ... did you cheat on me with other pointer !!");
		DWORD old_protect = 0;
		LPVOID address = (void*)(pVtable + (8 * (DWORD64)_index));
		VirtualProtect(address, sizeof(void*), PAGE_READWRITE, &old_protect);
		try {
			pVtable[_index] = to_hook;
		}
		catch (...) { return false; };
		VirtualProtect(address, sizeof(void*), old_protect, 0);
		return true;
	}
	void unhook_virtual_function(int _index) {
		assert(pVtable && "you forgot to Initialize class (this.setup(class)) you baka !!!");
		assert(_index <= vtable_size && "index is out of range ... did you cheat on me with other pointer !!");

		DWORD old_protect = 0;
		LPVOID address = (void*)(pVtable + (8 * (DWORD64)_index));
		VirtualProtect(address, sizeof(void*), PAGE_READWRITE, &old_protect);
		try {
			pVtable[_index] = original_vtable[_index];
		}
		catch (...) {};
		VirtualProtect(address, sizeof(void*), old_protect, 0);
	}
	template<typename type>
	type get_orignial_function(int _index) {
		assert(pVtable && "you forgot to Initialize class (this.setup(class)) you baka !!!");
		return (type)original_vtable[_index];
	}

	size_t get_vtable_size() {
		assert(pVtable && "you forgot to Initialize class (this.setup(class)) you baka !!!");
		MEMORY_BASIC_INFORMATION memInfo = { NULL };
		int m_nSize = -1;
		do {
			m_nSize++;
			VirtualQuery(reinterpret_cast<LPCVOID>(pVtable[m_nSize]), &memInfo, sizeof(memInfo));

		} while (memInfo.Protect == PAGE_EXECUTE_READ || memInfo.Protect == PAGE_EXECUTE_READWRITE);

		return m_nSize;
	}
	bool should_unhook() {
		return pVtable || original_vtable || is_used || vtable_size;
	}
private:
	size_t vtable_size = 0;
	void** pVtable = 0;
	void** original_vtable = 0;
	bool is_used = false;
};