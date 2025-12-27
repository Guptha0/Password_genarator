/**
 * @file clipboard.c
 * @brief Cross-platform clipboard operations implementation
 * @version 1.0
 * @date 2024
 */

#include "clipboard.h"
#include "utils.h"
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
    #include <windows.h>
#elif defined(__APPLE__)
    #include <AvailabilityMacros.h>
    #if MAC_OS_X_VERSION_MAX_ALLOWED >= 1050
        #include <Carbon/Carbon.h>
    #endif
#else
    /* Linux/Unix clipboard via xclip or xsel */
    #include <unistd.h>
    #include <sys/wait.h>
#endif

/* Internal clipboard state */
static bool clipboard_initialized = false;
static PlatformType current_platform = PLATFORM_UNKNOWN;

/**
 * @brief Initialize clipboard system
 */
bool clipboard_init(void) {
    if (clipboard_initialized) {
        return true;
    }
    
    /* Determine platform */
#ifdef _WIN32
    current_platform = PLATFORM_WINDOWS;
#elif defined(__APPLE__)
    current_platform = PLATFORM_MACOS;
#else
    /* Check for Linux/Unix clipboard utilities */
    if (system("which xclip > /dev/null 2>&1") == 0) {
        current_platform = PLATFORM_LINUX;
    } else if (system("which xsel > /dev/null 2>&1") == 0) {
        current_platform = PLATFORM_LINUX;
    } else {
        current_platform = PLATFORM_UNKNOWN;
    }
#endif
    
    clipboard_initialized = true;
    return true;
}

/**
 * @brief Get current platform type
 */
PlatformType get_current_platform(void) {
    if (!clipboard_initialized) {
        clipboard_init();
    }
    return current_platform;
}

/**
 * @brief Windows clipboard implementation
 */
#ifdef _WIN32

ClipboardResult copy_to_clipboard_win(const char *text) {
    if (!text || strlen(text) == 0) {
        return CLIPBOARD_ERROR_EMPTY;
    }
    
    size_t text_len = strlen(text);
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, text_len + 1);
    if (!hMem) {
        return CLIPBOARD_ERROR_ALLOCATION;
    }
    
    /* Lock and copy text */
    char *pMem = (char *)GlobalLock(hMem);
    if (!pMem) {
        GlobalFree(hMem);
        return CLIPBOARD_ERROR_ALLOCATION;
    }
    
    memcpy(pMem, text, text_len + 1);
    GlobalUnlock(hMem);
    
    /* Open clipboard */
    if (!OpenClipboard(NULL)) {
        GlobalFree(hMem);
        return CLIPBOARD_ERROR_OPEN;
    }
    
    /* Clear existing clipboard data */
    EmptyClipboard();
    
    /* Set new clipboard data */
    HANDLE hResult = SetClipboardData(CF_TEXT, hMem);
    
    /* Close clipboard (hMem is now owned by clipboard) */
    CloseClipboard();
    
    if (!hResult) {
        GlobalFree(hMem);
        return CLIPBOARD_ERROR_UNKNOWN;
    }
    
    return CLIPBOARD_SUCCESS;
}

ClipboardResult get_from_clipboard_win(char *buffer, size_t buffer_size) {
    if (!buffer || buffer_size == 0) {
        return CLIPBOARD_ERROR_ALLOCATION;
    }
    
    if (!OpenClipboard(NULL)) {
        return CLIPBOARD_ERROR_OPEN;
    }
    
    HANDLE hData = GetClipboardData(CF_TEXT);
    if (!hData) {
        CloseClipboard();
        return CLIPBOARD_ERROR_EMPTY;
    }
    
    char *pData = (char *)GlobalLock(hData);
    if (!pData) {
        CloseClipboard();
        return CLIPBOARD_ERROR_UNKNOWN;
    }
    
    size_t data_len = strlen(pData);
    if (data_len >= buffer_size) {
        data_len = buffer_size - 1;
    }
    
    memcpy(buffer, pData, data_len);
    buffer[data_len] = '\0';
    
    GlobalUnlock(hData);
    CloseClipboard();
    
    return CLIPBOARD_SUCCESS;
}

ClipboardResult clear_clipboard_win(void) {
    if (!OpenClipboard(NULL)) {
        return CLIPBOARD_ERROR_OPEN;
    }
    
    EmptyClipboard();
    CloseClipboard();
    
    return CLIPBOARD_SUCCESS;
}

#endif /* _WIN32 */

/**
 * @brief macOS clipboard implementation
 */
#ifdef __APPLE__

#if MAC_OS_X_VERSION_MAX_ALLOWED >= 1050

ClipboardResult copy_to_clipboard_mac(const char *text) {
    if (!text || strlen(text) == 0) {
        return CLIPBOARD_ERROR_EMPTY;
    }
    
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    
    NSPasteboard *pasteboard = [NSPasteboard generalPasteboard];
    [pasteboard declareTypes:[NSArray arrayWithObject:NSPasteboardTypeString] owner:nil];
    NSString *nsText = [NSString stringWithUTF8String:text];
    
    if ([pasteboard setString:nsText forType:NSPasteboardTypeString]) {
        [pool release];
        return CLIPBOARD_SUCCESS;
    }
    
    [pool release];
    return CLIPBOARD_ERROR_UNKNOWN;
}

