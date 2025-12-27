/**
 * @file file_ops.h
 * @brief File operations for password saving
 * @version 1.0
 * @date 2024
 */

#ifndef FILE_OPS_H
#define FILE_OPS_H

#include "password.h"
#include <stdbool.h>

/**
 * @brief Save password to text file
 * @param result Password result to save
 * @param filename File to save to
 * @param append Append to file (true) or overwrite (false)
 * @param include_metadata Include metadata like timestamp and strength
 * @return true if successful, false otherwise
 */
bool save_password_to_file(const PasswordResult *result, const char *filename, 
                          bool append, bool include_metadata);

/**
 * @brief Save multiple passwords to file
 * @param results Array of password results
 * @param count Number of passwords
 * @param filename File to save to
 * @param include_metadata Include metadata
 * @return true if successful, false otherwise
 */
bool save_bulk_passwords_to_file(const PasswordResult *results, size_t count,
                                const char *filename, bool include_metadata);

/**
 * @brief Save passwords to CSV file
 * @param results Array of password results
 * @param count Number of passwords
 * @param filename CSV file to save to
 * @return true if successful, false otherwise
 */
bool save_passwords_to_csv(const PasswordResult *results, size_t count,
                          const char *filename);

/**
 * @brief Save passwords to JSON file
 * @param results Array of password results
 * @param count Number of passwords
 * @param filename JSON file to save to
 * @return true if successful, false otherwise
 */
bool save_passwords_to_json(const PasswordResult *results, size_t count,
                           const char *filename);

/**
 * @brief Load passwords from file
 * @param filename File to load from
 * @param count Pointer to store number of passwords loaded
 * @return Array of password results (caller must free), or NULL on error
 */
PasswordResult *load_passwords_from_file(const char *filename, size_t *count);

/**
 * @brief Securely delete file (overwrite multiple times)
 * @param filename File to delete
 * @param passes Number of overwrite passes (3-7 recommended)
 * @return true if successful, false otherwise
 */
bool secure_delete_file(const char *filename, int passes);

/**
 * @brief Create backup of password file
 * @param filename File to backup
 * @return true if successful, false otherwise
 */
bool create_backup(const char *filename);

/**
 * @brief Check if file contains sensitive data
 * @param filename File to check
 * @return true if file appears to contain passwords, false otherwise
 */
bool file_contains_sensitive_data(const char *filename);

#endif /* FILE_OPS_H */