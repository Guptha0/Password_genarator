/**
 * @file main.c
 * @brief Main program implementation with command line parsing
 * @version 1.0
 * @date 2024
 */

#include "main.h"
#include "config.h"
#include "utils.h"
#include "password.h"
#include "security.h"
#include "ui.h"
#include "clipboard.h"
#include "file_ops.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <getopt.h>

/* Global variables */
static bool program_running = true;
static PasswordOptions current_options;
static UIConfig ui_config;

/**
 * @brief Get program version information
 */
VersionInfo get_version_info(void) {
    VersionInfo info;
    
    info.major = VERSION_MAJOR;
    info.minor = VERSION_MINOR;
    info.patch = VERSION_PATCH;
    info.build_date = __DATE__;
    info.build_time = __TIME__;
    
    return info;
}

/**
 * @brief Print help message
 */
void print_help(void) {
    VersionInfo version = get_version_info();
    
    printf("%s%s - Secure Password Generator v%d.%d.%d%s\n\n", 
           COLOR_BRIGHT_CYAN, PROGRAM_NAME, 
           version.major, version.minor, version.patch, COLOR_RESET);
    
    printf("%sUSAGE:%s\n", COLOR_BRIGHT_YELLOW, COLOR_RESET);
    printf("  %spassgen%s [OPTIONS]\n\n", COLOR_BRIGHT_WHITE, COLOR_RESET);
    
    printf("%sOPTIONS:%s\n", COLOR_BRIGHT_YELLOW, COLOR_RESET);
    printf("  %s-h, --help%s              Show this help message\n", 
           COLOR_BRIGHT_GREEN, COLOR_RESET);
    printf("  %s-v, --version%s           Show version information\n", 
           COLOR_BRIGHT_GREEN, COLOR_RESET);
    printf("  %s-i, --interactive%s       Run in interactive mode (default)\n", 
           COLOR_BRIGHT_GREEN, COLOR_RESET);
    printf("  %s-q, --quiet%s             Quiet mode (minimal output)\n", 
           COLOR_BRIGHT_GREEN, COLOR_RESET);
    printf("  %s-l, --length LEN%s        Password length (8-128, default: 16)\n", 
           COLOR_BRIGHT_GREEN, COLOR_RESET);
    printf("  %s-c, --count NUM%s         Number of passwords to generate (1-100)\n", 
           COLOR_BRIGHT_GREEN, COLOR_RESET);
    printf("  %s-u, --uppercase%s         Include uppercase letters (A-Z)\n", 
           COLOR_BRIGHT_GREEN, COLOR_RESET);
    printf("  %s-L, --lowercase%s         Include lowercase letters (a-z)\n", 
           COLOR_BRIGHT_GREEN, COLOR_RESET);
    printf("  %s-n, --numbers%s           Include numbers (0-9)\n", 
           COLOR_BRIGHT_GREEN, COLOR_RESET);
    printf("  %s-s, --special%s           Include special characters (!@#$%%^&*)\n", 
           COLOR_BRIGHT_GREEN, COLOR_RESET);
    printf("  %s-a, --avoid-ambiguous%s   Avoid ambiguous characters (l,I,1,O,0)\n", 
           COLOR_BRIGHT_GREEN, COLOR_RESET);
    printf("  %s-p, --pattern PAT%s       Generate from pattern (e.g., \"llUnss\")\n", 
           COLOR_BRIGHT_GREEN, COLOR_RESET);
    printf("  %s-o, --output FILE%s       Save passwords to file\n", 
           COLOR_BRIGHT_GREEN, COLOR_RESET);
    printf("  %s--format FORMAT%s         Output format: text, csv, json (default: text)\n", 
           COLOR_BRIGHT_GREEN, COLOR_RESET);
    printf("  %s--copy%s                  Copy password to clipboard\n", 
           COLOR_BRIGHT_GREEN, COLOR_RESET);
    printf("  %s--entropy%s               Show entropy information\n", 
           COLOR_BRIGHT_GREEN, COLOR_RESET);
    printf("  %s--strength%s              Show strength assessment\n", 
           COLOR_BRIGHT_GREEN, COLOR_RESET);
    printf("  %s--save-config%s           Save current settings as default\n", 
           COLOR_BRIGHT_GREEN, COLOR_RESET);
    printf("  %s--load-config FILE%s      Load configuration from file\n", 
           COLOR_BRIGHT_GREEN, COLOR_RESET);
    
    printf("\n%sEXAMPLES:%s\n", COLOR_BRIGHT_YELLOW, COLOR_RESET);
    printf("  %spassgen%s                    # Interactive mode\n", 
           COLOR_BRIGHT_WHITE, COLOR_RESET);
    printf("  %spassgen -l 32 -c 5%s         # Generate 5 passwords of 32 chars\n", 
           COLOR_BRIGHT_WHITE, COLOR_RESET);
    printf("  %spassgen -l 16 -u -n -s%s     # Include uppercase, numbers, special\n", 
           COLOR_BRIGHT_WHITE, COLOR_RESET);
    printf("  %spassgen -p \"llUnss\"%s       # Generate using pattern\n", 
           COLOR_BRIGHT_WHITE, COLOR_RESET);
    printf("  %spassgen -o passwords.txt%s   # Save to file\n", 
           COLOR_BRIGHT_WHITE, COLOR_RESET);
    printf("  %spassgen --copy --entropy%s   # Copy to clipboard with entropy\n", 
           COLOR_BRIGHT_WHITE, COLOR_RESET);
    
    printf("\n%sCHARACTER SETS:%s\n", COLOR_BRIGHT_YELLOW, COLOR_RESET);
    printf("  Default: %sa-z A-Z 0-9 !@#$%%^&*%s\n", COLOR_CYAN, COLOR_RESET);
    printf("  Pattern codes: %sl%s=lower, %sU%s=upper, %sn%s=number, %ss%s=special\n", 
           COLOR_GREEN, COLOR_RESET, 
           COLOR_GREEN, COLOR_RESET, 
           COLOR_GREEN, COLOR_RESET, 
           COLOR_GREEN, COLOR_RESET);
    
    printf("\n%sSECURITY:%s\n", COLOR_BRIGHT_YELLOW, COLOR_RESET);
    printf("  • Uses cryptographically secure random number generation\n");
    printf("  • No network access (local only)\n");
    printf("  • Passwords never logged unless explicitly saved\n");
    printf("  • Memory is securely cleared after use\n");
}

