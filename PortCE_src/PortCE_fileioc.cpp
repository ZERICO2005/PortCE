/**
 * @file
 *
 * @author "zerico2005"
 */

#include <algorithm>
#include "PortCE_Common.h"

#include <tice.h>
#include <fileioc.h>

#include "PortCE.h"

#undef ti_SetArchiveStatus

#define NameSize (8)
#define RamFolderPath "./Ti-Ram"
#define ArchiveFolderPath "./Ti-Archive"

/**
 * @brief
 *
 */
static const char* PortCE_FilePaths[] = {
    "./",
    "../",
    "../../",
    "./src/gfx/",
    "../src/gfx/",
    "../../src/gfx/",
    "./extra/",
    "../extra/",
    "../../extra/"
};

__attribute__((unused)) static const uint8_t File_8XV_Text[] = {
/* X      0 |  1 |  2 |  3 |  4 |  5 |  6 |  7 |  8 |  9 |  A |  B |  C |  D |  E |  F */
/* 0 */  '*', '*', 'T', 'I', '8', '3', '*',0x1A,0x0A,'\0','\0','\0','\0','\0','\0','\0',
/* 1 */ '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0',
/* 2 */ '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0',
/* 3 */ '\0','\0','\0','\0','\0'
};

typedef struct __attribute__((packed)) Ti_File_Structure  {
    char header[53];
    uint16_t file_size; /** 17 bytes more than data_size */
    uint8_t value_0x0D;
    uint8_t valueA_0x00;
    uint16_t data_size; /** size of actual data */
    uint8_t value_0x15; /** file type indicator? */
    char name[8];
    uint8_t valueB_0x00;
    uint8_t value_0x80;
    unsigned char data[]; /** size = data_size */
} Ti_File_Structure;

typedef struct Ti_File_Handle {
    bool active;
    bool read;
    bool write;
    bool append;
    FILE* file;
    char name[NameSize + 1];
} Ti_File_Handle;

static Ti_File_Handle ti_File_Handle[256];

/**
 * @brief Finds the first empty file handle
 *
 * @return uint8_t file handle or 0 on failure
 */
static uint8_t get_Empty_File_Handle() {
    for (size_t h = 1; h < ARRAY_LENGTH(ti_File_Handle); h++) {
        if (ti_File_Handle[h].active == false) {
            return h;
            break;
        }
    }
    return 0; // No handles available
}

/**
 * @brief Sets the file access flags
 *
 * @param[in] handle non zero file handle
 * @param[in] mode `r`, `w`, `a`, `r+`, `w+`, `a+`
 * @return false if parameters are invalid
 */
static bool set_File_Access_Flags(uint8_t handle, const char* mode) {
    if (handle == 0 || mode == NULL) {
        return false;
    }
    switch (mode[0]) {
        case 'r':
            ti_File_Handle[handle].read = true;
        break;
        case 'w':
            ti_File_Handle[handle].write = true;
        break;
        case 'a':
            ti_File_Handle[handle].append = true;
        break;
        default:
            return false;
    };
    if (mode[1] == '+') {
        if (ti_File_Handle[handle].read == true) {
            ti_File_Handle[handle].write = true;
        } else {
            ti_File_Handle[handle].read = true;
        }
    }
    return true;
}

static bool validate_File_Name(const char* name) {
    if (name == NULL) {
        return false;
    }
    size_t len = 0;
    while (*name != '\0') {
        len++;
        if (len > NameSize) {
            return false;
        }
        if (*name == '/') {
            return false; // Invalid character
        }
        name++;
    }
    if (len == 0) {
        return false;
    }
    return true;
}

static bool validate_File_Handle(uint8_t handle) {
    if (handle == 0) {
        return false;
    }
    if (ti_File_Handle[handle].active == false) {
        return false;
    }
    if (ti_File_Handle[handle].file == NULL) {
        return false;
    }
    if (
        (ti_File_Handle[handle].read == false) &&
        (ti_File_Handle[handle].write == false) &&
        (ti_File_Handle[handle].append == false)
    ) {
        return false;
    }
    return true;
}

static FILE* open_File(const char* name, const char* mode) {
    if (name == NULL || mode == NULL) {
        return NULL;
    }
    // Temporary
        static char path[4096];
        memset(path, '\0', sizeof(path));
        static char mode_temp[16];
        memset(mode_temp, '\0', sizeof(mode_temp));

    for (size_t i = 0; i < ARRAY_LENGTH(PortCE_FilePaths); i++) {
        memset(path, '\0', sizeof(path));
        snprintf(path,sizeof(path),"%s%s.8xv", PortCE_FilePaths[i], name);
        snprintf(mode_temp,sizeof(mode_temp),"%sb",mode);
        FILE* file = fopen(path, mode_temp);
        if (file != NULL) {
            return file;
        }
    }
    return NULL; // File not found
}

/* Public Functions */

void PortCE_terminate_fileioc(void);

uint8_t ti_Open(const char* name, const char* mode) {
    if (validate_File_Name(name) == false || mode == NULL) {
        return 0;
    }
    uint8_t handle = get_Empty_File_Handle();
    if (set_File_Access_Flags(handle, mode) == false) {
        return 0;
    }
    FILE* file = open_File(name, mode);
    if (file == NULL) {
        return 0;
    }
    ti_File_Handle[handle].file = file;

    {
        // memset(ti_File_Handle[handle].name, '\0', NameSize + 1);
        // memcpy(ti_File_Handle[handle].name, name, NameSize);

        /* Prevents -fsanitize=address from getting mad */
        bool found_null = false;
        for (size_t i = 0; i < NameSize + 1; i++) {
            if (found_null == false) {
                if (name[i] == '\0') {
                    found_null = true;
                }
                ti_File_Handle[handle].name[i] = name[i];
            }
            ti_File_Handle[handle].name[i] = '\0';
        }
    }

    ti_File_Handle[handle].active = true;
    return handle;
}

