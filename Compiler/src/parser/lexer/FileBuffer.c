// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------

#include "parser/lexer/FileBuffer.h"

#include "parser/Results.h"

// ------------------------------------------------------------------------------------------------
// Private definitions
// ------------------------------------------------------------------------------------------------

#if defined(PLATFORM_WINDOWS)
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
#elif defined(PLATFORM_LINUX)
	#include <sys/mman.h>      // mmap, munmap
	#include <sys/stat.h>      // fstat
	#include <fcntl.h>         // open, O_RDONLY
	#include <unistd.h>        // close
#endif

struct FileBuffer_T {
	const uint8_t* data;       // Pointer to memory-mapped file data
	ParserSize size;               // Size of the file in bytes

	FileBufferCursor Cursor;

#if defined(PLATFORM_WINDOWS)
	HANDLE fileHandle;         // Windows file handle
	HANDLE mappingHandle;      // Windows file mapping handle
#elif defined(PLATFORM_LINUX)
	int fileDescriptor;        // Linux file descriptor
#endif
};

#define MAP_FILE_BUFFER_CURSOR(buffer) \
    buffer->Cursor.begin = buffer->data; \
    buffer->Cursor.cur = buffer->data; \
    buffer->Cursor.end = buffer->data + buffer->size;


PARSER_ATTR ParserResult PARSER_CALL CreateFileBuffer(
	FileBufferConfig* cfg,
	FileBuffer* file)
{

    if (!cfg || !file) {
        return PARSER_ERROR_INVALID_ARG;
    }

    FileBuffer hdl = PARSER_MALLOC(sizeof(struct FileBuffer_T), NULL);
    if (!hdl)
        return PARSER_ERROR_INVALID_ARG;

#if defined(PLATFORM_WINDOWS)
    // Open file for reading
    hdl->fileHandle = CreateFileA(
        cfg->filePath,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hdl->fileHandle == INVALID_HANDLE_VALUE)
        return PARSER_ERROR_INVALID_FILE;

    // Get file size
    LARGE_INTEGER fileSize;
    if (!GetFileSizeEx(hdl->fileHandle, &fileSize)) {
        CloseHandle(hdl->fileHandle);
        return PARSER_ERROR_INVALID_FILE;
    }
    hdl->size = (size_t)fileSize.QuadPart;

    // Create file mapping
    hdl->mappingHandle = CreateFileMappingA(
        hdl->fileHandle,
        NULL,
        PAGE_READONLY,
        0,
        0,
        NULL
    );

    if (hdl->mappingHandle == NULL) {
        CloseHandle(hdl->fileHandle);
        return PARSER_ERROR_INVALID_FILE;
    }

    // Map view of file
    hdl->data = (const uint8_t*)MapViewOfFile(
        hdl->mappingHandle,
        FILE_MAP_READ,
        0,
        0,
        0
    );

    if (hdl->data == NULL) {
        CloseHandle(hdl->mappingHandle);
        CloseHandle(hdl->fileHandle);
        return PARSER_ERROR_FILE_DATA_NULL;
    }

    MAP_FILE_BUFFER_CURSOR(hdl);

    *file = hdl;

    return PARSER_RESULT_SUCCESS;

#elif defined(PLATFORM_LINUX)
    // Open file
    hdl->fileDescriptor = open(path, O_RDONLY);
    if (hdl->fileDescriptor == -1) {
        return PARSER_ERROR_INVALID_FILE;
    }

    // Get file size
    struct stat fileStat;
    if (fstat(hdl->fileDescriptor, &fileStat) == -1) {
        close(hdl->fileDescriptor);
        return PARSER_ERROR_INVALID_FILE;
    }
    hdl->size = (size_t)fileStat.st_size;

    // Memory-map the file
    hdl->data = (const uint8_t*)mmap(
        NULL,
        hdl->size,
        PROT_READ,
        MAP_PRIVATE,
        hdl->fileDescriptor,
        0
    );

    if (hdl->data == MAP_FAILED) {
        close(hdl->fileDescriptor);
        return PARSER_ERROR_INVALID_FILE;
    }

    MAP_FILE_BUFFER_CURSOR(hdl);

    *file = hdl;

    return PARSER_RESULT_SUCCESS;

#else

#error "Unsupported platform"

#endif

}


PARSER_ATTR inline ParserResult PARSER_CALL DestroyFileBuffer(
	FileBuffer file)
{

    if (!file)
        return PARSER_ERROR_INVALID_ARG;

#if defined(PLATFORM_WINDOWS)
    if (file->data) {
        UnmapViewOfFile(file->data);
        file->data = NULL;
    }

    if (file->mappingHandle) {
        CloseHandle(file->mappingHandle);
        file->mappingHandle = NULL;
    }

    if (file->fileHandle != INVALID_HANDLE_VALUE) {
        CloseHandle(file->fileHandle);
        file->fileHandle = INVALID_HANDLE_VALUE;
    }

#elif defined(PLATFORM_LINUX)
    if (file->data && file->data != MAP_FAILED) {
        munmap((void*)file->data, file->size);
        file->data = NULL;
    }

    if (file->fileDescriptor != -1) {
        close(file->fileDescriptor);
        file->fileDescriptor = -1;
    }
#endif

    file->size = 0;

    PARSER_FREE(file);

    return PARSER_RESULT_SUCCESS;
}

PARSER_ATTR int32_t PARSER_CALL AdvanceFileBuffer(FileBuffer file)
{
    if (!file) {
        return FILE_BUFFER_EOF;
    }

    // Check if we can advance
    if (file->Cursor.cur >= file->Cursor.end) {
        return FILE_BUFFER_EOF;  // Already at end
    }

    // Advance and return the byte at new position
    file->Cursor.cur++;

    if (file->Cursor.cur >= file->Cursor.end) {
        return FILE_BUFFER_EOF;  // Reached end after advancing
    }

    return (unsigned char)*file->Cursor.cur;  // Cast to unsigned to get 0-255
}

PARSER_ATTR inline const FileBufferCursor* PARSER_CALL GetFileBufferCursor(
	FileBuffer file)
{
    if (!file)
        return NULL;

    return &file->Cursor;
}