/**
 * @brief Print version information
 */
void print_version(void) {
    VersionInfo version = get_version_info();
    
    printf("%s%s - Secure Password Generator%s\n", 
           COLOR_BRIGHT_CYAN, PROGRAM_NAME, COLOR_RESET);
    printf("Version: %s%d.%d.%d%s\n", 
           COLOR_BRIGHT_GREEN, version.major, version.minor, version.patch, COLOR_RESET);
    printf("Build: %s%s %s%s\n", 
           COLOR_CYAN, version.build_date, version.build_time, COLOR_RESET);
    printf("Platform: %s", COLOR_CYAN);
    
#ifdef _WIN32
    printf("Windows");
#elif defined(__APPLE__)
    printf("macOS");
#elif defined(__linux__)
    printf("Linux");
#else
    printf("Unix");
#endif
    
    printf("%s\n", COLOR_RESET);
    printf("License: %sMIT Open Source%s\n", COLOR_CYAN, COLOR_RESET);
    printf("Website: %shttps://github.com/securepassgen%s\n", COLOR_CYAN, COLOR_RESET);
}

/**
 * @brief Print usage examples
 */
void print_examples(void) {
    printf("\n%sUSAGE EXAMPLES:%s\n\n", COLOR_BRIGHT_YELLOW, COLOR_RESET);
    
    printf("%sFor Beginners:%s\n", COLOR_BRIGHT_GREEN, COLOR_RESET);
    printf("  $ %spassgen%s\n", COLOR_BRIGHT_WHITE, COLOR_RESET);
    printf("  %s➔ Interactive menu with all options%s\n\n", COLOR_CYAN, COLOR_RESET);
    
    printf("%sFor Quick Generation:%s\n", COLOR_BRIGHT_GREEN, COLOR_RESET);
    printf("  $ %spassgen -l 16%s\n", COLOR_BRIGHT_WHITE, COLOR_RESET);
    printf("  %s➔ Single 16-character password%s\n\n", COLOR_CYAN, COLOR_RESET);
    
    printf("%sFor Bulk Generation:%s\n", COLOR_BRIGHT_GREEN, COLOR_RESET);
    printf("  $ %spassgen -l 24 -c 10 -o passwords.txt%s\n", COLOR_BRIGHT_WHITE, COLOR_RESET);
    printf("  %s➔ 10 passwords, 24 chars each, saved to file%s\n\n", COLOR_CYAN, COLOR_RESET);
    
    printf("%sFor Maximum Security:%s\n", COLOR_BRIGHT_GREEN, COLOR_RESET);
    printf("  $ %spassgen -l 32 -u -L -n -s -a --copy --entropy%s\n", COLOR_BRIGHT_WHITE, COLOR_RESET);
    printf("  %s➔ 32-char password with all char types, no ambiguous chars%s\n", COLOR_CYAN, COLOR_RESET);
    printf("  %s➔ Copied to clipboard with entropy info%s\n\n", COLOR_CYAN, COLOR_RESET);
    
    printf("%sFor Pattern-Based:%s\n", COLOR_BRIGHT_GREEN, COLOR_RESET);
    printf("  $ %spassgen -p \"llUnss\"%s\n", COLOR_BRIGHT_WHITE, COLOR_RESET);
    printf("  %s➔ Password matching pattern: lower, lower, Upper, number, special, special%s\n\n", COLOR_CYAN, COLOR_RESET);
    
    printf("%sFor System Integration:%s\n", COLOR_BRIGHT_GREEN, COLOR_RESET);
    printf("  $ %spassgen -q -l 20 -c 50 --format csv%s\n", COLOR_BRIGHT_WHITE, COLOR_RESET);
    printf("  %s➔ 50 passwords in CSV format (quiet mode)%s\n", COLOR_CYAN, COLOR_RESET);
}

