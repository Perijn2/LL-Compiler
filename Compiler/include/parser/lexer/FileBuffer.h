// ------------------------------------------------------------------------------------------------
// Include guard
// ------------------------------------------------------------------------------------------------

#ifndef LEXER_FILE_BUFFER_H
#define LEXER_FILE_BUFFER_H

// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------

#include <stdbool.h>

#include "parser/ParserCore.h"

// ------------------------------------------------------------------------------------------------
// Public definitions
// ------------------------------------------------------------------------------------------------

#define FILE_BUFFER_EOF         (-1)

PARSER_CORE_DEFINE_HANDLE(FileBuffer)

typedef enum FileBufferTypes {
	FILE_BUFFER_TYPE_UNKNOWN = 0,
	FILE_BUFFER_TYPE_DISK,
	FILE_BUFFER_TYPE_VIRTUAL,
	FILE_BUFFER_TYPE_NETWORK,
} FileBufferTypes;

typedef enum FileBufferEncoding {
	FILE_ENCODING_UNKNOWN = 0,
	FILE_ENCODING_ASCII,        // 7-bit ASCII (strict)
	FILE_ENCODING_UTF8,         // UTF-8 (8-bit variable length)
	FILE_ENCODING_UTF16_LE,     // UTF-16 Little Endian
	FILE_ENCODING_UTF16_BE,     // UTF-16 Big Endian
	FILE_ENCODING_UTF32_LE,     // UTF-32 Little Endian
	FILE_ENCODING_UTF32_BE,     // UTF-32 Big Endian
	FILE_ENCODING_LATIN1,       // ISO-8859-1
} FileBufferEncoding;

typedef struct FileBufferConfig_T {
	const char* fileName;
	const char* filePath;
	FileBufferTypes fileType;
	FileBufferEncoding encoding;
	bool autoDetectEncoding;
} FileBufferConfig;

typedef struct FileBufferCursor_T {
	const uint8_t* begin;       /* first byte of file                          */
	const uint8_t* cur;         /* current cursor                              */
	const uint8_t* end;         /* one-past-last byte                          */
} FileBufferCursor;

/**
* @brief Create a file buffer with a specified configuration
* 
* @description Creates a file buffer used by the lexer. The targeted
* file gets read into a large buffer to minimize kernel operations.
* 
* @param cfg[in] Configuration file for the buffer
* @param file[out] FileBuffer pointer
* 
* @return ParserResult
*/
PARSER_ATTR ParserResult PARSER_CALL CreateFileBuffer(
	FileBufferConfig* cfg,
	FileBuffer* file);

/**
* @brief Destroys the file buffer
*
* @description When the file buffer isn't used anymore, the destroy
* will cleanup all the memory used by the buffer.
*
* @param file[in] FileBuffer handle
*
* @return void
*/
PARSER_ATTR inline ParserResult PARSER_CALL DestroyFileBuffer(
	FileBuffer file);

/**
 * @brief Advance file buffer cursor and return current byte
 *
 * @param file[in] Pointer to FileBuffer
 *
 * @return int32_t Current byte (0-255) or EOF (-1) if at end
 *
 * @note Returns the byte AFTER advancing, following standard C conventions
 */
PARSER_ATTR inline int32_t PARSER_CALL AdvanceFileBuffer(
	FileBuffer file);

/**
 * @brief Peeks the file buffer for a char without advancing
 *
 * @param file[in] Pointer to FileBuffer
 *
 * @return int32_t Current byte (0-255) or EOF (-1) if at end
 *
 * @note Returns the byte AFTER advancing, following standard C conventions
 */
PARSER_ATTR inline int32_t PARSER_CALL PeekFileBuffer(
	FileBuffer file);

/**
* @brief Returns the file buffer cursor
*
* @description Returns the file buffer cursor. This cursor can be
* used to implement custom read functionality.
*
* @param file[in] FileBuffer handle
*
* @return FileBufferCursor Pointer to the cursor struct
*/
PARSER_ATTR inline const FileBufferCursor* PARSER_CALL GetFileBufferCursor(
	FileBuffer file);


/**
* @brief Returns the file buffer encoding
*
* @param file[in] FileBuffer handle
*
* @return FileBufferEncoding The encoding style used by the file buffer
*/
PARSER_ATTR inline FileBufferEncoding PARSER_CALL GetFileBufferEncoding(
	FileBuffer file);

// ------------------------------------------------------------------------------------------------

#endif // !LEXER_FILE_BUFFER_H

// ------------------------------------------------------------------------------------------------