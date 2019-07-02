#include "object.hpp"

#include <string.h>

object::object() {
    
}

 object::object(uint32_t obj_id, std::string sha1, std::string suffixes, uint32_t access_counter)
 {
     obj_id_ = obj_id;
     sha1_ = sha1;
     suffixes_ = suffixes;
     access_counter_ = access_counter;

     determine_object_type();
 }

void object::determine_object_type()
{
    if (strstr(suffixes_.c_str(), ".flac")) {
        object_type_ = OBJECT_TYPE_AUDIO;
    } else {
        object_type_ = OBJECT_TYPE_VISUAL;
    }
}

std::string object::get_audio_path()
{
    return sha1_ + ".flac";
}

std::string object::get_thumbnail_path(thumbnail_size_t size, thumbnail_quality_t quality)
{
    const char* file_ending = "";
    const char* thumbnail_size = "";

    // Thumbnail size
    switch(size) {
        case THUMBNAIL_SIZE_160:
            thumbnail_size = "_0160";
            break;
        case THUMBNAIL_SIZE_320:
            thumbnail_size = "_0320";
            break;
        case THUMBNAIL_SIZE_640:
            thumbnail_size = "_0640";
            break;
        case THUMBNAIL_SIZE_1024:
            thumbnail_size = "_1024";
            break;
        case THUMBNAIL_SIZE_2048:
            thumbnail_size = "_2048";
            break;
        case THUMBNAIL_SIZE_4096:
            thumbnail_size = "_4096";
            break;
        case THUMBNAIL_SIZE_8192:
            thumbnail_size = "_8192";
            break;
    }

    // File ending
    switch(size) {
        case THUMBNAIL_SIZE_160:
        case THUMBNAIL_SIZE_320:
        case THUMBNAIL_SIZE_640:
            if (quality == THUMBNAIL_QUALITY_LOW) {
                file_ending  = "_q1.png";
            } else {
                file_ending  = "_q2.jpg";
            }
            break;
        case THUMBNAIL_SIZE_1024:
        case THUMBNAIL_SIZE_2048:
        case THUMBNAIL_SIZE_4096:
        case THUMBNAIL_SIZE_8192:
            file_ending = ".jpg";
            break;
    }

    return sha1_ + std::string(thumbnail_size) + std::string(file_ending);
}

thumbnail_size_t object::get_highest_thumbnail_size()
{
    thumbnail_size_t size = THUMBNAIL_SIZE_160;

     if (strstr(suffixes_.c_str(), "0320")) {
        size = THUMBNAIL_SIZE_320; 
     }

     if (strstr(suffixes_.c_str(), "0640")) {
         size = THUMBNAIL_SIZE_640; 
     }

     if (strstr(suffixes_.c_str(), "1024")) {
         size = THUMBNAIL_SIZE_1024; 
     }

     if (strstr(suffixes_.c_str(), "2048")) {
         size = THUMBNAIL_SIZE_2048; 
     }

     if (strstr(suffixes_.c_str(), "4096")) {
         size = THUMBNAIL_SIZE_4096; 
     }

     if (strstr(suffixes_.c_str(), "8192")) {
         size = THUMBNAIL_SIZE_8192; 
     }

    return size;
}

    uint32_t object::get_obj_id()
    {
        return obj_id_;
    }

    std::string object::get_sha1()
    {
        return sha1_;
    }

    std::string object::get_suffixes()
    {
        return suffixes_;
    }

    uint32_t object::get_access_counter()
    {
        return access_counter_;
    }

    object_type_t object::get_object_type()
    {
        return object_type_;
    }