/**
 * @brief Parse command line arguments
 */
bool parse_command_line(int argc, char *argv[], CommandLineOptions *options) {
    if (!options) {
        return false;
    }
    
    /* Initialize with defaults */
    memset(options, 0, sizeof(CommandLineOptions));
    options->mode = UI_MODE_INTERACTIVE;
    options->pass_opts = password_options_init();
    options->count = 1;
    options->copy_to_clipboard = false;
    options->show_help = false;
    options->show_version = false;
    options->show_entropy = false;
    options->quiet_mode = false;
    
    /* Define long options */
    static struct option long_options[] = {
        {"help", no_argument, 0, 'h'},
        {"version", no_argument, 0, 'v'},
        {"interactive", no_argument, 0, 'i'},
        {"quiet", no_argument, 0, 'q'},
        {"length", required_argument, 0, 'l'},
        {"count", required_argument, 0, 'c'},
        {"uppercase", no_argument, 0, 'u'},
        {"lowercase", no_argument, 0, 'L'},
        {"numbers", no_argument, 0, 'n'},
        {"special", no_argument, 0, 's'},
        {"avoid-ambiguous", no_argument, 0, 'a'},
        {"pattern", required_argument, 0, 'p'},
        {"output", required_argument, 0, 'o'},
        {"format", required_argument, 0, 0},
        {"copy", no_argument, 0, 0},
        {"entropy", no_argument, 0, 0},
        {"strength", no_argument, 0, 0},
        {"save-config", no_argument, 0, 0},
        {"load-config", required_argument, 0, 0},
        {0, 0, 0, 0}
    };
    
    int opt;
    int option_index = 0;
    
    while ((opt = getopt_long(argc, argv, "hvqil:c:uLnsap:o:", 
                              long_options, &option_index)) != -1) {
        switch (opt) {
            case 'h':
                options->show_help = true;
                break;
                
            case 'v':
                options->show_version = true;
                break;
                
            case 'i':
                options->mode = UI_MODE_INTERACTIVE;
                break;
                
            case 'q':
                options->quiet_mode = true;
                options->mode = UI_MODE_SILENT;
                break;
                
            case 'l':
                {
                    int length;
                    if (string_to_int(optarg, &length, MIN_PASSWORD_LENGTH, MAX_PASSWORD_LENGTH)) {
                        options->pass_opts.length = (size_t)length;
                    } else {
                        fprintf(stderr, "Invalid length: %s. Using default: %d\n", 
                                optarg, DEFAULT_PASSWORD_LENGTH);
                    }
                }
                break;
                
            case 'c':
                {
                    int count;
                    if (string_to_int(optarg, &count, 1, MAX_BULK_GENERATE)) {
                        options->count = count;
                    } else {
                        fprintf(stderr, "Invalid count: %s. Using default: 1\n", optarg);
                    }
                }
                break;
                
            case 'u':
                options->pass_opts.charset.uppercase = true;
                break;
                
            case 'L':
                options->pass_opts.charset.lowercase = true;
                break;
                
            case 'n':
                options->pass_opts.charset.numbers = true;
                break;
                
            case 's':
                options->pass_opts.charset.special = true;
                break;
                
            case 'a':
                options->pass_opts.charset.avoid_ambiguous = true;
                break;
                
            case 'p':
                options->output_file = optarg; /* Temporarily store pattern */
                /* Pattern mode has special handling */
                break;
                
            case 'o':
                options->output_file = optarg;
                break;
                
            case 0:
                /* Handle long options without short equivalents */
                if (strcmp(long_options[option_index].name, "copy") == 0) {
                    options->copy_to_clipboard = true;
                } else if (strcmp(long_options[option_index].name, "entropy") == 0) {
                    options->show_entropy = true;
                } else if (strcmp(long_options[option_index].name, "strength") == 0) {
                    options->show_entropy = true; /* Strength implies entropy */
                } else if (strcmp(long_options[option_index].name, "format") == 0) {
                    /* Format will be handled in output generation */
                    printf("Format: %s\n", optarg);
                } else if (strcmp(long_options[option_index].name, "save-config") == 0) {
                    /* Will be handled later */
                } else if (strcmp(long_options[option_index].name, "load-config") == 0) {
                    /* Will be handled later */
                }
                break;
                
            default:
                fprintf(stderr, "Unknown option: %c\n", opt);
                return false;
        }
    }
    
    /* If no character sets specified, use defaults */
    if (!options->pass_opts.charset.lowercase && 
        !options->pass_opts.charset.uppercase &&
        !options->pass_opts.charset.numbers && 
        !options->pass_opts.charset.special) {
        /* Use defaults */
        options->pass_opts.charset.lowercase = true;
        options->pass_opts.charset.uppercase = true;
        options->pass_opts.charset.numbers = true;
        options->pass_opts.charset.special = true;
    }
    
    return true;
}

