#include <new>
#include <cstring>
#include <cstdio>
#include <cstdlib>

//.............................实现部分.............................//
namespace wuya{
#define DEBUG_NEW_HASHTABLESIZE 16384
#define DEBUG_NEW_HASH(p) (( reinterpret_cast<size_t>(p) >> 8) % DEBUG_NEW_HASHTABLESIZE)
#define DEBUG_NEW_FILENAME_LEN	20

	struct new_ptr_list_t {
		new_ptr_list_t*     next;
		char                file[DEBUG_NEW_FILENAME_LEN];
		int                 line;
		size_t              size;
	};

	static new_ptr_list_t* new_ptr_list[DEBUG_NEW_HASHTABLESIZE];
	bool check_leaks();

	class new_check_t {
	public:
		new_check_t() {
		}
		~new_check_t() {
			static bool has_checked = false;
			if (!has_checked) {
				check_leaks();
				has_checked = true;
			}
		}
		static size_t max_mem_use(size_t n=0){
			static size_t v = 0;
			return v+=n;
		}
		static int new_times(int n=0){
			static int v = 0;
			return v+=n;
		}
		static int new_arr_times(int n=0){
			static int v = 0;
			return v+=n;
		}
		static int delete_times(int n=0){
			static int v = 0;
			return v+=n;
		}
		static int delete_arr_times(int n=0){
			static int v = 0;
			return v+=n;
		}
	};
	static new_check_t new_check_object;

	bool check_leaks() {
		bool fLeaked = false;
		printf("%25s%d\n", "max memory used: ", new_check_object.max_mem_use());
		printf("%25s%d\n", "call new times: ", new_check_object.new_times());
		printf("%25s%d\n", "call new[] times: ", new_check_object.new_arr_times());
		printf("%25s%d\n", "call delete times: ", new_check_object.delete_times());
		printf("%25s%d\n", "call delete[] times: ", new_check_object.delete_arr_times());
		printf("\n");
		for (int i = 0; i < DEBUG_NEW_HASHTABLESIZE; ++i) {
			new_ptr_list_t* ptr = new_ptr_list[i];
			if (ptr == 0)
				continue;
			fLeaked = true;
			while (ptr) {
				printf("Leaked object at %p (size %u, %s:%d)\n",
					   (char*)ptr + sizeof(new_ptr_list_t),
					   ptr->size,
					   ptr->file,
					   ptr->line);
				ptr = ptr->next;
			}
		}
		return fLeaked;
	}
}

void* operator new(size_t size, const char* file, int line) {
	size_t s = size + sizeof(wuya::new_ptr_list_t);
	wuya::new_ptr_list_t* ptr = (wuya::new_ptr_list_t*)malloc(s);
	if (ptr == 0) {
		fprintf(stderr, "new:  out of memory when allocating %u bytes\n",
				size);
		abort();
	}
	void* pointer = (char*)ptr + sizeof(wuya::new_ptr_list_t);
	size_t hash_index = DEBUG_NEW_HASH(pointer);
	ptr->next = wuya::new_ptr_list[hash_index];
	strncpy(ptr->file, file, DEBUG_NEW_FILENAME_LEN - 1);
	ptr->file[DEBUG_NEW_FILENAME_LEN - 1] = '\0';
	ptr->line = line;
	ptr->size = size;
	wuya::new_ptr_list[hash_index] = ptr;
	wuya::new_check_object.max_mem_use(size);
	wuya::new_check_object.new_times(1);
	return pointer;
}

void* operator new[](size_t size, const char* file, int line) {
	wuya::new_check_object.new_times(-1);
	wuya::new_check_object.new_arr_times(1);
	return operator new(size, file, line);
}

void* operator new(size_t size) {
	return operator new(size, "<unknown_file>", 0);
}

void* operator new[](size_t size) {
	wuya::new_check_object.new_times(-1);
	wuya::new_check_object.new_arr_times(1);
	return operator new(size);
}

void* operator new(size_t size, const std::nothrow_t&) throw() {
	return operator new(size);
}

void* operator new[](size_t size, const std::nothrow_t&) throw() {
	wuya::new_check_object.new_times(-1);
	wuya::new_check_object.new_arr_times(1);
	return operator new[](size);
}

void operator delete(void* pointer) {
	if (pointer == 0)
		return;
	wuya::new_check_object.delete_times(1);
	size_t hash_index = DEBUG_NEW_HASH(pointer);
	wuya::new_ptr_list_t* ptr = wuya::new_ptr_list[hash_index];
	wuya::new_ptr_list_t* ptr_last = 0;
	while (ptr) {
		if ((char*)ptr + sizeof(wuya::new_ptr_list_t) == pointer) {
			if (ptr_last == 0)
				wuya::new_ptr_list[hash_index] = ptr->next;
			else
				ptr_last->next = ptr->next;
			free(ptr);
			return;
		}
		ptr_last = ptr;
		ptr = ptr->next;
	}
	fprintf(stderr, "delete: invalid pointer %p\n", pointer);
	abort();
}

void operator delete[](void* pointer) {
	wuya::new_check_object.delete_times(-1);
	wuya::new_check_object.delete_arr_times(1);
	operator delete(pointer);
}

void operator delete(void* pointer, const char* file, int line) {
	operator delete(pointer);
}

void operator delete[](void* pointer, const char* file, int line) {
	wuya::new_check_object.delete_times(-1);
	wuya::new_check_object.delete_arr_times(1);
	operator delete(pointer, file, line);
}

void operator delete(void* pointer, const std::nothrow_t&) {
	operator delete(pointer, "<unknown_file>", 0);
}

void operator delete[](void* pointer, const std::nothrow_t&) {
	wuya::new_check_object.delete_times(-1);
	wuya::new_check_object.delete_arr_times(1);
	operator delete(pointer, std::nothrow);
}

