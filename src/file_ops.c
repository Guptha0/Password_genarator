/**
 * @file file_ops.c
 * @brief File operations for password saving
 * @version 1.0
 * @date 2024
 */

#include "password.h"
#include "utils.h"
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>

/**
 * @brief Save password to text file
 */
bool save_password_to_file(const PasswordResult *result, const char *filename, 
                          bool append, bool include_metadata) {
    if (!result || !result->password || !filename) {
        return false;
    }
    
    FILE *file = fopen(filename, append ? "a" : "w");
    if (!file) {
        fprintf(stderr, "Error opening file %s: %s\n", filename, strerror(errno));
        return false;
    }
    
    if (include_metadata) {
        char timestamp[64];
        get_timestamp(timestamp, sizeof(timestamp), NULL);
        
        fprintf(file, "=== Password Entry ===\n");
        fprintf(file, "Date: %s\n", timestamp);
        fprintf(file, "Password: %s\n", result->password);
        fprintf(file, "Length: %zu characters\n", result->length);
        fprintf(file, "Entropy: %.1f bits\n", result->entropy);
        fprintf(file, "Strength: %s (%d/100)\n", result->strength, result->strength_score);
        fprintf(file, "=====================\n\n");
    } else {
        fprintf(file, "%s\n", result->password);
    }
    
    fclose(file);
    return true;
}

/**
 * @brief Save multiple passwords to file
 */
bool save_bulk_passwords_to_file(const PasswordResult *results, size_t count,
                                const char *filename, bool include_metadata) {
    if (!results || count == 0 || !filename) {
        return false;
    }
    
    FILE *file = fopen(filename, "w");
    if (!file) {
        fprintf(stderr, "Error opening file %s: %s\n", filename, strerror(errno));
        return false;
    }
    
    char timestamp[64];
    get_timestamp(timestamp, sizeof(timestamp), NULL);
    
    fprintf(file, "=== Password List ===\n");
    fprintf(file, "Generated: %s\n", timestamp);
    fprintf(file, "Count: %zu passwords\n", count);
    fprintf(file, "=====================\n\n");
    
    for (size_t i = 0; i < count; i++) {
        if (include_metadata) {
            fprintf(file, "[%03zu] %s\n", i + 1, results[i].password);
            fprintf(file, "    Length: %zu, Entropy: %.1f bits, Strength: %s\n\n",
                   results[i].length, results[i].entropy, results[i].strength);
        } else {
            fprintf(file, "%s\n", results[i].password);
        }
    }
    
    fclose(file);
    return true;
}

/**
 * @brief Save passwords to CSV file
 */
bool save_passwords_to_csv(const PasswordResult *results, size_t count,
                          const char *filename) {
    if (!results || count == 0 || !filename) {
        return false;
    }
    
    FILE *file = fopen(filename, "w");
    if (!file) {
        fprintf(stderr, "Error opening file %s: %s\n", filename, strerror(errno));
        return false;
    }
    
    /* CSV header */
    fprintf(file, "Index,Timestamp,Password,Length,Entropy,Strength,StrengthScore\n");
    
    char timestamp[64];
    
    for (size_t i = 0; i < count; i++) {
        get_timestamp(timestamp, sizeof(timestamp), NULL);
        
        /* Escape password for CSV (handle commas and quotes) */
        char escaped_password[1024] = {0};
        size_t dest_index = 0;
        
        for (size_t j = 0; results[i].password[j] != '\0' && dest_index < sizeof(escaped_password) - 3; j++) {
            if (results[i].password[j] == '"') {
                escaped_password[dest_index++] = '"';
                escaped_password[dest_index++] = '"';
            } else if (results[i].password[j] == ',') {
                escaped_password[dest_index++] = '"';
                escaped_password[dest_index++] = ',';
                escaped_password[dest_index++] = '"';
            } else {
                escaped_password[dest_index++] = results[i].password[j];
            }
        }
        escaped_password[dest_index] = '\0';
        
        fprintf(file, "%zu,%s,\"%s\",%zu,%.1f,\"%s\",%d\n",
               i + 1, timestamp, escaped_password, 
               results[i].length, results[i].entropy,
               results[i].strength, results[i].strength_score);
    }
    
    fclose(file);
    return true;
}

/**
 * @brief Save passwords to JSON file
 */
