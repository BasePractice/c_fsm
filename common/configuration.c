#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <u_memory.h>
#include <c_logger.h>
#include <cJSON.h>
#include <sha3.h>
#include "common.h"
#include "configuration.h"

#if defined(WIN32)
#else
#include <dirent.h>
#include <unistd.h>
#endif

#ifndef MAX_PATH
#define MAX_PATH 512
#endif

bool
configuration_read_file(const char *filename, struct ConfigurationReader *reader) {
    FILE *fd = fopen(filename, "r");
    bool ret = false;
    if (fd != 0) {
        size_t file_size;
        size_t read = 0;
        size_t reading = 0;
        char *buffer;

        fseek(fd, 0L, SEEK_END);
        file_size = (size_t) ftell(fd);
        rewind(fd);
        buffer = calloc(file_size, 1);
        while (reading < file_size && (read = fread(buffer + reading, 1, file_size - reading, fd) > 0)) {
            reading += (read * (file_size - reading));
        }
        assert(reading == file_size);
        fclose(fd);
        ret = configuration_read_memory(buffer, file_size, reader);
        free(buffer);
    }
    return ret;
}

bool
configuration_read_memory(const char *text, size_t size, struct ConfigurationReader *reader) {
    cJSON *parsed = NULL;
    if (reader == 0)
        return false;
    parsed = cJSON_Parse(text);
    if (parsed != NULL) {
        cJSON *item;

        cJSON_ArrayForEach(item, parsed) {
            if (cJSON_IsString(item) && strcmp(item->string, "version") == 0) {
                if (reader->read_version != 0) {
                    (*reader->read_version)(reader->ud, item->valuestring);
                } else {
                    log_warn("[configuration] Skipped property %s\n", item->string);
                }
            } else if (cJSON_IsArray(item)) {
                int map_size = cJSON_GetArraySize(item);
                int i;

                if (reader->read_matrix_int_allocate == 0 || reader->read_matrix_int_put == 0)
                    continue;
                (*reader->read_matrix_int_allocate)(reader->ud, item->string, map_size);
                for (i = 0; i < map_size; ++i) {
                    cJSON *line = cJSON_GetArrayItem(item, i);
                    if (cJSON_IsArray(line)) {
                        int line_size = cJSON_GetArraySize(line);
                        int j;

                        for (j = 0; j < line_size; ++j) {
                            cJSON *element = cJSON_GetArrayItem(line, j);
                            (*reader->read_matrix_int_put)(reader->ud, item->string, i, j, element->valueint);
                        }
                    }                    
                }
            } else if (cJSON_IsObject(item)) {
                cJSON *enable = cJSON_GetObjectItemCaseSensitive(item, "enable");
                if (cJSON_HasObjectItem(item, "value")) {
                    if (reader->read_number != 0) {
                        cJSON *value = cJSON_GetObjectItemCaseSensitive(item, "value");
                        log_debug("[configuration] %s, value = [%d : %f], enable = %s",
                                  item->string, value->valueint, value->valuedouble, cJSON_IsTrue(enable) ? "true" : "false");
                        (*reader->read_number)(reader->ud, item->string, value->valueint, value->valuedouble,
                                               cJSON_IsTrue(enable));
                    } else {
                        log_warn("[configuration] Skipped property %s\n", item->string);
                    }
                } else {
                    cJSON *minimum = cJSON_GetObjectItemCaseSensitive(item, "minimum");
                    cJSON *maximum = cJSON_GetObjectItemCaseSensitive(item, "maximum");
                    if (reader->read_range != 0 && maximum != 0 && minimum != 0) {
                        log_debug("[configuration] %s,  minimum = %f, maximum = %f, enable = %s",
                                  item->string, minimum->valuedouble, maximum->valuedouble, cJSON_IsTrue(enable) ? "true" : "false");
                        (*reader->read_range)(reader->ud, item->string, minimum->valueint, minimum->valuedouble,
                                              maximum->valueint, maximum->valuedouble, cJSON_IsTrue(enable));

                    } else {
                        if (reader->read_object != 0) {
                            cJSON *sub_item;
                            cJSON_ArrayForEach(sub_item, item) {
                                (*reader->read_object)(reader->ud,
                                                       item->string, sub_item->string, sub_item->valuestring);
                            }
                        } else {
                            log_warn("[configuration] Skipped property %s\n", item->string);
                        }
                    }
                }
            } else {
                log_warn("[configuration] Unsupported property %s\n", item->string);
            }
        }
        
        if (reader->hash) {
#define DIGEST_LEN 16
            uint8_t digest_result[DIGEST_LEN] = {0};
            char hex_result[ (DIGEST_LEN * 2) + 2] = {0};
            size_t hex_result_size = sizeof(hex_result);
            char *unformatted;

            cJSON_DeleteItemFromObject(parsed, "root");
            unformatted = cJSON_PrintUnformatted(parsed);
            sha3(unformatted, strlen(unformatted), digest_result, DIGEST_LEN);
            binary_to_hex(digest_result, DIGEST_LEN, hex_result, &hex_result_size);
            (*reader->hash)(reader->ud, hex_result, hex_result_size);
            log_debug("[configuration] configuration hash %s", hex_result);
            cJSON_free(unformatted);
#undef DIGEST_LEN
        }
        
        cJSON_Delete(parsed);
    } else {
        return false;
    }
    return true;
}

#if defined(WIN32)

#include <windows.h>

#else
#include <dirent.h>
#endif


char *configuration_cwd() {
    static char current_dir[MAX_PATH * 2] = {0};
    unsigned long current_dir_size = 0;

    if (current_dir[0] != 0)
        return current_dir;

#if defined(WIN32)
    current_dir_size = GetCurrentDirectoryA(sizeof(current_dir), current_dir);
    current_dir[current_dir_size] = 0;
#else
    getcwd(current_dir, MAX_PATH);
#endif
    return current_dir;
}
