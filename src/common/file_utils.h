#ifndef FILE_UTILS_H
#define FILE_UTILS_H

int get_size_file(long* size_file, const char* filename);
int read_file_to_buffer(char** buf, const char* filename);

#endif  // FILE_UTILS_H