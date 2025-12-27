/**
 * @file security.c
 * @brief Password security assessment functions implementation
 * @version 1.0
 * @date 2024
 */

#include "security.h"
#include "config.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

/* Weak patterns to check against */
static const WeakPattern weak_patterns[] = {
    {"123", "Sequential numbers"},
    {"abc", "Sequential letters"},
    {"qwerty", "Keyboard pattern"},
    {"password", "Common word"},
    {"admin", "Common word"},
    {"letmein", "Common phrase"},
    {"welcome", "Common word"},
    {"monkey", "Common word"},
    {"dragon", "Common word"},
    {"baseball", "Common word"},
    {"football", "Common word"},
    {"mustang", "Common word"},
    {"master", "Common word"},
    {"hello", "Common word"},
    {"secret", "Common word"},
    {"asdf", "Keyboard pattern"},
    {"zxcv", "Keyboard pattern"},
    {"111", "Repeated numbers"},
    {"aaa", "Repeated letters"},
    {"000", "Repeated numbers"},
    {NULL, NULL}  /* Terminator */
};

/* Dictionary words (common passwords) */
static const char *dictionary_words[] = {
    "password", "123456", "12345678", "1234", "qwerty",
    "12345", "dragon", "pussy", "baseball", "football",
    "letmein", "monkey", "696969", "abc123", "mustang",
    "michael", "shadow", "master", "jennifer", "111111",
    "2000", "jordan", "superman", "harley", "1234567",
    "fuckme", "hunter", "fuckyou", "trustno1", "ranger",
    "buster", "thomas", "tigger", "robert", "soccer",
    "fuck", "batman", "test", "pass", "killer",
    "hockey", "george", "charlie", "andrew", "michelle",
    "love", "sunshine", "jessica", "pepper", "daniel",
    "access", "123456789", "654321", "joshua", "maggie",
    "starwars", "silver", "william", "dallas", "yankees",
    "123123", "ashley", "666666", "hello", "amanda",
    "orange", "biteme", "freedom", "computer", "sexy",
    "thunder", "nicole", "ginger", "heather", "hammer",
    "summer", "corvette", "taylor", "fucker", "austin",
    "1111", "merlin", "matthew", "121212", "golfer",
    "cheese", "princess", "martin", "chelsea", "patrick",
    "richard", "diamond", "yellow", "bigdog", "secret",
    "asdfgh", "sparky", "cowboy", NULL  /* Terminator */
};

/**
 * @brief Initialize security assessment with default values
 */
SecurityAssessment security_assessment_init(void) {
    SecurityAssessment assessment;
    
    assessment.score = 0;
    assessment.category = STRENGTH_VERY_WEAK;
    assessment.entropy = 0.0;
    assessment.crack_time_seconds = 0.0;
    assessment.has_weak_pattern = false;
    assessment.has_dictionary_word = false;
    assessment.is_duplicate = false;
    
    return assessment;
}

/**
 * @brief Assess password strength comprehensively
 */
SecurityAssessment assess_password_security(const char *password, size_t length) {
    SecurityAssessment assessment = security_assessment_init();
    
    if (!password || length == 0) {
        return assessment;
    }
    
    /* Calculate basic metrics */
    assessment.score = calculate_strength_score(password, length);
    assessment.category = (StrengthCategory)(assessment.score / 20);
    if (assessment.category > STRENGTH_VERY_STRONG) {
        assessment.category = STRENGTH_VERY_STRONG;
    }
    
    /* Calculate entropy (simplified) */
    assessment.entropy = calculate_simple_entropy(password, length);
    
    /* Check for issues */
    assessment.has_weak_pattern = check_weak_patterns(password);
    assessment.has_dictionary_word = check_dictionary_words(password);
    
    /* Adjust score based on issues found */
    if (assessment.has_weak_pattern) {
        assessment.score = assessment.score * 70 / 100;  /* Reduce by 30% */
    }
    
    if (assessment.has_dictionary_word) {
        assessment.score = assessment.score * 60 / 100;  /* Reduce by 40% */
    }
    
    if (assessment.score < 0) assessment.score = 0;
    if (assessment.score > 100) assessment.score = 100;
    
    /* Update category based on adjusted score */
    assessment.category = (StrengthCategory)(assessment.score / 20);
    if (assessment.category > STRENGTH_VERY_STRONG) {
        assessment.category = STRENGTH_VERY_STRONG;
    }
    
    /* Estimate crack time */
    assessment.crack_time_seconds = estimate_crack_time(assessment.entropy, 
                                                       GPU_GUESSES_PER_SECOND);
    
    return assessment;
}

