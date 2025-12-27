/**
 * @file password.c
 * @brief Core password generation functions implementation
 * @version 1.0
 * @date 2024
 */

#include "password.h"
#include "utils.h"
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

/* Internal character sets */
static const char *lowercase_chars = CHARSET_LOWERCASE;
static const char *uppercase_chars = CHARSET_UPPERCASE;
static const char *number_chars = CHARSET_NUMBERS;
static const char *special_chars = CHARSET_SPECIAL;
static const char *ambiguous_chars = CHARSET_AMBIGUOUS;

/* Internal function declarations */
static char get_random_char_from_set(const char *char_set, size_t set_size);
static bool contains_char_type(const char *password, const char *char_set);
static size_t count_char_type(const char *password, const char *char_set);
static bool meets_minimum_requirements(const char *password, const PasswordOptions *options);
static void ensure_minimum_requirements(char *password, const PasswordOptions *options);

/**
 * @brief Initialize password generation options with default values
 */
PasswordOptions password_options_init(void) {
    PasswordOptions options;
    
    options.length = DEFAULT_PASSWORD_LENGTH;
    
    options.charset.lowercase = true;
    options.charset.uppercase = true;
    options.charset.numbers = true;
    options.charset.special = true;
    options.charset.avoid_ambiguous = false;
    
    options.require_all_types = true;
    options.min_numbers = 1;
    options.min_special = 1;
    
    return options;
}

/**
 * @brief Generate a single password based on options
 */
PasswordResult generate_password(const PasswordOptions *options) {
    PasswordResult result = {0};
    
    if (!options || !validate_options(options)) {
        result.strength = "Invalid options";
        return result;
    }
    
    /* Build character set based on options */
    SecureString char_set;
    if (!secure_string_init(&char_set, 256)) {
        result.strength = "Memory error";
        return result;
    }
    
    if (options->charset.lowercase) {
        secure_string_append_cstr(&char_set, lowercase_chars);
    }
    if (options->charset.uppercase) {
        secure_string_append_cstr(&char_set, uppercase_chars);
    }
    if (options->charset.numbers) {
        secure_string_append_cstr(&char_set, number_chars);
    }
    if (options->charset.special) {
        secure_string_append_cstr(&char_set, special_chars);
    }
    
    /* Remove ambiguous characters if requested */
    if (options->charset.avoid_ambiguous && char_set.length > 0) {
        SecureString filtered_set;
        if (!secure_string_init(&filtered_set, char_set.capacity)) {
            secure_string_free(&char_set);
            result.strength = "Memory error";
            return result;
        }
        
        for (size_t i = 0; i < char_set.length; i++) {
            if (strchr(ambiguous_chars, char_set.data[i]) == NULL) {
                secure_string_append(&filtered_set, char_set.data[i]);
            }
        }
        
        secure_string_free(&char_set);
        char_set = filtered_set;
    }
    
    /* Check if we have any characters to choose from */
    if (char_set.length == 0) {
        secure_string_free(&char_set);
        result.strength = "No character set selected";
        return result;
    }
    
    /* Allocate memory for password */
    result.password = (char *)calloc(options->length + 1, sizeof(char));
    if (!result.password) {
        secure_string_free(&char_set);
        result.strength = "Memory error";
        return result;
    }
    
    result.length = options->length;
    
    /* Generate password with secure random */
    for (size_t i = 0; i < options->length; i++) {
        unsigned char random_byte;
        if (!get_random_bytes(&random_byte, 1)) {
            /* Fallback to less secure random if needed */
            random_byte = (unsigned char)random_range(0, 255);
        }
        
        size_t index = random_byte % char_set.length;
        result.password[i] = char_set.data[index];
    }
    result.password[options->length] = '\0';
    
    /* Ensure minimum requirements are met */
    if (options->require_all_types || options->min_numbers > 0 || options->min_special > 0) {
        ensure_minimum_requirements(result.password, options);
    }
    
    /* Calculate metadata */
    result.entropy = calculate_entropy(result.password, options);
    result.strength_score = (int)((result.entropy / 128.0) * 100);
    if (result.strength_score > 100) result.strength_score = 100;
    if (result.strength_score < 0) result.strength_score = 0;
    
    result.strength = get_strength_category(result.strength_score);
    
    /* Cleanup */
    secure_string_free(&char_set);
    
    return result;
}

