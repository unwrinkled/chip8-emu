#include <stdio.h>
#include <string.h>

#define READ_BUFFER_LENGTH 1024
#define WRITE_BUFFER_LENGTH 1024
#define MAX_NAME_LENGTH 256

typedef unsigned char u8;

struct write_buffer {
  FILE *file;
  u8 *buffer;
  size_t capacity;
  size_t bytes_written;
};
typedef struct write_buffer write_buffer;

struct read_buffer {
  FILE *file;
  u8 *buffer;
  size_t capacity;
  size_t bytes_in_buffer;
  size_t bytes_read;
};
typedef struct read_buffer read_buffer;

enum rb_result {
  RB_EOF,
  RB_OK,
};
typedef enum rb_result rb_result;

static inline write_buffer wb_create(FILE *file, u8 *buffer, size_t capacity) {
  // TODO(geo): Add assertion on capacity.
  write_buffer wb = { file, buffer, capacity, 0 };
  return wb;
}

void wb_flush(write_buffer *buffer) {
  if (buffer->bytes_written == 0) {
    return;
  }

  // TODO(geo): Add error handling.
  fwrite(buffer->buffer, 1, buffer->bytes_written, buffer->file);
  buffer->bytes_written = 0;
}

void wb_write(write_buffer *buffer, char *data, size_t size) {
  while (size > 0) {
    size_t available_space = buffer->capacity - buffer->bytes_written;
    if (available_space == 0) {
      wb_flush(buffer);
      available_space = buffer->capacity;
    }

    size_t bytes_to_write = size;
    if (available_space < size) {
      bytes_to_write = available_space;
    }

    memcpy(&buffer->buffer[buffer->bytes_written], data, bytes_to_write);
    buffer->bytes_written += bytes_to_write;
    data += bytes_to_write;
    size -= bytes_to_write;
  }
}

void wb_write_str(write_buffer *buffer, char *data, size_t size) {
  wb_write(buffer, data, size - 1);
}

static inline read_buffer rb_create(FILE *file, u8 *buffer, size_t capacity) {
  // TODO(geo): Add assertion on capacity.
  read_buffer rb = { file, buffer, capacity, 0, 0 };
  return rb;
}

rb_result rb_fill_buffer(read_buffer *buffer) {
  buffer->bytes_in_buffer = fread(buffer->buffer, 1, buffer->capacity, buffer->file);
  buffer->bytes_read = 0;

  if (buffer->bytes_in_buffer == 0) {
    return RB_EOF;
  }
  
  return RB_OK;
}

rb_result rb_next_byte(read_buffer *buffer, u8 *byte) {
  if (buffer->bytes_read == buffer->bytes_in_buffer && rb_fill_buffer(buffer) == RB_EOF) {
    return RB_EOF;
  }

  *byte = buffer->buffer[buffer->bytes_read++];
  return RB_OK;
}

static inline char to_hex(u8 b) {
  return b < 10 ? '0' + b : 'a' + b - 10; 
}

static inline void convert_to_hex_fmt(char hex[2], u8 b) {
  u8 low_nibble = b & 0x0f;
  u8 high_nibble = (b >> 4) & 0x0f;

  hex[0] = to_hex(high_nibble);
  hex[1] = to_hex(low_nibble);
}

static inline void str_to_upper(char *str, size_t length) {
  for (size_t c = 0; c < length; ++c) {
    if (str[c] >= 'a' && str[c] <= 'z') {
      str[c] -= 32;
    }
  }
}

void newline_and_indent(write_buffer *buffer) {
  char str[] = "\n    ";
  wb_write_str(buffer, str, sizeof(str));
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Please provide a file path and a variable name.\n");
    return -1;
  }

  char *filename = argv[1];
  FILE *binary_file = fopen(filename, "r");
  if (binary_file == 0) {
    printf("Failed to open the provided file.\n");
    return -1;
  }

  char *result_name = argv[2];
  size_t name_length = strlen(result_name);
  if (name_length > MAX_NAME_LENGTH) {
    printf("Provide a name that is smaller than %d\n", MAX_NAME_LENGTH);
    return -1;
  }
  char output_file_name[MAX_NAME_LENGTH + 3] = {0};
  memcpy(output_file_name, result_name, name_length);
  output_file_name[name_length] = '.';
  output_file_name[name_length + 1] = 'h';
  FILE *output_file = fopen(output_file_name, "w");
  if (output_file == 0) {
    printf("Failed to create the output file.\n");
  }

  u8 _write_buffer[WRITE_BUFFER_LENGTH] = {0};
  write_buffer write_buffer = wb_create(output_file, _write_buffer, sizeof(_write_buffer));

  u8 _read_buffer[READ_BUFFER_LENGTH] = {0};
  read_buffer read_buffer = rb_create(binary_file, _read_buffer, sizeof(_read_buffer));

  output_file_name[name_length] = 0;
  output_file_name[name_length + 1] = 0;
  str_to_upper(output_file_name, name_length);
  char header_guard1[] = "#ifndef ";
  char header_guard2[] = "_H\n#define ";
  char header_guard3[] = "_H\n\n";
  wb_write_str(&write_buffer, header_guard1, sizeof(header_guard1));
  wb_write(&write_buffer, output_file_name, name_length);
  wb_write_str(&write_buffer, header_guard2, sizeof(header_guard2));
  wb_write(&write_buffer, output_file_name, name_length);
  wb_write_str(&write_buffer, header_guard3, sizeof(header_guard3));

  char variable_declaration1[] = "unsigned char ";
  char variable_declaration2[] = "_bytes[] = {";
  wb_write_str(&write_buffer, variable_declaration1, sizeof(variable_declaration1));
  wb_write(&write_buffer, result_name, name_length);
  wb_write_str(&write_buffer, variable_declaration2, sizeof(variable_declaration2));

  char hex_str[] = "0xXX, ";
  u8 byte_offset = 2;
  u8 next_byte = 0;

  u8 bytes_per_line = 16;
  u8 current_bytes_in_line = 0;

  while (rb_next_byte(&read_buffer, &next_byte) == RB_OK) {

    if (current_bytes_in_line == 0) {
      newline_and_indent(&write_buffer);
    }

    convert_to_hex_fmt(&hex_str[byte_offset], next_byte);
    wb_write_str(&write_buffer, hex_str, sizeof(hex_str));

    // NOTE(geo): This way I won't need to write an if for zeroing the current_bytes_in_line.
    current_bytes_in_line = (current_bytes_in_line + 1) % bytes_per_line;
  }
  // convert_to_hex_fmt(&hex_str[byte_offset], 0);
  // wb_write_str(&write_buffer, hex_str, sizeof(hex_str));

  fclose(binary_file);

  char closing1[] = "\n};\n\n#endif // ";
  char closing2[] = "_H\n";
  wb_write_str(&write_buffer, closing1, sizeof(closing1));
  wb_write(&write_buffer, output_file_name, name_length);
  wb_write_str(&write_buffer, closing2, sizeof(closing2));
  wb_flush(&write_buffer);
  fclose(output_file);

  return 0;
}
