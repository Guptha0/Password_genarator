/**
 * @file utils.c
 * @brief Utility functions and helpers implementation
 * @version 1.0
 * @date 2024
 */

#include "utils.h"
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <errno.h>

#ifdef _WIN32
    #include <windows.h>
    #include <wincrypt.h>
#else
    #include <unistd.h>
    #include <fcntl.h>
    #include <sys/ioctl.h>
    #include <termios.h>
#endif

/* Internal random state */
static bool random_initialized = false;

/**
 * @brief Initialize secure random number generator
 */
bool init_secure_random(void) {
    if (random_initialized) {
        return true;
    }
    
#ifdef _WIN32
    /* Windows uses CryptGenRandom */
    HCRYPTPROV hProvider = 0;
    if (!CryptAcquireContextW(&hProvider, NULL, NULL, PROV_RSA_FULL, 
                              CRYPT_VERIFYCONTEXT | CRYPT_SILENT)) {
        return false;
    }
    CryptReleaseContext(hProvider, 0);
#else
    /* Linux/Unix uses /dev/urandom */
    /* Check if /dev/urandom is accessible */
    FILE *urandom = fopen("/dev/urandom", "rb");
    if (!urandom) {
        return false;
    }
    fclose(urandom);
#endif
    
    /* Seed standard random for non-cryptographic uses */
    srand((unsigned int)time(NULL));
    random_initialized = true;
    return true;
}

/**
 * @brief Generate cryptographically secure random bytes
 */
bool get_random_bytes(unsigned char *buffer, size_t size) {
    if (!buffer || size == 0) {
        return false;
    }
    
    if (!random_initialized && !init_secure_random()) {
        return false;
    }
    
#ifdef _WIN32
    HCRYPTPROV hProvider = 0;
    if (!CryptAcquireContextW(&hProvider, NULL, NULL, PROV_RSA_FULL, 
                              CRYPT_VERIFYCONTEXT | CRYPT_SILENT)) {
        return false;
    }
    
    BOOL success = CryptGenRandom(hProvider, (DWORD)size, buffer);
    CryptReleaseContext(hProvider, 0);
    return success ? true : false;
#else
    FILE *urandom = fopen("/dev/urandom", "rb");
    if (!urandom) {
        return false;
    }
    
    size_t bytes_read = fread(buffer, 1, size, urandom);
    fclose(urandom);
    
    return bytes_read == size;
#endif
}

/**
 * @brief Generate secure random number in range
 */
unsigned int random_range(unsigned int min, unsigned int max) {
    if (min > max) {
        unsigned int temp = min;
        min = max;
        max = temp;
    }
    
    unsigned int range = max - min + 1;
    unsigned int limit = (RAND_MAX / range) * range;
    unsigned int random_value;
    
    do {
        random_value = rand();
    } while (random_value >= limit);
    
    return min + (random_value % range);
}

/**
 * @brief Secure string initialization
 */
bool secure_string_init(SecureString *str, size_t initial_capacity) {
    if (!str || initial_capacity == 0) {
        return false;
    }
    
    str->data = (char *)calloc(initial_capacity + 1, sizeof(char));
    if (!str->data) {
        return false;
    }
    
    str->length = 0;
    str->capacity = initial_capacity;
    return true;
}

/**
 * @brief Append character to secure string
 */
bool secure_string_append(SecureString *str, char c) {
    if (!str || !str->data) {
        return false;
    }
    
    if (str->length >= str->capacity) {
        size_t new_capacity = str->capacity * 2;
        char *new_data = (char *)realloc(str->data, new_capacity + 1);
        if (!new_data) {
            return false;
        }
        
        /* Clear the newly allocated memory */
        memset(new_data + str->capacity, 0, new_capacity - str->capacity + 1);
        
        str->data = new_data;
        str->capacity = new_capacity;
    }
    
    str->data[str->length++] = c;
    str->data[str->length] = '\0';
    return true;
}

/**
 * @brief Append C string to secure string
 */