/**
 * @brief Handle single password generation
 */
void handle_single_password(const CommandLineOptions *options) {
    if (!options) {
        return;
    }
    
    /* Generate password */
    printf("%sGenerating password...%s ", COLOR_BRIGHT_YELLOW, COLOR_RESET);
    fflush(stdout);
    
    PasswordResult result = generate_password(&options->pass_opts);
    
    if (!result.password) {
        print_error("Failed to generate password!");
        return;
    }
    
    printf("%s✅ Done!%s\n", COLOR_BRIGHT_GREEN, COLOR_RESET);
    
    /* Display password */
    if (!options->quiet_mode) {
        display_password_result(&result, &ui_config);
    } else {
        /* Quiet mode: just print the password */
        printf("%s\n", result.password);
    }
    
    /* Copy to clipboard if requested */
    if (options->copy_to_clipboard) {
        printf("%sCopying to clipboard...%s ", COLOR_BRIGHT_YELLOW, COLOR_RESET);
        fflush(stdout);
        
        ClipboardResult clip_result = copy_to_clipboard(result.password);
        
        if (clip_result == CLIPBOARD_SUCCESS) {
            printf("%s✅ Copied!%s\n", COLOR_BRIGHT_GREEN, COLOR_RESET);
        } else {
            printf("%s❌ Failed: %s%s\n", 
                   COLOR_BRIGHT_RED, get_clipboard_result_string(clip_result), COLOR_RESET);
        }
    }
    
    /* Save to file if requested */
    if (options->output_file) {
        printf("%sSaving to file...%s ", COLOR_BRIGHT_YELLOW, COLOR_RESET);
        fflush(stdout);
        
        bool saved = save_password_to_file(&result, options->output_file, 
                                          false, !options->quiet_mode);
        
        if (saved) {
            printf("%s✅ Saved to: %s%s\n", 
                   COLOR_BRIGHT_GREEN, options->output_file, COLOR_RESET);
        } else {
            printf("%s❌ Failed to save file%s\n", COLOR_BRIGHT_RED, COLOR_RESET);
        }
    }
    
    /* Show entropy if requested */
    if (options->show_entropy && !options->quiet_mode) {
        SecurityAssessment assessment = assess_password_security(result.password, result.length);
        print_security_assessment(&assessment);
    }
    
    /* Cleanup */
    free_password_result(&result);
}