/**
 * @brief Calculate strength score (0-100)
 */
int calculate_strength_score(const char *password, size_t length) {
    if (!password || length < MIN_PASSWORD_LENGTH) {
        return 0;
    }
    
    int score = 0;
    
    /* Length score (max 40 points) */
    if (length >= 12) score += 40;
    else if (length >= 10) score += 30;
    else if (length >= 8) score += 20;
    else score += 10;
    
    /* Character variety (max 40 points) */
    bool has_lower = false;
    bool has_upper = false;
    bool has_digit = false;
    bool has_special = false;
    
    for (size_t i = 0; i < length; i++) {
        char c = password[i];
        
        if (islower((unsigned char)c)) has_lower = true;
        else if (isupper((unsigned char)c)) has_upper = true;
        else if (isdigit((unsigned char)c)) has_digit = true;
        else has_special = true;
    }
    
    int variety_count = (has_lower ? 1 : 0) + (has_upper ? 1 : 0) +
                       (has_digit ? 1 : 0) + (has_special ? 1 : 0);
    
    switch (variety_count) {
        case 4: score += 40; break;
        case 3: score += 30; break;
        case 2: score += 20; break;
        case 1: score += 10; break;
        default: break;
    }
    
    /* Middle numbers/symbols (10 points) */
    for (size_t i = 1; i < length - 1; i++) {
        if (!isalpha((unsigned char)password[i])) {
            score += 10;
            break;
        }
    }
    
    /* Requirements (10 points) */
    if (length >= 8 && has_lower && has_upper && has_digit) {
        score += 10;
    }
    
    /* Cap at 100 */
    if (score > 100) score = 100;
    
    return score;
}

/**
 * @brief Check for weak patterns in password
 */
bool check_weak_patterns(const char *password) {
    if (!password) {
        return false;
    }
    
    size_t pass_len = strlen(password);
    
    /* Check against known weak patterns */
    for (int i = 0; weak_patterns[i].pattern != NULL; i++) {
        if (strstr(password, weak_patterns[i].pattern) != NULL) {
            return true;
        }
    }
    
    /* Check for sequential characters */
    for (size_t i = 0; i < pass_len - 2; i++) {
        char c1 = password[i];
        char c2 = password[i + 1];
        char c3 = password[i + 2];
        
        /* Check numeric sequences */
        if (isdigit((unsigned char)c1) && isdigit((unsigned char)c2) && isdigit((unsigned char)c3)) {
            if (c1 + 1 == c2 && c2 + 1 == c3) {
                return true;  /* e.g., 123, 456 */
            }
            if (c1 - 1 == c2 && c2 - 1 == c3) {
                return true;  /* e.g., 321, 654 */
            }
        }
        
        /* Check alphabetical sequences */
        if (isalpha((unsigned char)c1) && isalpha((unsigned char)c2) && isalpha((unsigned char)c3)) {
            if (tolower((unsigned char)c1) + 1 == tolower((unsigned char)c2) &&
                tolower((unsigned char)c2) + 1 == tolower((unsigned char)c3)) {
                return true;  /* e.g., abc, xyz */
            }
            if (tolower((unsigned char)c1) - 1 == tolower((unsigned char)c2) &&
                tolower((unsigned char)c2) - 1 == tolower((unsigned char)c3)) {
                return true;  /* e.g., cba, zyx */
            }
        }
    }
    
    /* Check for repeated characters */
    for (size_t i = 0; i < pass_len - 2; i++) {
        if (password[i] == password[i + 1] && 
            password[i] == password[i + 2]) {
            return true;  /* e.g., aaa, 111 */
        }
    }
    
    /* Check keyboard patterns (simple check) */
    const char *keyboard_rows[] = {
        "qwertyuiop",
        "asdfghjkl",
        "zxcvbnm",
        "1234567890",
        NULL
    };
    
    for (int r = 0; keyboard_rows[r] != NULL; r++) {
        const char *row = keyboard_rows[r];
        size_t row_len = strlen(row);
        
        for (size_t i = 0; i <= row_len - 3; i++) {
            char pattern[4] = {row[i], row[i + 1], row[i + 2], '\0'};
            if (strstr(password, pattern) != NULL) {
                return true;
            }
            
            /* Also check reverse */
            char reverse_pattern[4] = {row[i + 2], row[i + 1], row[i], '\0'};
            if (strstr(password, reverse_pattern) != NULL) {
                return true;
            }
        }
    }
    
    return false;
}