/**
 * @brief Generate multiple passwords in bulk
 */
size_t generate_bulk_passwords(const PasswordOptions *options, 
                              size_t count, 
                              PasswordResult *results) {
    if (!options || !results || count == 0 || count > MAX_BULK_GENERATE) {
        return 0;
    }
    
    if (!validate_options(options)) {
        return 0;
    }
    
    size_t successful = 0;
    
    for (size_t i = 0; i < count; i++) {
        results[i] = generate_password(options);
        
        if (results[i].password != NULL) {
            successful++;
        } else {
            /* Failed to generate this password */
            break;
        }
    }
    
    return successful;
}

/**
 * @brief Validate password options
 */
bool validate_options(const PasswordOptions *options) {
    if (!options) {
        return false;
    }
    
    /* Check password length */
    if (options->length < MIN_PASSWORD_LENGTH || 
        options->length > MAX_PASSWORD_LENGTH) {
        return false;
    }
    
    /* Check if at least one character set is selected */
    if (!options->charset.lowercase && !options->charset.uppercase &&
        !options->charset.numbers && !options->charset.special) {
        return false;
    }
    
    /* Check minimum requirements */
    if (options->require_all_types) {
        int selected_types = 0;
        if (options->charset.lowercase) selected_types++;
        if (options->charset.uppercase) selected_types++;
        if (options->charset.numbers) selected_types++;
        if (options->charset.special) selected_types++;
        
        if (options->length < (size_t)selected_types) {
            return false;
        }
    }
    
    /* Check if minimum numbers/special are possible */
    size_t max_possible_numbers = 0;
    size_t max_possible_special = 0;
    
    if (options->charset.numbers) {
        max_possible_numbers = options->length;
    }
    if (options->charset.special) {
        max_possible_special = options->length;
    }
    
    if (options->min_numbers > max_possible_numbers ||
        options->min_special > max_possible_special) {
        return false;
    }
    
    /* Check if combined minimums exceed password length */
    if ((options->min_numbers + options->min_special) > options->length) {
        return false;
    }
    
    return true;
}

/**
 * @brief Calculate password entropy
 */
double calculate_entropy(const char *password, const PasswordOptions *options) {
    if (!password || !options) {
        return 0.0;
    }
    
    size_t pool_size = 0;
    
    if (options->charset.lowercase) {
        pool_size += strlen(lowercase_chars);
        if (options->charset.avoid_ambiguous) {
            /* Subtract ambiguous lowercase letters */
            for (const char *c = ambiguous_chars; *c; c++) {
                if (strchr(lowercase_chars, *c)) {
                    pool_size--;
                }
            }
        }
    }
    
    if (options->charset.uppercase) {
        pool_size += strlen(uppercase_chars);
        if (options->charset.avoid_ambiguous) {
            /* Subtract ambiguous uppercase letters */
            for (const char *c = ambiguous_chars; *c; c++) {
                if (strchr(uppercase_chars, *c)) {
                    pool_size--;
                }
            }
        }
    }
    
    if (options->charset.numbers) {
        pool_size += strlen(number_chars);
        if (options->charset.avoid_ambiguous) {
            /* Subtract ambiguous numbers */
            for (const char *c = ambiguous_chars; *c; c++) {
                if (strchr(number_chars, *c)) {
                    pool_size--;
                }
            }
        }
    }
    
    if (options->charset.special) {
        pool_size += strlen(special_chars);
    }
    
    if (pool_size == 0) {
        return 0.0;
    }
    
    /* Entropy formula: log2(pool_size^length) = length * log2(pool_size) */
    double entropy_per_char = log2((double)pool_size);
    return (double)strlen(password) * entropy_per_char;
}

/**
 * @brief Get strength category based on score
 */
const char *get_strength_category(int score) {
    if (score < STRENGTH_THRESHOLD_VERY_WEAK) {
        return "Very Weak";
    } else if (score < STRENGTH_THRESHOLD_WEAK) {
        return "Weak";
    } else if (score < STRENGTH_THRESHOLD_FAIR) {
        return "Fair";
    } else if (score < STRENGTH_THRESHOLD_GOOD) {
        return "Good";
    } else if (score < STRENGTH_THRESHOLD_STRONG) {
        return "Strong";
    } else {
        return "Very Strong";
    }
}