/**
 * @brief Handle bulk password generation
 */
void handle_bulk_passwords(const CommandLineOptions *options) {
    if (!options || options->count <= 0) {
        return;
    }
    
    printf("%sGenerating %d passwords...%s ", 
           COLOR_BRIGHT_YELLOW, options->count, COLOR_RESET);
    fflush(stdout);
    
    /* Allocate memory for results */
    PasswordResult *results = (PasswordResult *)calloc(options->count, sizeof(PasswordResult));
    if (!results) {
        print_error("Memory allocation failed!");
        return;
    }
    
    /* Generate passwords */
    size_t generated = generate_bulk_passwords(&options->pass_opts, 
                                              options->count, results);
    
    if (generated != (size_t)options->count) {
        printf("%s❌ Generated only %zu/%d passwords%s\n", 
               COLOR_BRIGHT_RED, generated, options->count, COLOR_RESET);
        
        /* Free what we did generate */
        free_bulk_passwords(results, generated);
        free(results);
        return;
    }
    
    printf("%s✅ Done!%s\n", COLOR_BRIGHT_GREEN, COLOR_RESET);
    
    /* Display results */
    if (!options->quiet_mode) {
        display_bulk_results(results, generated, &ui_config);
    } else {
        /* Quiet mode: just print passwords */
        for (size_t i = 0; i < generated; i++) {
            printf("%s\n", results[i].password);
        }
    }
    
    /* Save to file if requested */
    if (options->output_file) {
        printf("%sSaving to file...%s ", COLOR_BRIGHT_YELLOW, COLOR_RESET);
        fflush(stdout);
        
        bool saved = false;
        const char *format = "text"; /* Default format */
        
        /* Check for format in filename extension */
        if (strstr(options->output_file, ".csv")) {
            saved = save_passwords_to_csv(results, generated, options->output_file);
        } else if (strstr(options->output_file, ".json")) {
            saved = save_passwords_to_json(results, generated, options->output_file);
        } else {
            saved = save_bulk_passwords_to_file(results, generated, 
                                               options->output_file, !options->quiet_mode);
        }
        
        if (saved) {
            printf("%s✅ Saved %zu passwords to: %s%s\n", 
                   COLOR_BRIGHT_GREEN, generated, options->output_file, COLOR_RESET);
        } else {
            printf("%s❌ Failed to save file%s\n", COLOR_BRIGHT_RED, COLOR_RESET);
        }
    }
    
    /* Cleanup */
    free_bulk_passwords(results, generated);
    free(results);
}

/**
 * @brief Handle pattern-based password generation
 */
void handle_pattern_password(const char *pattern, const CommandLineOptions *options) {
    if (!pattern || !options) {
        return;
    }
    
    printf("%sGenerating password from pattern \"%s\"...%s ", 
           COLOR_BRIGHT_YELLOW, pattern, COLOR_RESET);
    fflush(stdout);
    
    PasswordResult result = generate_password_from_pattern(pattern);
    
    if (!result.password) {
        print_error("Failed to generate password from pattern!");
        return;
    }
    
    printf("%s✅ Done!%s\n", COLOR_BRIGHT_GREEN, COLOR_RESET);
    
    /* Display password */
    if (!options->quiet_mode) {
        display_password_result(&result, &ui_config);
    } else {
        printf("%s\n", result.password);
    }
    
    /* Copy to clipboard if requested */
    if (options->copy_to_clipboard) {
        printf("%sCopying to clipboard...%s ", COLOR_BRIGHT_YELLOW, COLOR_RESET);
        fflush(stdout);
        
        ClipboardResult clip_result = copy_to_clipboard(result.password);
        
        if (clip_result == CLIPBOARD_SUCCESS) {
            printf("%s✅ Copied!%s\n", COLOR_BRIGHT_GREEN, COLOR_RESET);
        } else {
            printf("%s❌ Failed: %s%s\n", 
                   COLOR_BRIGHT_RED, get_clipboard_result_string(clip_result), COLOR_RESET);
        }
    }
    
    /* Save to file if requested */
    if (options->output_file) {
        printf("%sSaving to file...%s ", COLOR_BRIGHT_YELLOW, COLOR_RESET);
        fflush(stdout);
        
        bool saved = save_password_to_file(&result, options->output_file, 
                                          false, !options->quiet_mode);
        
        if (saved) {
            printf("%s✅ Saved to: %s%s\n", 
                   COLOR_BRIGHT_GREEN, options->output_file, COLOR_RESET);
        } else {
            printf("%s❌ Failed to save file%s\n", COLOR_BRIGHT_RED, COLOR_RESET);
        }
    }
    
    /* Cleanup */
    free_password_result(&result);
}