ClipboardResult get_from_clipboard_mac(char *buffer, size_t buffer_size) {
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    
    NSPasteboard *pasteboard = [NSPasteboard generalPasteboard];
    NSString *nsText = [pasteboard stringForType:NSPasteboardTypeString];
    
    if (!nsText) {
        [pool release];
        return CLIPBOARD_ERROR_EMPTY;
    }
    
    const char *cText = [nsText UTF8String];
    if (!cText) {
        [pool release];
        return CLIPBOARD_ERROR_UNKNOWN;
    }
    
    size_t text_len = strlen(cText);
    if (text_len >= buffer_size) {
        text_len = buffer_size - 1;
    }
    
    memcpy(buffer, cText, text_len);
    buffer[text_len] = '\0';
    
    [pool release];
    return CLIPBOARD_SUCCESS;
}

ClipboardResult clear_clipboard_mac(void) {
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    
    NSPasteboard *pasteboard = [NSPasteboard generalPasteboard];
    [pasteboard clearContents];
    
    [pool release];
    return CLIPBOARD_SUCCESS;
}

#else
/* Fallback for older macOS versions */
ClipboardResult copy_to_clipboard_mac(const char *text) {
    return CLIPBOARD_ERROR_PLATFORM;
}

ClipboardResult get_from_clipboard_mac(char *buffer, size_t buffer_size) {
    return CLIPBOARD_ERROR_PLATFORM;
}

ClipboardResult clear_clipboard_mac(void) {
    return CLIPBOARD_ERROR_PLATFORM;
}
#endif

#endif /* __APPLE__ */

/**
 * @brief Linux/Unix clipboard implementation (using xclip/xsel)
 */
#if !defined(_WIN32) && !defined(__APPLE__)

ClipboardResult copy_to_clipboard_unix(const char *text) {
    if (!text || strlen(text) == 0) {
        return CLIPBOARD_ERROR_EMPTY;
    }
    
    /* Try xclip first */
    if (system("which xclip > /dev/null 2>&1") == 0) {
        /* Use xclip to copy to clipboard */
        FILE *pipe = popen("xclip -selection clipboard -in", "w");
        if (!pipe) {
            return CLIPBOARD_ERROR_OPEN;
        }
        
        fprintf(pipe, "%s", text);
        int status = pclose(pipe);
        
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            return CLIPBOARD_SUCCESS;
        }
    }
    
    /* Try xsel as fallback */
    if (system("which xsel > /dev/null 2>&1") == 0) {
        FILE *pipe = popen("xsel --clipboard --input", "w");
        if (!pipe) {
            return CLIPBOARD_ERROR_OPEN;
        }
        
        fprintf(pipe, "%s", text);
        int status = pclose(pipe);
        
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            return CLIPBOARD_SUCCESS;
        }
    }
    
    return CLIPBOARD_ERROR_PLATFORM;
}

ClipboardResult get_from_clipboard_unix(char *buffer, size_t buffer_size) {
    if (!buffer || buffer_size == 0) {
        return CLIPBOARD_ERROR_ALLOCATION;
    }
    
    /* Try xclip first */
    if (system("which xclip > /dev/null 2>&1") == 0) {
        FILE *pipe = popen("xclip -selection clipboard -out", "r");
        if (!pipe) {
            return CLIPBOARD_ERROR_OPEN;
        }
        
        size_t bytes_read = fread(buffer, 1, buffer_size - 1, pipe);
        buffer[bytes_read] = '\0';
        
        int status = pclose(pipe);
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            return CLIPBOARD_SUCCESS;
        }
    }
    
    /* Try xsel as fallback */
    if (system("which xsel > /dev/null 2>&1") == 0) {
        FILE *pipe = popen("xsel --clipboard --output", "r");
        if (!pipe) {
            return CLIPBOARD_ERROR_OPEN;
        }
        
        size_t bytes_read = fread(buffer, 1, buffer_size - 1, pipe);
        buffer[bytes_read] = '\0';
        
        int status = pclose(pipe);
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            return CLIPBOARD_SUCCESS;
        }
    }
    
    return CLIPBOARD_ERROR_PLATFORM;
}

ClipboardResult clear_clipboard_unix(void) {
    /* Clear by copying empty string */
    return copy_to_clipboard_unix("");
}

#endif /* Linux/Unix */

/**
 * @brief Copy text to system clipboard (platform-independent)
 */
