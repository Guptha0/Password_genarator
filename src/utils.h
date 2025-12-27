/**
 * @file utils.h
 * @brief Utility functions and helpers
 * @version 1.0
 * @date 2024
 */

#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include <stddef.h>
#include <time.h>

/**
 * @brief Error codes for utility functions
 */
typedef enum {
    UTILS_SUCCESS = 0,
    UTILS_ERROR_MEMORY,
    UTILS_ERROR_FILE,
    UTILS_ERROR_INPUT,
    UTILS_ERROR_RANGE,
    UTILS_ERROR_UNKNOWN
} UtilsError;

/**
 * @brief Secure string structure
 */
typedef struct {
    char *data;
    size_t length;
    size_t capacity;
} SecureString;

/**
 * @brief Initialize secure random number generator
 * @return true if successful, false otherwise
 */
bool init_secure_random(void);

/**
 * @brief Generate cryptographically secure random bytes
 * @param buffer Buffer to fill with random bytes
 * @param size Number of bytes to generate
 * @return true if successful, false otherwise
 */
bool get_random_bytes(unsigned char *buffer, size_t size);

/**
 * @brief Generate secure random number in range
 * @param min Minimum value (inclusive)
 * @param max Maximum value (inclusive)
 * @return Random number in range
 */
unsigned int random_range(unsigned int min, unsigned int max);

/**
 * @brief Secure string initialization
 * @param str SecureString to initialize
 * @param initial_capacity Initial capacity
 * @return true if successful, false otherwise
 */
bool secure_string_init(SecureString *str, size_t initial_capacity);

/**
 * @brief Append character to secure string
 * @param str SecureString to append to
 * @param c Character to append
 * @return true if successful, false otherwise
 */
bool secure_string_append(SecureString *str, char c);

/**
 * @brief Append C string to secure string
 * @param str SecureString to append to
 * @param src String to append
 * @return true if successful, false otherwise
 */
bool secure_string_append_cstr(SecureString *str, const char *src);

/**
 * @brief Clear secure string (zero memory)
 * @param str SecureString to clear
 */
void secure_string_clear(SecureString *str);

/**
 * @brief Free secure string memory
 * @param str SecureString to free
 */
void secure_string_free(SecureString *str);

/**
 * @brief Securely clear memory (zero out)
 * @param ptr Pointer to memory
 * @param size Size of memory to clear
 */
void secure_clear(void *ptr, size_t size);

/**
 * @brief Get current timestamp as string
 * @param buffer Buffer to store timestamp
 * @param buffer_size Size of buffer
 * @param format Timestamp format (NULL for default)
 * @return Pointer to buffer
 */
char *get_timestamp(char *buffer, size_t buffer_size, const char *format);

/**
 * @brief Validate string input
 * @param str String to validate
 * @param max_length Maximum allowed length
 * @return true if valid, false otherwise
 */
bool validate_input_string(const char *str, size_t max_length);

/**
 * @brief Convert string to integer with validation
 * @param str String to convert
 * @param result Pointer to store result
 * @param min Minimum allowed value
 * @param max Maximum allowed value
 * @return true if conversion successful, false otherwise
 */
bool string_to_int(const char *str, int *result, int min, int max);

/**
 * @brief Yes/No prompt
 * @param prompt Prompt to display
 * @param default_yes Default to yes if empty input
 * @return true if yes, false if no
 */
bool prompt_yes_no(const char *prompt, bool default_yes);

/**
 * @brief Get integer input with validation
 * @param prompt Prompt to display
 * @param min Minimum value
 * @param max Maximum value
 * @param default_value Default value if empty input
 * @return Validated integer input
 */
int get_integer_input(const char *prompt, int min, int max, int default_value);

/**
 * @brief Trim whitespace from string
 * @param str String to trim
 * @return Trimmed string (original modified)
 */
char *trim_whitespace(char *str);

/**
 * @brief Check if file exists
 * @param filename File to check
 * @return true if exists, false otherwise
 */
bool file_exists(const char *filename);

/**
 * @brief Get file size
 * @param filename File to check
 * @return File size in bytes, -1 on error
 */
long get_file_size(const char *filename);

/**
 * @brief Sleep for milliseconds (cross-platform)
 * @param milliseconds Milliseconds to sleep
 */
void sleep_ms(unsigned int milliseconds);

#endif /* UTILS_H */