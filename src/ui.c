/**
 * @file ui.c
 * @brief User interface functions implementation
 * @version 1.0
 * @date 2024
 */

#include "ui.h"
#include "config.h"
#include "utils.h"
#include "clipboard.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* Global UI configuration */
static UIConfig global_ui_config;

/**
 * @brief Initialize UI configuration with defaults
 */
UIConfig ui_config_init(void) {
    UIConfig config;
    
    config.mode = UI_MODE_INTERACTIVE;
    config.use_colors = true;
    config.show_progress = true;
    config.detailed_output = true;
    config.terminal_width = get_terminal_width();
    
    global_ui_config = config;
    return config;
}

/**
 * @brief Print welcome banner
 */
void print_welcome_banner(void) {
    clear_screen();
    
    printf("%s", COLOR_BRIGHT_CYAN);
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║                                                              ║\n");
    printf("║    %s██╗  ██╗███████╗ ██████╗██╗   ██╗███████╗██████╗ ███████╗   %s║\n", 
           COLOR_BRIGHT_WHITE, COLOR_BRIGHT_CYAN);
    printf("║    %s██║  ██║██╔════╝██╔════╝██║   ██║██╔════╝██╔══██╗██╔════╝   %s║\n",
           COLOR_BRIGHT_WHITE, COLOR_BRIGHT_CYAN);
    printf("║    %s███████║█████╗  ██║     ██║   ██║█████╗  ██████╔╝███████╗   %s║\n",
           COLOR_BRIGHT_WHITE, COLOR_BRIGHT_CYAN);
    printf("║    %s██╔══██║██╔══╝  ██║     ██║   ██║██╔══╝  ██╔══██╗╚════██║   %s║\n",
           COLOR_BRIGHT_WHITE, COLOR_BRIGHT_CYAN);
    printf("║    %s██║  ██║███████╗╚██████╗╚██████╔╝███████╗██║  ██║███████║   %s║\n",
           COLOR_BRIGHT_WHITE, COLOR_BRIGHT_CYAN);
    printf("║    %s╚═╝  ╚═╝╚══════╝ ╚═════╝ ╚═════╝ ╚══════╝╚═╝  ╚═╝╚══════╝   %s║\n",
           COLOR_BRIGHT_WHITE, COLOR_BRIGHT_CYAN);
    printf("║                                                              ║\n");
    printf("╠══════════════════════════════════════════════════════════════╣\n");
    printf("║                    SECURE PASSWORD GENERATOR                 ║\n");
    printf("║                     Version %d.%d.%d                           ║\n",
           VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
    printf("╚══════════════════════════════════════════════════════════════╝\n");
    printf("%s", COLOR_RESET);
    
    printf("\n%s🔐 Generating cryptographically secure passwords%s\n\n", 
           COLOR_BRIGHT_YELLOW, COLOR_RESET);
}

/**
 * @brief Print main menu
 */
int print_main_menu(void) {
    printf("%s╔════════════════════════════════════════╗\n", COLOR_BRIGHT_BLUE);
    printf("║            MAIN MENU                  ║\n");
    printf("╠════════════════════════════════════════╣\n", COLOR_BRIGHT_BLUE);
    printf("║                                        ║\n");
    printf("║  %s1.%s Generate Password               %s║\n", 
           COLOR_BRIGHT_GREEN, COLOR_WHITE, COLOR_BRIGHT_BLUE);
    printf("║  %s2.%s Generate Multiple Passwords    %s║\n", 
           COLOR_BRIGHT_GREEN, COLOR_WHITE, COLOR_BRIGHT_BLUE);
    printf("║  %s3.%s Configure Options              %s║\n", 
           COLOR_BRIGHT_GREEN, COLOR_WHITE, COLOR_BRIGHT_BLUE);
    printf("║  %s4.%s Check Password Strength        %s║\n", 
           COLOR_BRIGHT_GREEN, COLOR_WHITE, COLOR_BRIGHT_BLUE);
    printf("║  %s5.%s Save to File                   %s║\n", 
           COLOR_BRIGHT_GREEN, COLOR_WHITE, COLOR_BRIGHT_BLUE);
    printf("║  %s6.%s Copy to Clipboard              %s║\n", 
           COLOR_BRIGHT_GREEN, COLOR_WHITE, COLOR_BRIGHT_BLUE);
    printf("║  %s7.%s Show Help                      %s║\n", 
           COLOR_BRIGHT_GREEN, COLOR_WHITE, COLOR_BRIGHT_BLUE);
    printf("║  %s8.%s Exit                           %s║\n", 
           COLOR_BRIGHT_GREEN, COLOR_WHITE, COLOR_BRIGHT_BLUE);
    printf("║                                        ║\n");
    printf("╚════════════════════════════════════════╝%s\n\n", COLOR_RESET);
    
    printf("%sEnter choice [1-8]: %s", COLOR_BRIGHT_WHITE, COLOR_CYAN);
    
    int choice;
    char input[32];
    
    if (fgets(input, sizeof(input), stdin)) {
        trim_whitespace(input);
        if (!string_to_int(input, &choice, 1, 8)) {
            choice = 0;
        }
    } else {
        choice = 0;
    }
    
    printf("%s", COLOR_RESET);
    return choice;
}

/**
 * @brief Print password generation options menu
 */
void print_options_menu(const PasswordOptions *current_options) {
    printf("\n%s╔════════════════════════════════════════╗\n", COLOR_BRIGHT_BLUE);
    printf("║        PASSWORD OPTIONS              ║\n");
    printf("╠════════════════════════════════════════╣%s\n", COLOR_RESET);
    
    printf("%s  Current Settings:%s\n", COLOR_BRIGHT_YELLOW, COLOR_RESET);
    printf("  Length: %s%zu%s characters\n", 
           COLOR_CYAN, current_options->length, COLOR_RESET);
    printf("  Character Sets: ");
    
    if (current_options->charset.lowercase) 
        printf("%sa-z%s ", COLOR_GREEN, COLOR_RESET);
    if (current_options->charset.uppercase) 
        printf("%sA-Z%s ", COLOR_GREEN, COLOR_RESET);
    if (current_options->charset.numbers) 
        printf("%s0-9%s ", COLOR_GREEN, COLOR_RESET);
    if (current_options->charset.special) 
        printf("%s!@#$%%^&*%s ", COLOR_GREEN, COLOR_RESET);
    printf("\n");
    
    if (current_options->charset.avoid_ambiguous)
        printf("  Avoid ambiguous: %sEnabled%s\n", COLOR_GREEN, COLOR_RESET);
    
    printf("\n%s  Configuration Options:%s\n", COLOR_BRIGHT_YELLOW, COLOR_RESET);
    printf("  %s1.%s Change Length (%zu chars)          %s\n", 
           COLOR_BRIGHT_GREEN, COLOR_WHITE, current_options->length, COLOR_RESET);
    printf("  %s2.%s Toggle Lowercase (%s)             %s\n", 
           COLOR_BRIGHT_GREEN, COLOR_WHITE, 
           current_options->charset.lowercase ? "ON" : "OFF", COLOR_RESET);
    printf("  %s3.%s Toggle Uppercase (%s)             %s\n", 
           COLOR_BRIGHT_GREEN, COLOR_WHITE, 
           current_options->charset.uppercase ? "ON" : "OFF", COLOR_RESET);
    printf("  %s4.%s Toggle Numbers (%s)               %s\n", 
           COLOR_BRIGHT_GREEN, COLOR_WHITE, 
           current_options->charset.numbers ? "ON" : "OFF", COLOR_RESET);
    printf("  %s5.%s Toggle Special Chars (%s)         %s\n", 
           COLOR_BRIGHT_GREEN, COLOR_WHITE, 
           current_options->charset.special ? "ON" : "OFF", COLOR_RESET);
    printf("  %s6.%s Toggle Avoid Ambiguous (%s)       %s\n", 
           COLOR_BRIGHT_GREEN, COLOR_WHITE, 
           current_options->charset.avoid_ambiguous ? "ON" : "OFF", COLOR_RESET);
    printf("  %s7.%s Toggle Require All Types (%s)     %s\n", 
           COLOR_BRIGHT_GREEN, COLOR_WHITE, 
           current_options->require_all_types ? "ON" : "OFF", COLOR_RESET);
    printf("  %s8.%s Set Minimum Numbers (%zu)         %s\n", 
           COLOR_BRIGHT_GREEN, COLOR_WHITE, 
           current_options->min_numbers, COLOR_RESET);
    printf("  %s9.%s Set Minimum Special (%zu)         %s\n", 
           COLOR_BRIGHT_GREEN, COLOR_WHITE, 
           current_options->min_special, COLOR_RESET);
    printf("  %s0.%s Back to Main Menu                %s\n", 
           COLOR_BRIGHT_GREEN, COLOR_WHITE, COLOR_RESET);
    
    printf("%s╚════════════════════════════════════════╝%s\n", 
           COLOR_BRIGHT_BLUE, COLOR_RESET);
    
    printf("\n%sEnter choice [0-9]: %s", COLOR_BRIGHT_WHITE, COLOR_CYAN);
}

/**
 * @brief Display generated password with styling
 */
void display_password_result(const PasswordResult *result, const UIConfig *config) {
    if (!result || !result->password) {
        print_error("No password generated!");
        return;
    }
    
    SecurityAssessment assessment = assess_password_security(result->password, result->length);
    const char *color = get_strength_color(assessment.category);
    
    printf("\n");
    print_separator(config->terminal_width, '═');
    
    printf("%s🔐 GENERATED PASSWORD%s\n\n", COLOR_BRIGHT_CYAN, COLOR_RESET);
    
    /* Display password in a box */
    printf("%s┌", COLOR_BRIGHT_BLUE);
    for (size_t i = 0; i < result->length + 4; i++) printf("─");
    printf("┐%s\n", COLOR_RESET);
    
    printf("%s│  %s%s%s  │%s\n", 
           COLOR_BRIGHT_BLUE, 
           color, result->password, 
           COLOR_BRIGHT_BLUE,
           COLOR_RESET);
    
    printf("%s└", COLOR_BRIGHT_BLUE);
    for (size_t i = 0; i < result->length + 4; i++) printf("─");
    printf("┘%s\n", COLOR_RESET);
    
    printf("\n%s📊 Password Details:%s\n", COLOR_BRIGHT_YELLOW, COLOR_RESET);
    printf("  Length: %s%zu%s characters\n", 
           COLOR_CYAN, result->length, COLOR_RESET);
    printf("  Entropy: %s%.1f bits%s\n", 
           COLOR_CYAN, result->entropy, COLOR_RESET);
    printf("  Strength: %s%s%s (%d/100)\n", 
           color, result->strength, COLOR_RESET, result->strength_score);
    
    /* Additional security info */
    if (config->detailed_output) {
        printf("\n%s🔒 Security Assessment:%s\n", COLOR_BRIGHT_YELLOW, COLOR_RESET);
        print_security_assessment(&assessment);
    }
    
    print_separator(config->terminal_width, '═');
    printf("\n");
}

/**
 * @brief Display multiple password results
 */
void display_bulk_results(const PasswordResult *results, size_t count, const UIConfig *config) {
    if (!results || count == 0) {
        print_error("No passwords to display!");
        return;
    }
    
    printf("\n");
    print_separator(config->terminal_width, '═');
    printf("%s📦 GENERATED %zu PASSWORDS%s\n\n", 
           COLOR_BRIGHT_CYAN, count, COLOR_RESET);
    
    for (size_t i = 0; i < count; i++) {
        const PasswordResult *result = &results[i];
        SecurityAssessment assessment = assess_password_security(result->password, result->length);
        const char *color = get_strength_color(assessment.category);
        
        printf("%s[%03zu]%s ", COLOR_BRIGHT_BLUE, i + 1, COLOR_RESET);
        printf("%s%s%s ", color, result->password, COLOR_RESET);
        printf("(%s%zu chars%s, ", COLOR_CYAN, result->length, COLOR_RESET);
        printf("%s%.1f bits%s)\n", COLOR_MAGENTA, result->entropy, COLOR_RESET);
    }
    
    /* Calculate averages */
    double avg_entropy = 0.0;
    int avg_strength = 0;
    
    for (size_t i = 0; i < count; i++) {
        avg_entropy += results[i].entropy;
        avg_strength += results[i].strength_score;
    }
    
    avg_entropy /= count;
    avg_strength /= count;
    
    printf("\n%s📈 Summary:%s\n", COLOR_BRIGHT_YELLOW, COLOR_RESET);
    printf("  Average Entropy: %s%.1f bits%s\n", 
           COLOR_CYAN, avg_entropy, COLOR_RESET);
    printf("  Average Strength: %s%d/100%s\n", 
           get_strength_color((StrengthCategory)(avg_strength / 20)), 
           avg_strength, COLOR_RESET);
    
    print_separator(config->terminal_width, '═');
    printf("\n");
}

/**
 * @brief Print colored text
 */
void print_colored(const char *text, TerminalColor color, bool bold) {
    if (!text) return;
    
    const char *color_code = "";
    const char *bold_code = bold ? BOLD_ON : "";
    
    switch (color) {
        case COLOR_BLACK: color_code = COLOR_BLACK; break;
        case COLOR_RED: color_code = COLOR_RED; break;
        case COLOR_GREEN: color_code = COLOR_GREEN; break;
        case COLOR_YELLOW: color_code = COLOR_YELLOW; break;
        case COLOR_BLUE: color_code = COLOR_BLUE; break;
        case COLOR_MAGENTA: color_code = COLOR_MAGENTA; break;
        case COLOR_CYAN: color_code = COLOR_CYAN; break;
        case COLOR_WHITE: color_code = COLOR_WHITE; break;
        case COLOR_BRIGHT_BLACK: color_code = COLOR_BRIGHT_BLACK; break;
        case COLOR_BRIGHT_RED: color_code = COLOR_BRIGHT_RED; break;
        case COLOR_BRIGHT_GREEN: color_code = COLOR_BRIGHT_GREEN; break;
        case COLOR_BRIGHT_YELLOW: color_code = COLOR_BRIGHT_YELLOW; break;
        case COLOR_BRIGHT_BLUE: color_code = COLOR_BRIGHT_BLUE; break;
        case COLOR_BRIGHT_MAGENTA: color_code = COLOR_BRIGHT_MAGENTA; break;
        case COLOR_BRIGHT_CYAN: color_code = COLOR_BRIGHT_CYAN; break;
        case COLOR_BRIGHT_WHITE: color_code = COLOR_BRIGHT_WHITE; break;
        case COLOR_RESET: color_code = COLOR_RESET; break;
    }
    
    printf("%s%s%s%s", color_code, bold_code, text, COLOR_RESET);
}

/**
 * @brief Print success message
 */
void print_success(const char *message) {
    printf("%s✅ %s%s\n", COLOR_BRIGHT_GREEN, message, COLOR_RESET);
}

/**
 * @brief Print error message
 */
void print_error(const char *message) {
    fprintf(stderr, "%s❌ %s%s\n", COLOR_BRIGHT_RED, message, COLOR_RESET);
}

/**
 * @brief Print warning message
 */
void print_warning(const char *message) {
    printf("%s⚠️  %s%s\n", COLOR_BRIGHT_YELLOW, message, COLOR_RESET);
}

/**
 * @brief Print info message
 */
void print_info(const char *message) {
    printf("%sℹ️  %s%s\n", COLOR_BRIGHT_CYAN, message, COLOR_RESET);
}

/**
 * @brief Clear screen (platform-specific)
 */
void clear_screen(void) {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

/**
 * @brief Print separator line
 */
void print_separator(int width, char character) {
    if (width <= 0) {
        width = global_ui_config.terminal_width;
    }
    
    if (width > 200) width = 200;
    
    printf("%s", COLOR_BRIGHT_BLACK);
    for (int i = 0; i < width; i++) {
        putchar(character);
    }
    printf("%s\n", COLOR_RESET);
}

/**
 * @brief Print progress bar
 */
void print_progress_bar(int progress, int width) {
    if (progress < 0) progress = 0;
    if (progress > 100) progress = 100;
    if (width <= 0) width = PROGRESS_BAR_WIDTH;
    
    int filled = (progress * width) / 100;
    
    printf("%s[", COLOR_BRIGHT_BLUE);
    
    /* Filled portion */
    printf("%s", COLOR_BRIGHT_GREEN);
    for (int i = 0; i < filled; i++) {
        printf("█");
    }
    
    /* Remaining portion */
    printf("%s", COLOR_BRIGHT_BLACK);
    for (int i = filled; i < width; i++) {
        printf("░");
    }
    
    printf("%s] %3d%%%s", COLOR_BRIGHT_BLUE, progress, COLOR_RESET);
}

/**
 * @brief Show password strength meter
 */
void show_strength_meter(int score) {
    printf("\n%sStrength Meter:%s\n", COLOR_BRIGHT_YELLOW, COLOR_RESET);
    
    const char *labels[] = {"Very Weak", "Weak", "Fair", "Good", "Strong", "Very Strong"};
    const char *colors[] = {
        COLOR_BRIGHT_RED, COLOR_RED, COLOR_YELLOW, 
        COLOR_GREEN, COLOR_BRIGHT_GREEN, COLOR_BRIGHT_CYAN
    };
    
    int segments = 20;  /* 5 points per segment */
    int filled_segments = (score * segments) / 100;
    
    printf("[");
    for (int i = 0; i < segments; i++) {
        if (i < filled_segments) {
            int color_index = (i * 6) / segments;
            if (color_index >= 6) color_index = 5;
            printf("%s█%s", colors[color_index], COLOR_RESET);
        } else {
            printf("%s░%s", COLOR_BRIGHT_BLACK, COLOR_RESET);
        }
    }
    printf("] ");
    
    /* Show label */
    int label_index = score / 20;
    if (label_index >= 6) label_index = 5;
    if (label_index < 0) label_index = 0;
    
    printf("%s%s%s\n", colors[label_index], labels[label_index], COLOR_RESET);
}

/**
 * @brief Show password with strength visualization
 */
void show_password_with_strength(const char *password, int score) {
    if (!password) return;
    
    size_t len = strlen(password);
    int segments = 20;
    int filled_segments = (score * segments) / 100;
    
    printf("\n%sPassword: %s", COLOR_BRIGHT_WHITE, COLOR_RESET);
    
    /* Color password characters based on position in strength meter */
    for (size_t i = 0; i < len; i++) {
        int char_score_position = (i * segments) / len;
        
        if (char_score_position < filled_segments) {
            int color_index = (char_score_position * 6) / segments;
            if (color_index >= 6) color_index = 5;
            
            const char *colors[] = {
                COLOR_BRIGHT_RED, COLOR_RED, COLOR_YELLOW, 
                COLOR_GREEN, COLOR_BRIGHT_GREEN, COLOR_BRIGHT_CYAN
            };
            
            printf("%s%c%s", colors[color_index], password[i], COLOR_RESET);
        } else {
            printf("%s%c%s", COLOR_BRIGHT_BLACK, password[i], COLOR_RESET);
        }
    }
    
    printf("\n");
    show_strength_meter(score);
}

/**
 * @brief Get user input with prompt
 */
bool get_user_input(const char *prompt, char *buffer, size_t buffer_size) {
    if (!prompt || !buffer || buffer_size == 0) {
        return false;
    }
    
    printf("%s%s: %s", COLOR_BRIGHT_WHITE, prompt, COLOR_CYAN);
    
    if (fgets(buffer, (int)buffer_size, stdin) == NULL) {
        printf("%s", COLOR_RESET);
        return false;
    }
    
    printf("%s", COLOR_RESET);
    
    /* Remove newline */
    buffer[strcspn(buffer, "\n")] = '\0';
    trim_whitespace(buffer);
    
    return strlen(buffer) > 0;
}

/**
 * @brief Show loading animation
 */
void show_loading(const char *message, int duration_ms) {
    if (!message) return;
    
    printf("%s%s ", COLOR_BRIGHT_YELLOW, message);
    fflush(stdout);
    
    const char *spinner[] = {"⣾", "⣽", "⣻", "⢿", "⡿", "⣟", "⣯", "⣷"};
    int spinner_count = sizeof(spinner) / sizeof(spinner[0]);
    int start_time = (int)clock();
    int elapsed = 0;
    int frame = 0;
    
    while (elapsed < duration_ms) {
        printf("\b%s", spinner[frame % spinner_count]);
        fflush(stdout);
        
        sleep_ms(100);
        
        frame++;
        elapsed = (int)((clock() - start_time) * 1000 / CLOCKS_PER_SEC);
    }
    
    printf("\b \b\n%s", COLOR_RESET);
}

/**
 * @brief Confirm action with user
 */
bool confirm_action(const char *action) {
    if (!action) return false;
    
    printf("%s%s? [y/N]: %s", 
           COLOR_BRIGHT_YELLOW, action, COLOR_CYAN);
    
    char input[32];
    if (fgets(input, sizeof(input), stdin)) {
        trim_whitespace(input);
        printf("%s", COLOR_RESET);
        
        if (strlen(input) > 0 && 
            (tolower((unsigned char)input[0]) == 'y')) {
            return true;
        }
    }
    
    printf("%s", COLOR_RESET);
    return false;
}

/**
 * @brief Display ASCII art for password strength
 */
void display_strength_art(StrengthCategory category) {
    const char *art[] = {
        /* Very Weak */
        "    ⚠️  DANGER ZONE ⚠️\n"
        "    ┌──────────────┐\n"
        "    │   TOO WEAK   │\n"
        "    │  ┌──────┐    │\n"
        "    │  │      │    │\n"
        "    │  │      │    │\n"
        "    │  └──────┘    │\n"
        "    └──────────────┘",
        
        /* Weak */
        "    ⚠️  NEEDS WORK ⚠️\n"
        "    ┌──────────────┐\n"
        "    │    WEAK      │\n"
        "    │  ┌──────┐    │\n"
        "    │  │████  │    │\n"
        "    │  │      │    │\n"
        "    │  └──────┘    │\n"
        "    └──────────────┘",
        
        /* Fair */
        "    ⚠️  GETTING BETTER ⚠️\n"
        "    ┌──────────────┐\n"
        "    │    FAIR      │\n"
        "    │  ┌──────┐    │\n"
        "    │  │██████│    │\n"
        "    │  │      │    │\n"
        "    │  └──────┘    │\n"
        "    └──────────────┘",
        
        /* Good */
        "    ✅ GOOD PASSWORD ✅\n"
        "    ┌──────────────┐\n"
        "    │    GOOD      │\n"
        "    │  ┌──────┐    │\n"
        "    │  │██████│    │\n"
        "    │  │████  │    │\n"
        "    │  └──────┘    │\n"
        "    └──────────────┘",
        
        /* Strong */
        "    ✅ STRONG PASSWORD ✅\n"
        "    ┌──────────────┐\n"
        "    │   STRONG     │\n"
        "    │  ┌──────┐    │\n"
        "    │  │██████│    │\n"
        "    │  │██████│    │\n"
        "    │  └──────┘    │\n"
        "    └──────────────┘",
        
        /* Very Strong */
        "    🔥 VERY STRONG 🔥\n"
        "    ┌──────────────┐\n"
        "    │ VERY STRONG  │\n"
        "    │  ╔══════╗    │\n"
        "    │  ║██████║    │\n"
        "    │  ║██████║    │\n"
        "    │  ╚══════╝    │\n"
        "    └──────────────┘"
    };
    
    if (category >= STRENGTH_VERY_WEAK && category <= STRENGTH_VERY_STRONG) {
        const char *color = get_strength_color(category);
        printf("%s\n%s\n%s\n", color, art[category], COLOR_RESET);
    }
}