bool secure_string_append_cstr(SecureString *str, const char *src) {
    if (!str || !str->data || !src) {
        return false;
    }
    
    size_t src_len = strlen(src);
    if (src_len == 0) {
        return true;
    }
    
    /* Ensure enough capacity */
    if (str->length + src_len >= str->capacity) {
        size_t new_capacity = str->capacity;
        while (str->length + src_len >= new_capacity) {
            new_capacity *= 2;
        }
        
        char *new_data = (char *)realloc(str->data, new_capacity + 1);
        if (!new_data) {
            return false;
        }
        
        str->data = new_data;
        str->capacity = new_capacity;
    }
    
    memcpy(str->data + str->length, src, src_len);
    str->length += src_len;
    str->data[str->length] = '\0';
    return true;
}

/**
 * @brief Clear secure string (zero memory)
 */
void secure_string_clear(SecureString *str) {
    if (str && str->data) {
        secure_clear(str->data, str->capacity + 1);
        str->length = 0;
    }
}

/**
 * @brief Free secure string memory
 */
void secure_string_free(SecureString *str) {
    if (str) {
        if (str->data) {
            secure_clear(str->data, str->capacity + 1);
            free(str->data);
            str->data = NULL;
        }
        str->length = 0;
        str->capacity = 0;
    }
}

/**
 * @brief Securely clear memory (zero out)
 */
void secure_clear(void *ptr, size_t size) {
    if (ptr && size > 0) {
#ifdef _WIN32
        SecureZeroMemory(ptr, size);
#else
        volatile unsigned char *p = (volatile unsigned char *)ptr;
        while (size--) {
            *p++ = 0;
        }
#endif
    }
}

/**
 * @brief Get current timestamp as string
 */
char *get_timestamp(char *buffer, size_t buffer_size, const char *format) {
    if (!buffer || buffer_size == 0) {
        return NULL;
    }
    
    time_t raw_time;
    struct tm *time_info;
    
    time(&raw_time);
    time_info = localtime(&raw_time);
    
    if (!format) {
        format = "%Y-%m-%d %H:%M:%S";
    }
    
    strftime(buffer, buffer_size, format, time_info);
    return buffer;
}

/**
 * @brief Validate string input
 */
bool validate_input_string(const char *str, size_t max_length) {
    if (!str) {
        return false;
    }
    
    size_t len = strlen(str);
    if (len == 0 || len > max_length) {
        return false;
    }
    
    /* Check for null bytes in the middle of string */
    for (size_t i = 0; i < len; i++) {
        if (str[i] == '\0' && i != len) {
            return false;
        }
        
        /* Check for control characters (except tab and newline) */
        if (iscntrl((unsigned char)str[i]) && 
            str[i] != '\t' && str[i] != '\n' && str[i] != '\r') {
            return false;
        }
    }
    
    return true;
}

/**
 * @brief Convert string to integer with validation
 */
bool string_to_int(const char *str, int *result, int min, int max) {
    if (!str || !result) {
        return false;
    }
    
    char *endptr;
    errno = 0;
    long value = strtol(str, &endptr, 10);
    
    /* Check for conversion errors */
    if (errno == ERANGE || value < INT_MIN || value > INT_MAX) {
        return false;
    }
    
    /* Check if entire string was converted */
    if (*endptr != '\0' && *endptr != '\n') {
        return false;
    }
    
    /* Check range */
    if (value < min || value > max) {
        return false;
    }
    
    *result = (int)value;
    return true;
}

/**
 * @brief Yes/No prompt
 */
bool prompt_yes_no(const char *prompt, bool default_yes) {
    if (!prompt) {
        return default_yes;
    }
    
    printf("%s [%s/%s]: ", prompt, default_yes ? "Y" : "y", 
           default_yes ? "n" : "N");
    
    char buffer[16];
    if (!fgets(buffer, sizeof(buffer), stdin)) {
        return default_yes;
    }
    
    trim_whitespace(buffer);
    
    if (strlen(buffer) == 0) {
        return default_yes;
    }
    
    char first_char = tolower((unsigned char)buffer[0]);
    return (first_char == 'y');
}