/**
 * @brief Free memory allocated for PasswordResult
 */
void free_password_result(PasswordResult *result) {
    if (result) {
        if (result->password) {
            secure_clear(result->password, result->length);
            free(result->password);
            result->password = NULL;
        }
        result->length = 0;
        result->entropy = 0.0;
        result->strength_score = 0;
        result->strength = NULL;
    }
}

/**
 * @brief Free memory allocated for multiple PasswordResults
 */
void free_bulk_passwords(PasswordResult *results, size_t count) {
    if (results) {
        for (size_t i = 0; i < count; i++) {
            free_password_result(&results[i]);
        }
    }
}

/* Internal helper functions */

/**
 * @brief Get random character from character set
 */
static char get_random_char_from_set(const char *char_set, size_t set_size) {
    if (!char_set || set_size == 0) {
        return '\0';
    }
    
    unsigned char random_byte;
    if (!get_random_bytes(&random_byte, 1)) {
        random_byte = (unsigned char)random_range(0, 255);
    }
    
    size_t index = random_byte % set_size;
    return char_set[index];
}

/**
 * @brief Check if password contains characters from specific set
 */
static bool contains_char_type(const char *password, const char *char_set) {
    if (!password || !char_set) {
        return false;
    }
    
    for (size_t i = 0; password[i] != '\0'; i++) {
        if (strchr(char_set, password[i]) != NULL) {
            return true;
        }
    }
    
    return false;
}

/**
 * @brief Count characters from specific set in password
 */
static size_t count_char_type(const char *password, const char *char_set) {
    if (!password || !char_set) {
        return 0;
    }
    
    size_t count = 0;
    for (size_t i = 0; password[i] != '\0'; i++) {
        if (strchr(char_set, password[i]) != NULL) {
            count++;
        }
    }
    
    return count;
}

/**
 * @brief Check if password meets minimum requirements
 */
static bool meets_minimum_requirements(const char *password, const PasswordOptions *options) {
    if (!password || !options) {
        return false;
    }
    
    /* Check require all types */
    if (options->require_all_types) {
        if (options->charset.lowercase && 
            !contains_char_type(password, lowercase_chars)) {
            return false;
        }
        if (options->charset.uppercase && 
            !contains_char_type(password, uppercase_chars)) {
            return false;
        }
        if (options->charset.numbers && 
            !contains_char_type(password, number_chars)) {
            return false;
        }
        if (options->charset.special && 
            !contains_char_type(password, special_chars)) {
            return false;
        }
    }
    
    /* Check minimum numbers */
    if (options->min_numbers > 0) {
        size_t num_count = count_char_type(password, number_chars);
        if (num_count < options->min_numbers) {
            return false;
        }
    }
    
    /* Check minimum special characters */
    if (options->min_special > 0) {
        size_t special_count = count_char_type(password, special_chars);
        if (special_count < options->min_special) {
            return false;
        }
    }
    
    return true;
}

/**
 * @brief Ensure password meets minimum requirements by modifying it
 */
