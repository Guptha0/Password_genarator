/**
 * @file password.h
 * @brief Core password generation functions and structures
 * @version 1.0
 * @date 2024
 */

#ifndef PASSWORD_GENERATOR_H
#define PASSWORD_GENERATOR_H

#include <stdbool.h>
#include <stddef.h>

#define MIN_PASSWORD_LENGTH 8
#define MAX_PASSWORD_LENGTH 128
#define DEFAULT_PASSWORD_LENGTH 12
#define MAX_BULK_GENERATE 100

/**
 * @brief Character set configuration structure
 */
typedef struct {
    bool lowercase;     /**< Include lowercase letters (a-z) */
    bool uppercase;     /**< Include uppercase letters (A-Z) */
    bool numbers;       /**< Include numbers (0-9) */
    bool special;       /**< Include special characters (!@#$%^&*) */
    bool avoid_ambiguous; /**< Avoid ambiguous characters (l, I, 1, O, 0) */
} CharSetConfig;

/**
 * @brief Password generation options structure
 */
typedef struct {
    size_t length;              /**< Password length */
    CharSetConfig charset;      /**< Character set configuration */
    bool require_all_types;     /**< Require at least one of each selected type */
    size_t min_numbers;         /**< Minimum number of digits required */
    size_t min_special;         /**< Minimum number of special chars required */
} PasswordOptions;

/**
 * @brief Password with metadata structure
 */
typedef struct {
    char *password;         /**< Generated password string */
    size_t length;          /**< Password length */
    double entropy;         /**< Entropy in bits */
    int strength_score;     /**< Strength score (0-100) */
    const char *strength;   /**< Strength category */
} PasswordResult;

/**
 * @brief Initialize password generation options with default values
 * @return PasswordOptions with default settings
 */
PasswordOptions password_options_init(void);

/**
 * @brief Generate a single password based on options
 * @param options Password generation options
 * @return PasswordResult containing the generated password and metadata
 */
PasswordResult generate_password(const PasswordOptions *options);

/**
 * @brief Generate multiple passwords in bulk
 * @param options Password generation options
 * @param count Number of passwords to generate (1-100)
 * @param results Array to store generated passwords
 * @return Number of passwords successfully generated
 */
size_t generate_bulk_passwords(const PasswordOptions *options, 
                              size_t count, 
                              PasswordResult *results);

/**
 * @brief Validate password options
 * @param options Options to validate
 * @return true if options are valid, false otherwise
 */
bool validate_options(const PasswordOptions *options);

/**
 * @brief Calculate password entropy
 * @param password The password to analyze
 * @param options Generation options used
 * @return Entropy in bits
 */
double calculate_entropy(const char *password, const PasswordOptions *options);

/**
 * @brief Get strength category based on score
 * @param score Strength score (0-100)
 * @return String representation of strength
 */
const char *get_strength_category(int score);

/**
 * @brief Free memory allocated for PasswordResult
 * @param result PasswordResult to free
 */
void free_password_result(PasswordResult *result);

/**
 * @brief Free memory allocated for multiple PasswordResults
 * @param results Array of PasswordResults
 * @param count Number of results to free
 */
void free_bulk_passwords(PasswordResult *results, size_t count);

#endif /* PASSWORD_GENERATOR_H */