/**
 * @brief Run interactive mode
 */
void run_interactive_mode(void) {
    print_welcome_banner();
    
    /* Initialize systems */
    if (!init_secure_random()) {
        print_error("Failed to initialize secure random number generator!");
        return;
    }
    
    if (!clipboard_init()) {
        print_warning("Clipboard may not work properly on this system.");
    }
    
    current_options = password_options_init();
    ui_config = ui_config_init();
    
    while (program_running) {
        int choice = print_main_menu();
        
        switch (choice) {
            case 1:  /* Generate Password */
                {
                    printf("\n");
                    print_separator(ui_config.terminal_width, '═');
                    
                    printf("%sGenerating password...%s\n", 
                           COLOR_BRIGHT_YELLOW, COLOR_RESET);
                    
                    print_progress_bar(0, 40);
                    fflush(stdout);
                    
                    for (int i = 0; i <= 100; i += 10) {
                        sleep_ms(50);
                        printf("\r");
                        print_progress_bar(i, 40);
                        fflush(stdout);
                    }
                    
                    printf("\n");
                    
                    PasswordResult result = generate_password(&current_options);
                    
                    if (result.password) {
                        display_password_result(&result, &ui_config);
                        
                        /* Ask to copy to clipboard */
                        if (prompt_yes_no("Copy to clipboard?", true)) {
                            ClipboardResult clip_result = copy_to_clipboard(result.password);
                            if (clip_result == CLIPBOARD_SUCCESS) {
                                print_success("Password copied to clipboard!");
                            } else {
                                print_error(get_clipboard_result_string(clip_result));
                            }
                        }
                        
                        /* Ask to save to file */
                        if (prompt_yes_no("Save to file?", false)) {
                            char filename[256];
                            printf("%sEnter filename: %s", COLOR_BRIGHT_WHITE, COLOR_CYAN);
                            fgets(filename, sizeof(filename), stdin);
                            trim_whitespace(filename);
                            printf("%s", COLOR_RESET);
                            
                            if (strlen(filename) > 0) {
                                bool saved = save_password_to_file(&result, filename, 
                                                                  false, true);
                                if (saved) {
                                    print_success("Password saved to file!");
                                } else {
                                    print_error("Failed to save password!");
                                }
                            }
                        }
                        
                        free_password_result(&result);
                    } else {
                        print_error("Failed to generate password!");
                    }
                }
                break;
                
            case 2:  /* Generate Multiple Passwords */
                {
                    printf("\n");
                    print_separator(ui_config.terminal_width, '═');
                    
                    int count = get_integer_input("How many passwords?", 
                                                  1, MAX_BULK_GENERATE, 5);
                    
                    printf("%sGenerating %d passwords...%s\n", 
                           COLOR_BRIGHT_YELLOW, count, COLOR_RESET);
                    
                    PasswordResult *results = (PasswordResult *)calloc(count, sizeof(PasswordResult));
                    if (!results) {
                        print_error("Memory allocation failed!");
                        break;
                    }
                    
                    print_progress_bar(0, 40);
                    fflush(stdout);
                    
                    for (int i = 0; i < count; i++) {
                        results[i] = generate_password(&current_options);
                        
                        int progress = (i + 1) * 100 / count;
                        printf("\r");
                        print_progress_bar(progress, 40);
                        fflush(stdout);
                    }
                    
                    printf("\n");
                    
                    /* Check if all were generated successfully */
                    bool all_ok = true;
                    for (int i = 0; i < count; i++) {
                        if (!results[i].password) {
                            all_ok = false;
                            break;
                        }
                    }
                    
                    if (all_ok) {
                        display_bulk_results(results, count, &ui_config);
                        
                        /* Ask to save to file */
                        if (prompt_yes_no("Save to file?", false)) {
                            char filename[256];
                            printf("%sEnter filename: %s", COLOR_BRIGHT_WHITE, COLOR_CYAN);
                            fgets(filename, sizeof(filename), stdin);
                            trim_whitespace(filename);
                            printf("%s", COLOR_RESET);
                            
                            if (strlen(filename) > 0) {
                                bool saved = save_bulk_passwords_to_file(results, count, 
                                                                        filename, true);
                                if (saved) {
                                    print_success("Passwords saved to file!");
                                } else {
                                    print_error("Failed to save passwords!");
                                }
                            }
                        }
                    } else {
                        print_error("Failed to generate some passwords!");
                    }
                    
                    free_bulk_passwords(results, count);
                    free(results);
                }
                break;
                
            case 3:  /* Configure Options */
                {
                    bool options_changed = true;
                    
                    while (options_changed) {
                        printf("\n");
                        print_options_menu(&current_options);
                        
                        char input[32];
                        if (fgets(input, sizeof(input), stdin)) {
                            trim_whitespace(input);
                            
                            if (strlen(input) == 0) {
                                continue;
                            }
                            
                            int choice = atoi(input);
                            
                            switch (choice) {
                                case 0:
                                    options_changed = false;
                                    break;
                                    
                                case 1:
                                    {
                                        int length = get_integer_input("Password length", 
                                                                       MIN_PASSWORD_LENGTH, 
                                                                       MAX_PASSWORD_LENGTH, 
                                                                       (int)current_options.length);
                                        current_options.length = (size_t)length;
                                    }
                                    break;
                                    
                                case 2:
                                    current_options.charset.lowercase = !current_options.charset.lowercase;
                                    break;
                                    
                                case 3:
                                    current_options.charset.uppercase = !current_options.charset.uppercase;
                                    break;
                                    
                                case 4:
                                    current_options.charset.numbers = !current_options.charset.numbers;
                                    break;
                                    
                                case 5:
                                    current_options.charset.special = !current_options.charset.special;
                                    break;
                                    
                                case 6:
                                    current_options.charset.avoid_ambiguous = !current_options.charset.avoid_ambiguous;
                                    break;
                                    
                                case 7:
                                    current_options.require_all_types = !current_options.require_all_types;
                                    break;
                                    
                                case 8:
                                    {
                                        int min_nums = get_integer_input("Minimum numbers", 
                                                                         0, 
                                                                         (int)current_options.length, 
                                                                         (int)current_options.min_numbers);
                                        current_options.min_numbers = (size_t)min_nums;
                                    }
                                    break;
                                    
                                case 9:
                                    {
                                        int min_special = get_integer_input("Minimum special characters", 
                                                                            0, 
                                                                            (int)current_options.length, 
                                                                            (int)current_options.min_special);
                                        current_options.min_special = (size_t)min_special;
                                    }
                                    break;
                                    
                                default:
                                    printf("%sInvalid choice!%s\n", COLOR_BRIGHT_RED, COLOR_RESET);
                                    break;
                            }
                        }
                    }
                }
                break;
                
            case 4:  /* Check Password Strength */
                {
                    printf("\n");
                    print_separator(ui_config.terminal_width, '═');
                    
                    char password[256];
                    printf("%sEnter password to check: %s", 
                           COLOR_BRIGHT_WHITE, COLOR_CYAN);
                    
                    /* Hide input for security */
#ifdef _WIN32
                    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
                    DWORD mode;
                    GetConsoleMode(hStdin, &mode);
                    SetConsoleMode(hStdin, mode & ~ENABLE_ECHO_INPUT);
                    
                    fgets(password, sizeof(password), stdin);
                    
                    SetConsoleMode(hStdin, mode);
#else
                    struct termios oldt, newt;
                    tcgetattr(STDIN_FILENO, &oldt);
                    newt = oldt;
                    newt.c_lflag &= ~ECHO;
                    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
                    
                    fgets(password, sizeof(password), stdin);
                    
                    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#endif
                    
                    trim_whitespace(password);
                    printf("%s", COLOR_RESET);
                    
                    if (strlen(password) == 0) {
                        print_error("No password entered!");
                        break;
                    }
                    
                    printf("\n");
                    
                    SecurityAssessment assessment = assess_password_security(password, strlen(password));
                    show_password_with_strength(password, assessment.score);
                    print_security_assessment(&assessment);
                }
                break;
                
            case 5:  /* Save to File */
                {
                    printf("\n");
                    print_separator(ui_config.terminal_width, '═');
                    
                    printf("%sSave Generated Password to File%s\n\n", 
                           COLOR_BRIGHT_CYAN, COLOR_RESET);
                    
                    /* First generate a password */
                    PasswordResult result = generate_password(&current_options);
                    
                    if (!result.password) {
                        print_error("Failed to generate password!");
                        break;
                    }
                    
                    display_password_result(&result, &ui_config);
                    
                    char filename[256];
                    printf("%sEnter filename: %s", COLOR_BRIGHT_WHITE, COLOR_CYAN);
                    fgets(filename, sizeof(filename), stdin);
                    trim_whitespace(filename);
                    printf("%s", COLOR_RESET);
                    
                    if (strlen(filename) > 0) {
                        bool saved = save_password_to_file(&result, filename, false, true);
                        if (saved) {
                            print_success("Password saved to file!");
                        } else {
                            print_error("Failed to save password!");
                        }
                    }
                    
                    free_password_result(&result);
                }
                break;
                
            case 6:  /* Copy to Clipboard */
                {
                    printf("\n");
                    print_separator(ui_config.terminal_width, '═');
                    
                    printf("%sCopy Generated Password to Clipboard%s\n\n", 
                           COLOR_BRIGHT_CYAN, COLOR_RESET);
                    
                    /* First generate a password */
                    PasswordResult result = generate_password(&current_options);
                    
                    if (!result.password) {
                        print_error("Failed to generate password!");
                        break;
                    }
                    
                    display_password_result(&result, &ui_config);
                    
                    if (prompt_yes_no("Copy to clipboard?", true)) {
                        ClipboardResult clip_result = copy_to_clipboard(result.password);
                        if (clip_result == CLIPBOARD_SUCCESS) {
                            print_success("Password copied to clipboard!");
                            
                            /* Ask about auto-clear */
                            if (prompt_yes_no("Enable auto-clear after 30 seconds?", true)) {
                                printf("%sNote: Auto-clear requires running timer (not implemented in console)%s\n",
                                       COLOR_BRIGHT_YELLOW, COLOR_RESET);
                            }
                        } else {
                            print_error(get_clipboard_result_string(clip_result));
                        }
                    }
                    
                    free_password_result(&result);
                }
                break;
                
            case 7:  /* Show Help */
                print_help();
                print_examples();
                printf("\n%sPress Enter to continue...%s", COLOR_BRIGHT_WHITE, COLOR_RESET);
                getchar();
                break;
                
            case 8:  /* Exit */
                program_running = false;
                print_success("Goodbye! Stay secure!");
                break;
                
            default:
                printf("%sInvalid choice! Please enter 1-8.%s\n", 
                       COLOR_BRIGHT_RED, COLOR_RESET);
                break;
        }
    }
}