int ti_Close(uint8_t handle) {
    if (handle == 0) {
        return 0;
    }
    if (ti_File_Handle[handle].active == false) {
        return 0; // File already closed
    }
    ti_File_Handle[handle].active = false;
    return fclose(ti_File_Handle[handle].file);
}

size_t ti_Write(const void *data, size_t size, size_t count, uint8_t handle) {
    if (validate_File_Handle(handle) == false) {
        return 0;
    }
    return fwrite(data, size, count, ti_File_Handle[handle].file);
}

size_t ti_Read(void *data, size_t size, size_t count, uint8_t handle) {
    if (validate_File_Handle(handle) == false) {
        return 0;
    }
    return fread(data, size, count, ti_File_Handle[handle].file);
}

int ti_PutC(char ch, uint8_t handle) {
    if (validate_File_Handle(handle) == false) {
        return EOF;
    }
    return putc(ch,ti_File_Handle[handle].file);
}

int ti_GetC(uint8_t handle) {
    if (validate_File_Handle(handle) == false) {
        return EOF;
    }
    return getc(ti_File_Handle[handle].file);
}

int ti_Seek(int offset, size_t origin, uint8_t handle) {
    if (validate_File_Handle(handle) == false) {
        return EOF;
    }
    return (int24_t)fseek(ti_File_Handle[handle].file,offset,origin);
}

int ti_Rewind(uint8_t handle) {
    return (int24_t)ti_Seek(0, SEEK_SET, handle);
}

uint16_t ti_Tell(uint8_t handle) {
    if (validate_File_Handle(handle) == false) {
        return 0;
    }
    long position = ftell(ti_File_Handle[handle].file);
    if (position < 0) {
        printf("ftell error: %ld", position);
        return 0;
    }
    return std::max<uint16_t>((uint16_t)position, INT16_MAX);
}

uint16_t ti_GetSize(uint8_t handle) {
    if (validate_File_Handle(handle) == false) {
        return 0;
    }
    fpos_t currentPos;
    fgetpos(ti_File_Handle[handle].file,&currentPos);
    fseek(ti_File_Handle[handle].file, 0, SEEK_END);
    long fileSize = ftell(ti_File_Handle[handle].file);
    fsetpos(ti_File_Handle[handle].file,&currentPos);
    if (fileSize < 0) {
        printf("ftell error: %ld", fileSize);
        return 0;
    }
    return std::max<uint16_t>((uint16_t)fileSize, INT16_MAX);
}

static size_t internal_ti_GetSize(uint8_t handle) {
    if (validate_File_Handle(handle) == false) {
        return 0;
    }
    fpos_t currentPos;
    fgetpos(ti_File_Handle[handle].file,&currentPos);
    fseek(ti_File_Handle[handle].file, 0, SEEK_END);
    long fileSize = ftell(ti_File_Handle[handle].file);
    fsetpos(ti_File_Handle[handle].file,&currentPos);
    if (fileSize < 0) {
        printf("ftell error: %ld", fileSize);
        return 0;
    }
    return (size_t)fileSize;
}

static size_t ram_file_count = 0;
static uint8_t* ram_file[256];

void* ti_GetDataPtr(uint8_t handle) {
    if (validate_File_Handle(handle) == false) {
        return NULL;
    }
    if (ram_file_count >= ARRAY_LENGTH(ram_file)) {
        return NULL;
    }

    size_t read_size = internal_ti_GetSize(handle);
    ram_file[ram_file_count] = (unsigned char*)calloc(read_size + 1, sizeof(unsigned char));
    if (ram_file[ram_file_count] == NULL) {
        return NULL;
    }
    fread(ram_file[ram_file_count], sizeof(unsigned char), read_size, ti_File_Handle[handle].file);
    ram_file_count++;
    printf("ram_file_count: %zu/256\n",ram_file_count);
    if (ram_file_count == 256) {
        PortCE_terminate_fileioc();
        printf("Terminating program, opened too many files.\n");
        exit(-1);
    }
    return (void*)(ram_file[ram_file_count - 1] + 0x4A);
}

void ti_CloseAll(void) {
    for (size_t h = 0; h < ARRAY_LENGTH(ti_File_Handle); h++) {
        ti_Close(h);
    }
}

void PortCE_terminate_fileioc(void) {
    for (size_t h = 0; h < ARRAY_LENGTH(ti_File_Handle); h++) {
        ti_Close(h);
    }
    for (size_t r = 0; r < ARRAY_LENGTH(ram_file); r++) {
        FREE(ram_file[r]);
    }
}

char *ti_Detect(__attribute__((unused)) void **vat_ptr, __attribute__((unused)) const char *detect_string) {
    return NULL;
}

char *ti_DetectVar(__attribute__((unused)) void **vat_ptr, __attribute__((unused)) const char *detect_string, __attribute__((unused)) uint8_t var_type) {
    return NULL;
}

char *ti_DetectAny(__attribute__((unused)) void **vat_ptr, __attribute__((unused)) const char *detect_string, __attribute__((unused)) uint8_t *var_type) {
    return NULL;
}

int ti_IsArchived(__attribute__((unused)) uint8_t handle) {
    return 0;
}

int ti_SetArchiveStatus(__attribute__((unused)) uint8_t archive, __attribute__((unused)) uint8_t handle) {
    return 0;
}

int ti_Delete(__attribute__((unused)) const char *name) {
    return 0;
}