bool save_passwords_to_json(const PasswordResult *results, size_t count,
                           const char *filename) {
    if (!results || count == 0 || !filename) {
        return false;
    }
    
    FILE *file = fopen(filename, "w");
    if (!file) {
        fprintf(stderr, "Error opening file %s: %s\n", filename, strerror(errno));
        return false;
    }
    
    char timestamp[64];
    get_timestamp(timestamp, sizeof(timestamp), NULL);
    
    fprintf(file, "{\n");
    fprintf(file, "  \"metadata\": {\n");
    fprintf(file, "    \"generated\": \"%s\",\n", timestamp);
    fprintf(file, "    \"count\": %zu,\n", count);
    fprintf(file, "    \"application\": \"SecurePassGen\"\n");
    fprintf(file, "  },\n");
    fprintf(file, "  \"passwords\": [\n");
    
    for (size_t i = 0; i < count; i++) {
        /* Escape password for JSON */
        char escaped_password[1024] = {0};
        size_t dest_index = 0;
        
        for (size_t j = 0; results[i].password[j] != '\0' && dest_index < sizeof(escaped_password) - 3; j++) {
            switch (results[i].password[j]) {
                case '"': 
                    escaped_password[dest_index++] = '\\';
                    escaped_password[dest_index++] = '"';
                    break;
                case '\\':
                    escaped_password[dest_index++] = '\\';
                    escaped_password[dest_index++] = '\\';
                    break;
                case '\b':
                    escaped_password[dest_index++] = '\\';
                    escaped_password[dest_index++] = 'b';
                    break;
                case '\f':
                    escaped_password[dest_index++] = '\\';
                    escaped_password[dest_index++] = 'f';
                    break;
                case '\n':
                    escaped_password[dest_index++] = '\\';
                    escaped_password[dest_index++] = 'n';
                    break;
                case '\r':
                    escaped_password[dest_index++] = '\\';
                    escaped_password[dest_index++] = 'r';
                    break;
                case '\t':
                    escaped_password[dest_index++] = '\\';
                    escaped_password[dest_index++] = 't';
                    break;
                default:
                    escaped_password[dest_index++] = results[i].password[j];
            }
        }
        escaped_password[dest_index] = '\0';
        
        fprintf(file, "    {\n");
        fprintf(file, "      \"index\": %zu,\n", i + 1);
        fprintf(file, "      \"password\": \"%s\",\n", escaped_password);
        fprintf(file, "      \"length\": %zu,\n", results[i].length);
        fprintf(file, "      \"entropy\": %.1f,\n", results[i].entropy);
        fprintf(file, "      \"strength\": \"%s\",\n", results[i].strength);
        fprintf(file, "      \"strengthScore\": %d\n", results[i].strength_score);
        
        if (i < count - 1) {
            fprintf(file, "    },\n");
        } else {
            fprintf(file, "    }\n");
        }
    }
    
    fprintf(file, "  ]\n");
    fprintf(file, "}\n");
    
    fclose(file);
    return true;
}

/**
 * @brief Load passwords from file
 */
PasswordResult *load_passwords_from_file(const char *filename, size_t *count) {
    if (!filename || !count) {
        return NULL;
    }
    
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error opening file %s: %s\n", filename, strerror(errno));
        return NULL;
    }
    
    /* First pass: count passwords */
    size_t password_count = 0;
    char buffer[1024];
    
    while (fgets(buffer, sizeof(buffer), file)) {
        trim_whitespace(buffer);
        if (strlen(buffer) > 0 && 
            !strstr(buffer, "===") && 
            !strstr(buffer, "Date:") &&
            !strstr(buffer, "Length:") &&
            !strstr(buffer, "Entropy:") &&
            !strstr(buffer, "Strength:") &&
            !strstr(buffer, "Index,") &&  /* CSV header */
            !strstr(buffer, "\"index\":")) {  /* JSON */
            password_count++;
        }
    }
    
    if (password_count == 0) {
        fclose(file);
        *count = 0;
        return NULL;
    }
    
    /* Allocate memory for results */
    PasswordResult *results = (PasswordResult *)calloc(password_count, sizeof(PasswordResult));
    if (!results) {
        fclose(file);
        *count = 0;
        return NULL;
    }
    
    /* Second pass: read passwords */
    rewind(file);
    size_t current = 0;
    
    while (fgets(buffer, sizeof(buffer), file) && current < password_count) {
        trim_whitespace(buffer);
        
        /* Skip metadata lines */
        if (strlen(buffer) == 0 || 
            strstr(buffer, "===") || 
            strstr(buffer, "Date:") ||
            strstr(buffer, "Length:") ||
            strstr(buffer, "Entropy:") ||
            strstr(buffer, "Strength:") ||
            strstr(buffer, "Index,") ||
            strstr(buffer, "\"index\":")) {
            continue;
        }
        
        /* Handle JSON password field */
        if (strstr(buffer, "\"password\":")) {
            char *start = strstr(buffer, "\"password\": \"");
            if (start) {
                start += 13; /* Skip "\"password\": \"" */
                char *end = strrchr(start, '"');
                if (end) {
                    *end = '\0';
                    strcpy(buffer, start);
                }
            }
        }
        
        /* Handle CSV format */
        else if (strchr(buffer, ',') && current == 0) {
            /* Skip CSV header */
            continue;
        }
        else if (strchr(buffer, ',')) {
            /* Extract password from CSV */
            char *comma = strchr(buffer, ',');
            if (comma) {
                char *password_start = strchr(comma + 1, ',');
                if (password_start) {
                    password_start++; /* Skip comma */
                    
                    /* Check if password is quoted */
                    if (*password_start == '"') {
                        password_start++;
                        char *password_end = strrchr(password_start, '"');
                        if (password_end) {
                            *password_end = '\0';
                            strcpy(buffer, password_start);
                        }
                    } else {
                        /* Password not quoted, find next comma */
                        char *next_comma = strchr(password_start, ',');
                        if (next_comma) {
                            *next_comma = '\0';
                            strcpy(buffer, password_start);
                        }
                    }
                }
            }
        }
        
        /* Store password */
        if (strlen(buffer) > 0) {
            results[current].password = strdup(buffer);
            results[current].length = strlen(buffer);
            results[current].entropy = 0.0; /* Will be calculated later if needed */
            results[current].strength_score = 0;
            results[current].strength = "Unknown";
            current++;
        }
    }
    
    fclose(file);
    *count = current;
    
    return results;
}