/**
 * @brief Main program entry point
 */
int main(int argc, char *argv[]) {
    /* Initialize systems */
    if (!init_secure_random()) {
        fprintf(stderr, "FATAL: Failed to initialize secure random number generator!\n");
        return 1;
    }
    
    /* Parse command line arguments */
    CommandLineOptions options;
    
    if (!parse_command_line(argc, argv, &options)) {
        print_help();
        return 1;
    }
    
    /* Handle help and version requests */
    if (options.show_help) {
        print_help();
        return 0;
    }
    
    if (options.show_version) {
        print_version();
        return 0;
    }
    
    /* Initialize clipboard */
    if (!clipboard_init()) {
        if (!options.quiet_mode) {
            print_warning("Clipboard may not work properly on this system.");
        }
    }
    
    /* Run in appropriate mode */
    if (options.mode == UI_MODE_INTERACTIVE) {
        run_interactive_mode();
    } else {
        /* Command line mode */
        
        /* Check if we have a pattern */
        for (int i = 1; i < argc; i++) {
            if (strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--pattern") == 0) {
                if (i + 1 < argc) {
                    handle_pattern_password(argv[i + 1], &options);
                    return 0;
                }
            }
        }
        
        /* Normal generation */
        if (options.count == 1) {
            handle_single_password(&options);
        } else {
            handle_bulk_passwords(&options);
        }
    }
    
    /* Cleanup */
    clipboard_cleanup();
    
    return 0;
}