#include "futils.h"

long vut_fut_get_size_FILE(FILE *fp) {
	fseek(fp, 0, SEEK_END);
	int size = ftell(fp);
	rewind(fp);
	return size;
}

struct vut_str vut_fut_read_all_FILE(FILE *fp, struct vut_allocator alloc) {
	static const int READ_BUFF_CAPACITY = 1024;

	struct vut_str str = vut_str_init(alloc);

	char buffer[READ_BUFF_CAPACITY];
	size_t bytes_read;

	// This is done to be certain that w are at the beginning of the file
	rewind(fp);
	while ((bytes_read = fread(buffer, 1, READ_BUFF_CAPACITY, fp)) > 0) {
		VUT_VEC_PUT_MANY(&str, buffer, bytes_read);
	}

	if (ferror(fp)) {
		vut_str_free(&str);
	}

	return str;
}
