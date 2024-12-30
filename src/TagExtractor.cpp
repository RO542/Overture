
#define STB_IMAGE_IMPLEMENTATION

#define STB_IMAGE_RESIZE_IMPLEMENTATION

#define STB_IMAGE_WRITE_IMPLEMENTATION

#define XXH_IMPLEMENTATION
#define XXH_STATIC_LINKING_ONLY
#include "TagExtractor.hpp"

//NOTE: stbi_uc is just unsigned char 

void downsizeThumbnailAndSave(const TagLib::ByteVector& inputData) {
    int in_width, in_height, channels;
    const stbi_uc *inputBytes = (const stbi_uc *)inputData.data();
    if (!inputBytes) {
        std::cout << "input images could not be converted to u8 bytes \n";
        return;
    }

    stbi_uc *raw_img_bytes = stbi_load_from_memory(inputBytes, inputData.size(), &in_width, &in_height, &channels, 0);
    if (!raw_img_bytes) {
        std::cout << "Unable to use stbi_load_from_memory for this TagLib::ByteVector\n"; 
        return;
    } 
    unsigned char* resized_bytes = stbir_resize_uint8_linear(
        raw_img_bytes, in_width, in_height, 0,
        NULL, 256, 256, 0,
        (stbir_pixel_layout)(channels)
    );
    if (!resized_bytes) {
        std::cout << "Unable to perform stb image resize \n";
        stbi_image_free(raw_img_bytes);
        return;
    }

    char string_hash_buffer[128];
    memset(string_hash_buffer, 0, 128);
    //FIXME: is this use of strnlen right ?
    uint64_t thumbnail_xxhash = XXH64(resized_bytes, (256 * 256 * channels, 128), 0);
    snprintf(string_hash_buffer, 128, "%llu", thumbnail_xxhash);

    std::string thumbnail_filename = string_hash_buffer; 
    thumbnail_filename += ".jpg";

    std::string thumbnail_filepath = img_cache_dir + thumbnail_filename ;
    int write_result = stbi_write_jpg(thumbnail_filepath.c_str(), 256, 256, channels, resized_bytes, 100);
    std::cout << "Saving " << thumbnail_filename  << " To " << thumbnail_filepath <<  "\n";
    std::cout << "RESULT ?  " <<  (write_result == 1 ? "SUCCESSS" : "FAILURE") << "\n";
    std::cout << std::endl;

    stbi_image_free(raw_img_bytes);
    free(resized_bytes);
}


/*
int extractMetadataToDB(
    fs::path filepath
    //TODO: add a reference to the Query Manger here
) {
    TagLib::FileRef fileref(filepath.c_str());
    if (fileref.isNull() || !fileref.tag()) {
        std::cout << "unable to parse fileref\n";
        return -1;
    } 
    TagLib::Tag *basic_tags = fileref.tag();
    if (!basic_tags || basic_tags->isEmpty()) {
        std::cout << "Tag doesn't exist or is empty, skipping ....";
        return -1;
    }
    //TODO: consider making all strings lowercase here
    TagLib::String title = basic_tags->title().stripWhiteSpace();
    TagLib::String artist = basic_tags->artist().stripWhiteSpace();
    TagLib::String album = basic_tags->album().stripWhiteSpace();
    TagLib::String genre = basic_tags->genre().stripWhiteSpace();
    // unsigned int track_num = basic_tags->track();
    // unsigned int year =  basic_tags->year();


    bool found_cover_art = false;
    TagLib::ByteVector cover_art;
    if (fileref.complexPropertyKeys().contains("PICTURE")) {
        const TagLib::List<TagLib::VariantMap>picture_list_maps = fileref.complexProperties("PICTURE");
        for (const auto &property_map : picture_list_maps) {
            for (auto &[prop_name, prop_value] : property_map) {
                if (prop_value.type() == TagLib::Variant::ByteVector) {
                    cover_art = prop_value.value<TagLib::ByteVector>();
                    found_cover_art = true;
                }
            }
        }
    }

    return 1;
}
*/
/*
void buildMusicCache(const fs::path &music_dir_path, sqlite3 *db) {
    if (!fs::exists(music_dir_path)) {
        std::cout << "in_path music directory " << music_dir_path << " does not exist\n";
        return;
    }
    // map string -> sqlite primary key int
    std::map<std::string, sqlite3_int64>artists_map;
    std::map<std::string, sqlite3_int64>albums_map;
    std::map<std::string, sqlite3_int64>genres_map;

    for (const auto &dir_entry : fs::recursive_directory_iterator(music_dir_path)) {
        std::string file_ext = dir_entry.path().extension().string();
        if (ext_set.find(file_ext) == ext_set.end()) {
            std::cout << "Skipping... " << dir_entry << "   is not a music file based on it's extension " << file_ext << "\n";
            continue;
        }
        int extract_result = extractMetadataToDB(
            fs::absolute(dir_entry.path()),
            artists_map,
            albums_map,
            genres_map,
            db
        );

    }


    //TODO: batch metadata extraction
    // char *err;
    // int begin_transact_result = sqlite3_exec(db, "BEGIN TRANSCATION", NULL , NULL, &err);
    // sqlite3_free(&err);
    // int end_transact_result = sqlite3_exec(db, "COMMIT;", NULL , NULL, &err);
    // sqlite3_free(&err);
}
*/