/**
 * @brief Check if password contains dictionary words
 */
bool check_dictionary_words(const char *password) {
    if (!password) {
        return false;
    }
    
    char lower_password[256];
    size_t pass_len = strlen(password);
    
    if (pass_len >= sizeof(lower_password)) {
        pass_len = sizeof(lower_password) - 1;
    }
    
    /* Convert to lowercase for comparison */
    for (size_t i = 0; i < pass_len; i++) {
        lower_password[i] = tolower((unsigned char)password[i]);
    }
    lower_password[pass_len] = '\0';
    
    /* Check against dictionary words */
    for (int i = 0; dictionary_words[i] != NULL; i++) {
        if (strstr(lower_password, dictionary_words[i]) != NULL) {
            return true;
        }
    }
    
    /* Check for leet speak substitutions */
    char *leet_password = strdup(lower_password);
    if (!leet_password) {
        return false;
    }
    
    /* Common leet speak substitutions */
    for (size_t i = 0; leet_password[i] != '\0'; i++) {
        switch (leet_password[i]) {
            case '4': leet_password[i] = 'a'; break;
            case '3': leet_password[i] = 'e'; break;
            case '0': leet_password[i] = 'o'; break;
            case '1': leet_password[i] = 'i'; break;
            case '5': leet_password[i] = 's'; break;
            case '7': leet_password[i] = 't'; break;
            case '@': leet_password[i] = 'a'; break;
            case '$': leet_password[i] = 's'; break;
            case '!': leet_password[i] = 'i'; break;
        }
    }
    
    /* Check modified password */
    for (int i = 0; dictionary_words[i] != NULL; i++) {
        if (strstr(leet_password, dictionary_words[i]) != NULL) {
            free(leet_password);
            return true;
        }
    }
    
    free(leet_password);
    return false;
}

/**
 * @brief Calculate simplified entropy for password
 */
double calculate_simple_entropy(const char *password, size_t length) {
    if (!password || length == 0) {
        return 0.0;
    }
    
    bool has_lower = false;
    bool has_upper = false;
    bool has_digit = false;
    bool has_special = false;
    
    /* Determine character set used */
    for (size_t i = 0; i < length; i++) {
        char c = password[i];
        
        if (islower((unsigned char)c)) has_lower = true;
        else if (isupper((unsigned char)c)) has_upper = true;
        else if (isdigit((unsigned char)c)) has_digit = true;
        else has_special = true;
    }
    
    /* Calculate pool size */
    size_t pool_size = 0;
    if (has_lower) pool_size += 26;
    if (has_upper) pool_size += 26;
    if (has_digit) pool_size += 10;
    if (has_special) pool_size += 32;  /* Approximate */
    
    if (pool_size == 0) {
        return 0.0;
    }
    
    /* Entropy = log2(pool_size^length) = length * log2(pool_size) */
    return (double)length * log2((double)pool_size);
}

/**
 * @brief Estimate time to crack password
 */
double estimate_crack_time(double entropy_bits, double guesses_per_second) {
    if (entropy_bits <= 0 || guesses_per_second <= 0) {
        return 0.0;
    }
    
    /* Number of possible combinations = 2^entropy */
    double combinations = pow(2.0, entropy_bits);
    
    /* Time in seconds = combinations / guesses_per_second */
    double seconds = combinations / guesses_per_second;
    
    return seconds;
}

/**
 * @brief Get color code for password strength
 */
