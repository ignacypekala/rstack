/*
 * Implements rstack_read.
 */
#include "types.h"
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <ctype.h>
#include <errno.h>

static const int UINT64_MAX_STRING_LENGTH = 20;

/*
 * Returns the first non-zero non-whitespace character from a file stream or
 * EOF on error or end of file.
 * Sets found_zeros to indicate whether any zeros were found.
 */
static int rstack_skip_whitespace_and_zeros(FILE *file, bool *found_zeros) {
    int character;
    while ((character = fgetc(file)) != EOF && isspace(character));
    *found_zeros = false;
    while(character == '0') {
        *found_zeros = true;
        character = getc(file);
    }
    return character;
}

/*
 * Loads a uint64_t from a file stream to a buffer. Reads at most
 * UINT64_MAX_STRING_LENGTH characters, terminates the string with '\0'.
 * Returns the number of characters written (including '\0') or -2 on invalid
 * formatting (sets errno to EBADMSG or ERANGE).
 */
static int rstack_load_digits_to_buffer(
        FILE *file,
        char  buffer[UINT64_MAX_STRING_LENGTH + 1],
        int   first_char
  ) {
    int written_chars = 0;
    int character = first_char;

    while (character != EOF && written_chars < UINT64_MAX_STRING_LENGTH) {
        if (isdigit(character)) {
            buffer[written_chars++] = (char)character;
        } else if (isspace(character)) {
            break;
        } else {
            errno = EINVAL;
            return -2;
        }
        character = fgetc(file);
    }

    const bool buffer_full = written_chars == UINT64_MAX_STRING_LENGTH;
    if (character != EOF && buffer_full && !isspace(character)) {
        errno = isdigit(character) ? ERANGE : EINVAL;
        return -2;
    }

    buffer[written_chars++] = '\0';
    return written_chars;
}

/*
 * Reads a single uint64_t from a file stream, handling leading whitespace
 * and safely ignoring leading zeros.
 * Returns 0 on success, -1 if no number is found, -2 on invalid formatting
 * or range errors, and -3 on stream I/O failures.
 * Possible errno values on failure:
 * - EBADMSG: Encountered a non-digit character within a number sequence.
 * - ERANGE: The parsed sequence exceeds the limits of a uint64_t.
 * - EIO: An underlying file stream operation (fgetc) failed.
 */
static int rstack_read_number(FILE *file, uint64_t *number) {
    bool found_zeros;
    int character = rstack_skip_whitespace_and_zeros(file, &found_zeros);
    if (character == EOF) {
        if (ferror(file)) {
            errno = EIO;
            return -3;
        } 
        if (found_zeros) {
            *number = 0;
            return 0;
        } 
        return -1;
    }

    char buffer[UINT64_MAX_STRING_LENGTH + 1];
    int written_chars = rstack_load_digits_to_buffer(file, buffer, character);
    if (written_chars < 0) return written_chars;
    if (character == EOF) {
        if (ferror(file)) {
            errno = EIO;
            return -3;
        }
    }
    if (written_chars == 0) {
        if (found_zeros) {
            *number = 0;
            return 0;
        }
        return -1;
    }

    errno = 0;
    char *end_ptr = nullptr;
    *number = (uint64_t) strtoull(buffer, &end_ptr, 10);
    if (errno == ERANGE) {
        return -2;
    }
    return 0;
}

/*
 * Reads numerical values from a whitespace-separated text file and populates a
 * new rstack. Returns a pointer to the rstack, or nullptr on failure. 
 * Possible errno values:
 *  - EINVAL: path is nullptr.
 *  - EBADMSG: invalid file contents
 *  - ERANGE: a value in the file exceeds the range of uint64_t.
 *  - ENOMEM: memory allocation failed.
 *  - EIO: file reading failed
 *  And any values specified by the POSIX standard:
 *  https://pubs.opengroup.org/onlinepubs/9699919799/functions/fopen.html
 *  https://pubs.opengroup.org/onlinepubs/9699919799/functions/strtoul.html
 */
rstack_t *rstack_read(char const *path) {
    if (path == nullptr) {
        errno = EINVAL;
        return nullptr;
    }

    FILE *file = fopen(path, "r+");
    if (file == nullptr) {
        return nullptr;
    }

    rstack_t *stack = rstack_new();
    uint64_t number = 0;
    int code;
    while (!feof(file) && (code = rstack_read_number(file, &number)) == 0) {
        if (rstack_push_value(stack, number) != 0) {
            rstack_delete(stack);
            fclose(file);
            return nullptr;
        }
    }
    fclose(file);
    if (code < -1) {
        rstack_delete(stack);
        return nullptr;
    }
    return stack;
}
