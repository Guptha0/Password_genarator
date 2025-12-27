/**
 * @file config.h
 * @brief Configuration constants and defaults
 * @version 1.0
 * @date 2024
 */

#ifndef CONFIG_H
#define CONFIG_H

/**
 * @brief Program name and version
 */
#define PROGRAM_NAME "SecurePassGen"
#define VERSION_MAJOR 1
#define VERSION_MINOR 0
#define VERSION_PATCH 0

/**
 * @brief Default configuration values
 */
#define DEFAULT_PASSWORD_LENGTH 16
#define DEFAULT_BULK_COUNT 5
#define DEFAULT_CLIPBOARD_TIMEOUT 30
#define DEFAULT_ENTROPY_THRESHOLD 64.0

/**
 * @brief Character sets
 */
#define CHARSET_LOWERCASE "abcdefghijklmnopqrstuvwxyz"
#define CHARSET_UPPERCASE "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define CHARSET_NUMBERS "0123456789"
#define CHARSET_SPECIAL "!@#$%^&*"
#define CHARSET_AMBIGUOUS "lI1O0"

/**
 * @brief File paths and extensions
 */
#define CONFIG_FILENAME "securepassgen.conf"
#define HISTORY_FILENAME "passwords_history.txt"
#define BACKUP_EXTENSION ".bak"
#define ENCRYPTED_EXTENSION ".enc"

/**
 * @brief Security constants
 */
#define MIN_ENTROPY_BITS 40
#define MAX_ENTROPY_BITS 256
#define GPU_GUESSES_PER_SECOND 1e9  // 1 billion guesses per second

/**
 * @brief Color configuration (ANSI codes)
 */
#ifdef _WIN32
    #define ENABLE_COLORS 0
#else
    #define ENABLE_COLORS 1
#endif

#if ENABLE_COLORS
    #define COLOR_RESET "\033[0m"
    #define COLOR_BLACK "\033[30m"
    #define COLOR_RED "\033[31m"
    #define COLOR_GREEN "\033[32m"
    #define COLOR_YELLOW "\033[33m"
    #define COLOR_BLUE "\033[34m"
    #define COLOR_MAGENTA "\033[35m"
    #define COLOR_CYAN "\033[36m"
    #define COLOR_WHITE "\033[37m"
    #define COLOR_BRIGHT_BLACK "\033[90m"
    #define COLOR_BRIGHT_RED "\033[91m"
    #define COLOR_BRIGHT_GREEN "\033[92m"
    #define COLOR_BRIGHT_YELLOW "\033[93m"
    #define COLOR_BRIGHT_BLUE "\033[94m"
    #define COLOR_BRIGHT_MAGENTA "\033[95m"
    #define COLOR_BRIGHT_CYAN "\033[96m"
    #define COLOR_BRIGHT_WHITE "\033[97m"
    #define BOLD_ON "\033[1m"
    #define BOLD_OFF "\033[22m"
#else
    #define COLOR_RESET ""
    #define COLOR_BLACK ""
    #define COLOR_RED ""
    #define COLOR_GREEN ""
    #define COLOR_YELLOW ""
    #define COLOR_BLUE ""
    #define COLOR_MAGENTA ""
    #define COLOR_CYAN ""
    #define COLOR_WHITE ""
    #define COLOR_BRIGHT_BLACK ""
    #define COLOR_BRIGHT_RED ""
    #define COLOR_BRIGHT_GREEN ""
    #define COLOR_BRIGHT_YELLOW ""
    #define COLOR_BRIGHT_BLUE ""
    #define COLOR_BRIGHT_MAGENTA ""
    #define COLOR_BRIGHT_CYAN ""
    #define COLOR_BRIGHT_WHITE ""
    #define BOLD_ON ""
    #define BOLD_OFF ""
#endif

/**
 * @brief Strength thresholds
 */
#define STRENGTH_THRESHOLD_VERY_WEAK 20
#define STRENGTH_THRESHOLD_WEAK 40
#define STRENGTH_THRESHOLD_FAIR 60
#define STRENGTH_THRESHOLD_GOOD 75
#define STRENGTH_THRESHOLD_STRONG 90

/**
 * @brief UI constants
 */
#define PROGRESS_BAR_WIDTH 40
#define MAX_FILENAME_LENGTH 256
#define MAX_INPUT_LENGTH 1024

#endif /* CONFIG_H */