ClipboardResult copy_to_clipboard(const char *text) {
    if (!clipboard_initialized) {
        clipboard_init();
    }
    
    if (!text || strlen(text) == 0) {
        return CLIPBOARD_ERROR_EMPTY;
    }
    
    switch (current_platform) {
#ifdef _WIN32
        case PLATFORM_WINDOWS:
            return copy_to_clipboard_win(text);
#endif
            
#ifdef __APPLE__
        case PLATFORM_MACOS:
            return copy_to_clipboard_mac(text);
#endif
            
#if !defined(_WIN32) && !defined(__APPLE__)
        case PLATFORM_LINUX:
            return copy_to_clipboard_unix(text);
#endif
            
        default:
            return CLIPBOARD_ERROR_PLATFORM;
    }
}

/**
 * @brief Get text from system clipboard
 */
ClipboardResult get_from_clipboard(char *buffer, size_t buffer_size) {
    if (!clipboard_initialized) {
        clipboard_init();
    }
    
    if (!buffer || buffer_size == 0) {
        return CLIPBOARD_ERROR_ALLOCATION;
    }
    
    switch (current_platform) {
#ifdef _WIN32
        case PLATFORM_WINDOWS:
            return get_from_clipboard_win(buffer, buffer_size);
#endif
            
#ifdef __APPLE__
        case PLATFORM_MACOS:
            return get_from_clipboard_mac(buffer, buffer_size);
#endif
            
#if !defined(_WIN32) && !defined(__APPLE__)
        case PLATFORM_LINUX:
            return get_from_clipboard_unix(buffer, buffer_size);
#endif
            
        default:
            return CLIPBOARD_ERROR_PLATFORM;
    }
}

/**
 * @brief Clear clipboard contents
 */
ClipboardResult clear_clipboard(void) {
    if (!clipboard_initialized) {
        clipboard_init();
    }
    
    switch (current_platform) {
#ifdef _WIN32
        case PLATFORM_WINDOWS:
            return clear_clipboard_win();
#endif
            
#ifdef __APPLE__
        case PLATFORM_MACOS:
            return clear_clipboard_mac();
#endif
            
#if !defined(_WIN32) && !defined(__APPLE__)
        case PLATFORM_LINUX:
            return clear_clipboard_unix();
#endif
            
        default:
            return CLIPBOARD_ERROR_PLATFORM;
    }
}

/**
 * @brief Copy with auto-clear after specified seconds
 */
ClipboardResult copy_with_autoclear(const char *text, int seconds) {
    if (!text || seconds < 0) {
        return CLIPBOARD_ERROR_EMPTY;
    }
    
    ClipboardResult result = copy_to_clipboard(text);
    if (result != CLIPBOARD_SUCCESS) {
        return result;
    }
    
    if (seconds > 0) {
        /* Note: In a real implementation, you'd want to use a separate thread
           or process for the auto-clear timer. For simplicity, we'll just
           note that auto-clear should be implemented. */
        printf("%sNote: Password copied to clipboard%s\n", 
               COLOR_BRIGHT_YELLOW, COLOR_RESET);
        printf("%sAuto-clear after %d seconds (not implemented in console)%s\n",
               COLOR_BRIGHT_YELLOW, seconds, COLOR_RESET);
    }
    
    return CLIPBOARD_SUCCESS;
}

/**
 * @brief Get string representation of clipboard result
 */
const char *get_clipboard_result_string(ClipboardResult result) {
    switch (result) {
        case CLIPBOARD_SUCCESS:
            return "Successfully copied to clipboard";
        case CLIPBOARD_ERROR_ALLOCATION:
            return "Memory allocation error";
        case CLIPBOARD_ERROR_OPEN:
            return "Failed to open clipboard";
        case CLIPBOARD_ERROR_EMPTY:
            return "No text to copy";
        case CLIPBOARD_ERROR_PLATFORM:
            return "Unsupported platform or missing clipboard utilities";
        case CLIPBOARD_ERROR_UNKNOWN:
            return "Unknown clipboard error";
        default:
            return "Invalid clipboard result";
    }
}

/**
 * @brief Cleanup clipboard system resources
 */
void clipboard_cleanup(void) {
    /* Currently no cleanup needed, but function exists for future use */
    clipboard_initialized = false;
}

/**
 * @brief Test clipboard functionality
 */
bool test_clipboard(void) {
    printf("%sTesting clipboard functionality...%s\n", 
           COLOR_BRIGHT_YELLOW, COLOR_RESET);
    
    const char *test_text = "Clipboard Test 123";
    ClipboardResult result = copy_to_clipboard(test_text);
    
    if (result == CLIPBOARD_SUCCESS) {
        printf("%s✅ Clipboard test successful%s\n", 
               COLOR_BRIGHT_GREEN, COLOR_RESET);
        
        /* Verify by reading back */
        char buffer[256];
        result = get_from_clipboard(buffer, sizeof(buffer));
        
        if (result == CLIPBOARD_SUCCESS && strcmp(buffer, test_text) == 0) {
            printf("%s✅ Clipboard read/write verified%s\n", 
                   COLOR_BRIGHT_GREEN, COLOR_RESET);
            return true;
        }
    }
    
    printf("%s❌ Clipboard test failed: %s%s\n", 
           COLOR_BRIGHT_RED, get_clipboard_result_string(result), COLOR_RESET);
    return false;
}