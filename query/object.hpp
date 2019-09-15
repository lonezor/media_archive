#pragma once

#include <iostream>
#include <stdint.h>

typedef enum {
    OBJECT_TYPE_IMAGE,
    OBJECT_TYPE_AUDIO,
    OBJECT_TYPE_VIDEO,
} object_type_t;

typedef enum {
    THUMBNAIL_SIZE_160 = 160,
    THUMBNAIL_SIZE_320 = 320,
    THUMBNAIL_SIZE_640 = 640,
    THUMBNAIL_SIZE_1024 = 1024,
    THUMBNAIL_SIZE_2048 = 2048,
    THUMBNAIL_SIZE_4096 = 4096,
    THUMBNAIL_SIZE_8192 = 8192,
} thumbnail_size_t;

typedef enum {
    THUMBNAIL_QUALITY_LOW,
    THUMBNAIL_QUALITY_HIGH,
} thumbnail_quality_t;

typedef struct {
  std::string artist;
  std::string album;
  std::string title;
  std::string duration;
  std::string genre;
  std::string producer;
  int total_discs;
  int total_tracks;
  int disc;
  int track;
  int total_timestamps;
} audio_metadata_t;

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
void set_audio_metadata(std::string artist,
                                std::string album,
                                std::string title,
                                std::string duration,
                                std::string genre,
                                std::string producer,
                                int total_discs,
                                int total_tracks,
                                int disc,
                                int track,
                                int total_timestamps);
    
    audio_metadata_t audio_metadata;

private:
    uint32_t obj_id_;
    std::string sha1_;
    std::string suffixes_;
    uint32_t access_counter_;
    object_type_t object_type_;
};