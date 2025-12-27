/**
 * @file clipboard.h
 * @brief Cross-platform clipboard operations
 * @version 1.0
 * @date 2024
 */

#ifndef CLIPBOARD_H
#define CLIPBOARD_H

#include <stdbool.h>

/**
 * @brief Platform types for clipboard operations
 */
typedef enum {
    PLATFORM_WINDOWS,
    PLATFORM_LINUX,
    PLATFORM_MACOS,
    PLATFORM_UNKNOWN
} PlatformType;

/**
 * @brief Clipboard operation result codes
 */
typedef enum {
    CLIPBOARD_SUCCESS = 0,
    CLIPBOARD_ERROR_ALLOCATION,
    CLIPBOARD_ERROR_OPEN,
    CLIPBOARD_ERROR_EMPTY,
    CLIPBOARD_ERROR_PLATFORM,
    CLIPBOARD_ERROR_UNKNOWN
} ClipboardResult;

/**
 * @brief Initialize clipboard system
 * @return true if initialization successful, false otherwise
 */
bool clipboard_init(void);

/**
 * @brief Copy text to system clipboard
 * @param text Text to copy
 * @return ClipboardResult indicating success or error
 */
ClipboardResult copy_to_clipboard(const char *text);

/**
 * @brief Get text from system clipboard
 * @param buffer Buffer to store clipboard text
 * @param buffer_size Size of buffer
 * @return ClipboardResult indicating success or error
 */
ClipboardResult get_from_clipboard(char *buffer, size_t buffer_size);

/**
 * @brief Clear clipboard contents
 * @return ClipboardResult indicating success or error
 */
ClipboardResult clear_clipboard(void);

/**
 * @brief Copy with auto-clear after specified seconds
 * @param text Text to copy
 * @param seconds Seconds before auto-clear (0 = no auto-clear)
 * @return ClipboardResult indicating success or error
 */
ClipboardResult copy_with_autoclear(const char *text, int seconds);

/**
 * @brief Get current platform type
 * @return PlatformType enum value
 */
PlatformType get_current_platform(void);

/**
 * @brief Get string representation of clipboard result
 * @param result Clipboard result code
 * @return String description
 */
const char *get_clipboard_result_string(ClipboardResult result);

/**
 * @brief Cleanup clipboard system resources
 */
void clipboard_cleanup(void);

#endif /* CLIPBOARD_H */