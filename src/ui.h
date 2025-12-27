/**
 * @file ui.h
 * @brief User interface functions for password generator
 * @version 1.0
 * @date 2024
 */

#ifndef UI_H
#define UI_H

#include "password.h"

/**
 * @brief UI display modes
 */
typedef enum {
    UI_MODE_INTERACTIVE,    /**< Interactive menu mode */
    UI_MODE_COMMAND_LINE,   /**< Command line mode */
    UI_MODE_SILENT,         /**< Silent mode (no UI) */
    UI_MODE_HELP            /**< Help mode */
} UIMode;

/**
 * @brief Color codes for terminal output
 */
typedef enum {
    COLOR_RESET = 0,
    COLOR_BLACK,
    COLOR_RED,
    COLOR_GREEN,
    COLOR_YELLOW,
    COLOR_BLUE,
    COLOR_MAGENTA,
    COLOR_CYAN,
    COLOR_WHITE,
    COLOR_BRIGHT_BLACK,
    COLOR_BRIGHT_RED,
    COLOR_BRIGHT_GREEN,
    COLOR_BRIGHT_YELLOW,
    COLOR_BRIGHT_BLUE,
    COLOR_BRIGHT_MAGENTA,
    COLOR_BRIGHT_CYAN,
    COLOR_BRIGHT_WHITE
} TerminalColor;

/**
 * @brief UI configuration structure
 */
typedef struct {
    UIMode mode;                /**< Current UI mode */
    bool use_colors;            /**< Enable colored output */
    bool show_progress;         /**< Show progress indicators */
    bool detailed_output;       /**< Show detailed information */
    int terminal_width;         /**< Terminal width for formatting */
} UIConfig;

/**
 * @brief Initialize UI configuration with defaults
 * @return UIConfig structure
 */
UIConfig ui_config_init(void);

/**
 * @brief Run interactive menu mode
 * @param config UI configuration
 */
void run_interactive_mode(UIConfig *config);

/**
 * @brief Print welcome banner
 */
void print_welcome_banner(void);

/**
 * @brief Print main menu
 * @return Selected menu option
 */
int print_main_menu(void);

/**
 * @brief Print password generation options menu
 * @param current_options Current password options
 */
void print_options_menu(const PasswordOptions *current_options);

/**
 * @brief Display generated password with styling
 * @param result Password result to display
 * @param config UI configuration
 */
void display_password_result(const PasswordResult *result, const UIConfig *config);

/**
 * @brief Display multiple password results
 * @param results Array of password results
 * @param count Number of results
 * @param config UI configuration
 */
void display_bulk_results(const PasswordResult *results, size_t count, const UIConfig *config);

/**
 * @brief Print colored text
 * @param text Text to print
 * @param color Color to use
 * @param bold Whether to use bold text
 */
void print_colored(const char *text, TerminalColor color, bool bold);

/**
 * @brief Print success message
 * @param message Message to print
 */
void print_success(const char *message);

/**
 * @brief Print error message
 * @param message Message to print
 */
void print_error(const char *message);

/**
 * @brief Print warning message
 * @param message Message to print
 */
void print_warning(const char *message);

/**
 * @brief Print info message
 * @param message Message to print
 */
void print_info(const char *message);

/**
 * @brief Clear screen (platform-specific)
 */
void clear_screen(void);

/**
 * @brief Get terminal width
 * @return Terminal width in characters
 */
int get_terminal_width(void);

/**
 * @brief Print separator line
 * @param width Width of separator
 * @param character Character to use for separator
 */
void print_separator(int width, char character);

/**
 * @brief Print progress bar
 * @param progress Progress percentage (0-100)
 * @param width Width of progress bar
 */
void print_progress_bar(int progress, int width);

#endif /* UI_H */