/**
 * @brief Get integer input with validation
 */
int get_integer_input(const char *prompt, int min, int max, int default_value) {
    if (!prompt) {
        return default_value;
    }
    
    printf("%s [%d-%d, default: %d]: ", prompt, min, max, default_value);
    
    char buffer[32];
    if (!fgets(buffer, sizeof(buffer), stdin)) {
        return default_value;
    }
    
    trim_whitespace(buffer);
    
    if (strlen(buffer) == 0) {
        return default_value;
    }
    
    int result;
    if (!string_to_int(buffer, &result, min, max)) {
        printf("Invalid input. Using default value %d.\n", default_value);
        return default_value;
    }
    
    return result;
}

/**
 * @brief Trim whitespace from string
 */
char *trim_whitespace(char *str) {
    if (!str) {
        return NULL;
    }
    
    char *end;
    
    /* Trim leading space */
    while (isspace((unsigned char)*str)) {
        str++;
    }
    
    if (*str == 0) {
        return str;
    }
    
    /* Trim trailing space */
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) {
        end--;
    }
    
    /* Write new null terminator */
    *(end + 1) = '\0';
    
    return str;
}

/**
 * @brief Check if file exists
 */
bool file_exists(const char *filename) {
    if (!filename) {
        return false;
    }
    
#ifdef _WIN32
    DWORD attributes = GetFileAttributesA(filename);
    return (attributes != INVALID_FILE_ATTRIBUTES && 
            !(attributes & FILE_ATTRIBUTE_DIRECTORY));
#else
    struct stat buffer;
    return (stat(filename, &buffer) == 0 && S_ISREG(buffer.st_mode));
#endif
}

/**
 * @brief Get file size
 */
long get_file_size(const char *filename) {
    if (!filename) {
        return -1;
    }
    
#ifdef _WIN32
    struct _stat buffer;
    if (_stat(filename, &buffer) != 0) {
        return -1;
    }
    return buffer.st_size;
#else
    struct stat buffer;
    if (stat(filename, &buffer) != 0) {
        return -1;
    }
    return buffer.st_size;
#endif
}

/**
 * @brief Sleep for milliseconds (cross-platform)
 */
void sleep_ms(unsigned int milliseconds) {
#ifdef _WIN32
    Sleep(milliseconds);
#else
    usleep(milliseconds * 1000);
#endif
}

/**
 * @brief Get terminal width (cross-platform)
 */
int get_terminal_width(void) {
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        return csbi.srWindow.Right - csbi.srWindow.Left + 1;
    }
    return 80;
#else
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0) {
        return w.ws_col;
    }
    
    /* Fallback to environment variable */
    char *col_str = getenv("COLUMNS");
    if (col_str) {
        int cols;
        if (string_to_int(col_str, &cols, 40, 512)) {
            return cols;
        }
    }
    
    return 80;
#endif
}

/**
 * @brief Get a single character input without echo
 */
char getch(void) {
#ifdef _WIN32
    return _getch();
#else
    struct termios oldt, newt;
    char ch;
    
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    
    ch = getchar();
    
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
#endif
}

/**
 * @brief Check if stdin has input waiting
 */
bool kbhit(void) {
#ifdef _WIN32
    return _kbhit() != 0;
#else
    struct timeval tv = {0, 0};
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    return select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv) > 0;
#endif
}

/**
 * @brief Create directory if it doesn't exist
 */
bool create_directory(const char *path) {
    if (!path) {
        return false;
    }
    
#ifdef _WIN32
    return CreateDirectoryA(path, NULL) || GetLastError() == ERROR_ALREADY_EXISTS;
#else
    struct stat st = {0};
    if (stat(path, &st) == -1) {
        return mkdir(path, 0700) == 0;
    }
    return S_ISDIR(st.st_mode);
#endif
}