static void ensure_minimum_requirements(char *password, const PasswordOptions *options) {
    if (!password || !options) {
        return;
    }
    
    size_t length = strlen(password);
    if (length == 0) {
        return;
    }
    
    /* Array to track which positions we can modify */
    int *modifiable = (int *)calloc(length, sizeof(int));
    if (!modifiable) {
        return;
    }
    
    /* Initially, all positions are modifiable */
    for (size_t i = 0; i < length; i++) {
        modifiable[i] = 1;
    }
    
    /* Ensure required character types */
    if (options->require_all_types) {
        if (options->charset.lowercase && 
            !contains_char_type(password, lowercase_chars)) {
            /* Find a modifiable position and put a lowercase letter */
            for (size_t i = 0; i < length; i++) {
                if (modifiable[i]) {
                    password[i] = get_random_char_from_set(lowercase_chars, 
                                                          strlen(lowercase_chars));
                    modifiable[i] = 0;
                    break;
                }
            }
        }
        
        if (options->charset.uppercase && 
            !contains_char_type(password, uppercase_chars)) {
            for (size_t i = 0; i < length; i++) {
                if (modifiable[i]) {
                    password[i] = get_random_char_from_set(uppercase_chars, 
                                                          strlen(uppercase_chars));
                    modifiable[i] = 0;
                    break;
                }
            }
        }
        
        if (options->charset.numbers && 
            !contains_char_type(password, number_chars)) {
            for (size_t i = 0; i < length; i++) {
                if (modifiable[i]) {
                    password[i] = get_random_char_from_set(number_chars, 
                                                          strlen(number_chars));
                    modifiable[i] = 0;
                    break;
                }
            }
        }
        
        if (options->charset.special && 
            !contains_char_type(password, special_chars)) {
            for (size_t i = 0; i < length; i++) {
                if (modifiable[i]) {
                    password[i] = get_random_char_from_set(special_chars, 
                                                          strlen(special_chars));
                    modifiable[i] = 0;
                    break;
                }
            }
        }
    }
    
    /* Ensure minimum numbers */
    if (options->min_numbers > 0) {
        size_t num_count = count_char_type(password, number_chars);
        
        while (num_count < options->min_numbers) {
            for (size_t i = 0; i < length; i++) {
                if (modifiable[i]) {
                    password[i] = get_random_char_from_set(number_chars, 
                                                          strlen(number_chars));
                    modifiable[i] = 0;
                    num_count++;
                    break;
                }
            }
        }
    }
    
    /* Ensure minimum special characters */
    if (options->min_special > 0) {
        size_t special_count = count_char_type(password, special_chars);
        
        while (special_count < options->min_special) {
            for (size_t i = 0; i < length; i++) {
                if (modifiable[i]) {
                    password[i] = get_random_char_from_set(special_chars, 
                                                          strlen(special_chars));
                    modifiable[i] = 0;
                    special_count++;
                    break;
                }
            }
        }
    }
    
    free(modifiable);
}

/**
 * @brief Generate password from pattern
 * 
 * Pattern format: "llUnss" where:
 * l = lowercase, U = uppercase, n = number, s = special
 */
PasswordResult generate_password_from_pattern(const char *pattern) {
    PasswordResult result = {0};
    
    if (!pattern || strlen(pattern) == 0) {
        result.strength = "Invalid pattern";
        return result;
    }
    
    size_t length = strlen(pattern);
    result.password = (char *)calloc(length + 1, sizeof(char));
    if (!result.password) {
        result.strength = "Memory error";
        return result;
    }
    
    result.length = length;
    
    for (size_t i = 0; i < length; i++) {
        char pattern_char = pattern[i];
        char generated_char = '\0';
        
        switch (pattern_char) {
            case 'l':  /* lowercase */
                generated_char = get_random_char_from_set(lowercase_chars, 
                                                         strlen(lowercase_chars));
                break;
                
            case 'U':  /* uppercase */
                generated_char = get_random_char_from_set(uppercase_chars, 
                                                         strlen(uppercase_chars));
                break;
                
            case 'n':  /* number */
                generated_char = get_random_char_from_set(number_chars, 
                                                         strlen(number_chars));
                break;
                
            case 's':  /* special */
                generated_char = get_random_char_from_set(special_chars, 
                                                         strlen(special_chars));
                break;
                
            default:
                /* Invalid pattern character */
                free_password_result(&result);
                result.strength = "Invalid pattern character";
                return result;
        }
        
        if (generated_char == '\0') {
            free_password_result(&result);
            result.strength = "Failed to generate character";
            return result;
        }
        
        result.password[i] = generated_char;
    }
    
    result.password[length] = '\0';
    
    /* Calculate metadata */
    PasswordOptions options = password_options_init();
    options.length = length;
    
    /* Determine character sets from pattern */
    options.charset.lowercase = (strchr(pattern, 'l') != NULL);
    options.charset.uppercase = (strchr(pattern, 'U') != NULL);
    options.charset.numbers = (strchr(pattern, 'n') != NULL);
    options.charset.special = (strchr(pattern, 's') != NULL);
    
    result.entropy = calculate_entropy(result.password, &options);
    result.strength_score = (int)((result.entropy / 128.0) * 100);
    if (result.strength_score > 100) result.strength_score = 100;
    if (result.strength_score < 0) result.strength_score = 0;
    
    result.strength = get_strength_category(result.strength_score);
    
    return result;
}