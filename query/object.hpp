#pragma once

#include <iostream>
#include <stdint.h>

typedef enum {
    OBJECT_TYPE_VISUAL,
    OBJECT_TYPE_AUDIO,
} object_type_t;

typedef enum {
    THUMBNAIL_SIZE_160,
    THUMBNAIL_SIZE_320,
    THUMBNAIL_SIZE_640,
    THUMBNAIL_SIZE_1024,
    THUMBNAIL_SIZE_2048,
    THUMBNAIL_SIZE_4096,
    THUMBNAIL_SIZE_8192,
} thumbnail_size_t;

typedef enum {
    THUMBNAIL_QUALITY_LOW,
    THUMBNAIL_QUALITY_HIGH,
} thumbnail_quality_t;

class object {
public:
    object();
    object(uint32_t obj_id, std::string sha1, std::string suffixes, uint32_t access_counter);

    void determine_object_type();

    thumbnail_size_t get_highest_thumbnail_size();

    uint32_t      get_obj_id();
    std::string   get_sha1();
    std::string   get_suffixes();
    uint32_t      get_access_counter();
    object_type_t get_object_type();


    std::string get_audio_path();  
    std::string get_thumbnail_path(thumbnail_size_t size, thumbnail_quality_t quality);  

private:
    uint32_t obj_id_;
    std::string sha1_;
    std::string suffixes_;
    uint32_t access_counter_;
    object_type_t object_type_;
};