/**
 * @brief Securely delete file (overwrite multiple times)
 */
bool secure_delete_file(const char *filename, int passes) {
    if (!filename || passes <= 0) {
        return false;
    }
    
    FILE *file = fopen(filename, "rb+");
    if (!file) {
        /* File doesn't exist or can't be opened */
        return false;
    }
    
    /* Get file size */
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);
    
    if (file_size <= 0) {
        fclose(file);
        remove(filename);
        return true;
    }
    
    /* Create buffer for overwriting */
    unsigned char *buffer = (unsigned char *)malloc(file_size);
    if (!buffer) {
        fclose(file);
        return false;
    }
    
    /* Multiple overwrite passes */
    const unsigned char patterns[][2] = {
        {0x00, 0x00},  /* All zeros */
        {0xFF, 0xFF},  /* All ones */
        {0xAA, 0x55},  /* Alternating pattern 10101010 01010101 */
        {0x55, 0xAA},  /* Alternating pattern 01010101 10101010 */
        {0x92, 0x49},  /* Random pattern */
        {0x49, 0x24},  /* Another random pattern */
        {0x24, 0x92},  /* Another random pattern */
        {0x00, 0x00},  /* Final zeros */
    };
    
    int max_patterns = sizeof(patterns) / sizeof(patterns[0]);
    if (passes > max_patterns) passes = max_patterns;
    
    for (int pass = 0; pass < passes; pass++) {
        /* Fill buffer with pattern */
        for (long i = 0; i < file_size; i++) {
            buffer[i] = patterns[pass % max_patterns][i % 2];
        }
        
        /* Write pattern to file */
        rewind(file);
        size_t written = fwrite(buffer, 1, file_size, file);
        fflush(file);
        
        if (written != (size_t)file_size) {
            free(buffer);
            fclose(file);
            return false;
        }
    }
    
    /* Close and delete file */
    fclose(file);
    free(buffer);
    
    /* Remove the file */
    if (remove(filename) != 0) {
        return false;
    }
    
    return true;
}

/**
 * @brief Create backup of password file
 */
bool create_backup(const char *filename) {
    if (!filename) {
        return false;
    }
    
    if (!file_exists(filename)) {
        return false;
    }
    
    char backup_name[512];
    char timestamp[64];
    
    get_timestamp(timestamp, sizeof(timestamp), "%Y%m%d_%H%M%S");
    snprintf(backup_name, sizeof(backup_name), "%s.backup_%s", filename, timestamp);
    
    FILE *source = fopen(filename, "rb");
    if (!source) {
        return false;
    }
    
    FILE *dest = fopen(backup_name, "wb");
    if (!dest) {
        fclose(source);
        return false;
    }
    
    /* Copy file contents */
    unsigned char buffer[4096];
    size_t bytes;
    
    while ((bytes = fread(buffer, 1, sizeof(buffer), source)) > 0) {
        fwrite(buffer, 1, bytes, dest);
    }
    
    fclose(source);
    fclose(dest);
    
    printf("%sBackup created: %s%s\n", 
           COLOR_BRIGHT_GREEN, backup_name, COLOR_RESET);
    
    return true;
}

/**
 * @brief Check if file contains sensitive data
 */
bool file_contains_sensitive_data(const char *filename) {
    if (!filename) {
        return false;
    }
    
    FILE *file = fopen(filename, "r");
    if (!file) {
        return false;
    }
    
    char buffer[1024];
    bool sensitive = false;
    
    while (fgets(buffer, sizeof(buffer), file)) {
        trim_whitespace(buffer);
        
        /* Check for password-like patterns */
        if (strlen(buffer) >= 8) {
            /* Check for mix of character types */
            bool has_lower = false;
            bool has_upper = false;
            bool has_digit = false;
            bool has_special = false;
            
            for (size_t i = 0; buffer[i] != '\0'; i++) {
                if (islower((unsigned char)buffer[i])) has_lower = true;
                else if (isupper((unsigned char)buffer[i])) has_upper = true;
                else if (isdigit((unsigned char)buffer[i])) has_digit = true;
                else has_special = true;
            }
            
            /* If it looks like a password (mix of types) */
            if ((has_lower && has_upper) || 
                (has_lower && has_digit && has_special) ||
                (has_upper && has_digit && has_special)) {
                sensitive = true;
                break;
            }
        }
    }
    
    fclose(file);
    return sensitive;
}