const char *get_strength_color(StrengthCategory category) {
    switch (category) {
        case STRENGTH_VERY_WEAK:
            return COLOR_BRIGHT_RED;
        case STRENGTH_WEAK:
            return COLOR_RED;
        case STRENGTH_FAIR:
            return COLOR_YELLOW;
        case STRENGTH_GOOD:
            return COLOR_GREEN;
        case STRENGTH_STRONG:
            return COLOR_BRIGHT_GREEN;
        case STRENGTH_VERY_STRONG:
            return COLOR_BRIGHT_CYAN;
        default:
            return COLOR_RESET;
    }
}

/**
 * @brief Get string representation of strength category
 */
const char *get_strength_string(StrengthCategory category) {
    switch (category) {
        case STRENGTH_VERY_WEAK:
            return "Very Weak";
        case STRENGTH_WEAK:
            return "Weak";
        case STRENGTH_FAIR:
            return "Fair";
        case STRENGTH_GOOD:
            return "Good";
        case STRENGTH_STRONG:
            return "Strong";
        case STRENGTH_VERY_STRONG:
            return "Very Strong";
        default:
            return "Unknown";
    }
}

/**
 * @brief Print security assessment in a formatted way
 */
void print_security_assessment(const SecurityAssessment *assessment) {
    if (!assessment) {
        return;
    }
    
    const char *color = get_strength_color(assessment->category);
    const char *strength = get_strength_string(assessment->category);
    
    printf("\n%s╔════════════════════════════════════════╗\n", COLOR_BRIGHT_BLUE);
    printf("║        SECURITY ASSESSMENT         ║\n");
    printf("╠════════════════════════════════════════╣%s\n", COLOR_RESET);
    
    printf("%s  Strength: %s%-12s%s\n", 
           COLOR_BRIGHT_WHITE, color, strength, COLOR_RESET);
    
    printf("%s  Score:    %s%3d/100%s\n", 
           COLOR_BRIGHT_WHITE, 
           color, assessment->score, COLOR_RESET);
    
    printf("%s  Entropy:  %s%.1f bits%s\n", 
           COLOR_BRIGHT_WHITE, 
           COLOR_CYAN, assessment->entropy, COLOR_RESET);
    
    /* Format crack time */
    double seconds = assessment->crack_time_seconds;
    const char *time_unit = "seconds";
    double display_time = seconds;
    
    if (seconds > 31536000) {  /* More than 1 year */
        display_time = seconds / 31536000;
        time_unit = "years";
    } else if (seconds > 86400) {  /* More than 1 day */
        display_time = seconds / 86400;
        time_unit = "days";
    } else if (seconds > 3600) {  /* More than 1 hour */
        display_time = seconds / 3600;
        time_unit = "hours";
    } else if (seconds > 60) {  /* More than 1 minute */
        display_time = seconds / 60;
        time_unit = "minutes";
    }
    
    printf("%s  Crack Time: %s%.1f %s%s\n", 
           COLOR_BRIGHT_WHITE, 
           COLOR_MAGENTA, display_time, time_unit, COLOR_RESET);
    
    /* Issues */
    if (assessment->has_weak_pattern) {
        printf("%s  ⚠️  Contains weak patterns%s\n", 
               COLOR_BRIGHT_RED, COLOR_RESET);
    }
    
    if (assessment->has_dictionary_word) {
        printf("%s  ⚠️  Contains dictionary words%s\n", 
               COLOR_BRIGHT_RED, COLOR_RESET);
    }
    
    if (assessment->is_duplicate) {
        printf("%s  ⚠️  Duplicate password detected%s\n", 
               COLOR_BRIGHT_YELLOW, COLOR_RESET);
    }
    
    printf("%s╚════════════════════════════════════════╝%s\n\n", 
           COLOR_BRIGHT_BLUE, COLOR_RESET);
}

/**
 * @brief Check if two passwords are similar
 */
bool are_passwords_similar(const char *pass1, const char *pass2, double threshold) {
    if (!pass1 || !pass2) {
        return false;
    }
    
    size_t len1 = strlen(pass1);
    size_t len2 = strlen(pass2);
    
    if (len1 != len2) {
        return false;
    }
    
    size_t matches = 0;
    for (size_t i = 0; i < len1; i++) {
        if (pass1[i] == pass2[i]) {
            matches++;
        }
    }
    
    double similarity = (double)matches / len1;
    return similarity >= threshold;
}