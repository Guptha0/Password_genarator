/**
 * @file security.h
 * @brief Password security assessment functions
 * @version 1.0
 * @date 2024
 */

#ifndef SECURITY_H
#define SECURITY_H

#include <stdbool.h>

/**
 * @brief Password strength categories
 */
typedef enum {
    STRENGTH_VERY_WEAK = 0,
    STRENGTH_WEAK,
    STRENGTH_FAIR,
    STRENGTH_GOOD,
    STRENGTH_STRONG,
    STRENGTH_VERY_STRONG
} StrengthCategory;

/**
 * @brief Security assessment result structure
 */
typedef struct {
    int score;                  /**< Strength score (0-100) */
    StrengthCategory category;  /**< Strength category */
    double entropy;             /**< Entropy in bits */
    double crack_time_seconds;  /**< Estimated time to crack */
    bool has_weak_pattern;      /**< Contains weak patterns */
    bool has_dictionary_word;   /**< Contains dictionary words */
    bool is_duplicate;          /**< Is duplicate of previous passwords */
} SecurityAssessment;

/**
 * @brief Common weak patterns to check against
 */
typedef struct {
    const char *pattern;
    const char *description;
} WeakPattern;

/**
 * @brief Initialize security assessment with default values
 * @return SecurityAssessment structure
 */
SecurityAssessment security_assessment_init(void);

/**
 * @brief Assess password strength comprehensively
 * @param password Password to assess
 * @param length Length of the password
 * @return Complete security assessment
 */
SecurityAssessment assess_password_security(const char *password, size_t length);

/**
 * @brief Calculate strength score (0-100)
 * @param password Password to score
 * @param length Length of the password
 * @return Strength score
 */
int calculate_strength_score(const char *password, size_t length);

/**
 * @brief Check for weak patterns in password
 * @param password Password to check
 * @return true if weak pattern found, false otherwise
 */
bool check_weak_patterns(const char *password);

/**
 * @brief Check if password contains dictionary words
 * @param password Password to check
 * @return true if dictionary word found, false otherwise
 */
bool check_dictionary_words(const char *password);

/**
 * @brief Estimate time to crack password
 * @param entropy_bits Entropy in bits
 * @param guesses_per_second Assumed attack speed (default: 1e9 for GPU)
 * @return Estimated time in seconds
 */
double estimate_crack_time(double entropy_bits, double guesses_per_second);

/**
 * @brief Get color code for password strength
 * @param category Strength category
 * @return ANSI color code string
 */
const char *get_strength_color(StrengthCategory category);

/**
 * @brief Get string representation of strength category
 * @param category Strength category
 * @return String representation
 */
const char *get_strength_string(StrengthCategory category);

/**
 * @brief Print security assessment in a formatted way
 * @param assessment Security assessment to print
 */
void print_security_assessment(const SecurityAssessment *assessment);

#endif /* SECURITY_H */