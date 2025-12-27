/**
 * @file main.h
 * @brief Main program definitions and command line parsing
 * @version 1.0
 * @date 2024
 */

#ifndef MAIN_H
#define MAIN_H

#include "password.h"
#include "ui.h"

/**
 * @brief Command line options structure
 */
typedef struct {
    UIMode mode;                /**< UI mode to use */
    PasswordOptions pass_opts;  /**< Password generation options */
    int count;                  /**< Number of passwords to generate */
    const char *output_file;    /**< Output file path */
    bool copy_to_clipboard;     /**< Copy to clipboard */
    bool show_help;             /**< Show help message */
    bool show_version;          /**< Show version info */
    bool show_entropy;          /**< Show entropy information */
    bool quiet_mode;            /**< Quiet output mode */
} CommandLineOptions;

/**
 * @brief Program version information
 */
typedef struct {
    int major;
    int minor;
    int patch;
    const char *build_date;
    const char *build_time;
} VersionInfo;

/**
 * @brief Get program version information
 * @return VersionInfo structure
 */
VersionInfo get_version_info(void);

/**
 * @brief Parse command line arguments
 * @param argc Argument count
 * @param argv Argument values
 * @param options Pointer to store parsed options
 * @return true if parsing successful, false otherwise
 */
bool parse_command_line(int argc, char *argv[], CommandLineOptions *options);

/**
 * @brief Print help message
 */
void print_help(void);

/**
 * @brief Print version information
 */
void print_version(void);

/**
 * @brief Print usage examples
 */
void print_examples(void);

/**
 * @brief Main program entry point
 * @param argc Argument count
 * @param argv Argument values
 * @return Program exit code
 */
int main(int argc, char *argv[]);

#endif /* MAIN_H */