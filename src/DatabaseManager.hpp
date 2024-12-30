#include <filesystem>
#include <unordered_map>
#include <chrono>
#include <assert.h>
#include <sqlite3/sqlite3.h>


#include "TagExtractor.hpp"
#include "FileExtensions.hpp"
#include "platform/char_encoding.hpp"


#ifndef DB_MANAGER_H_
#define DB_MANAGER_H_



namespace fs = std::filesystem;
class DatabaseManager {

public:

    std::unordered_map<std::string, sqlite3_int64>artists_map;
    std::unordered_map<std::string, sqlite3_int64>albums_map;
    std::unordered_map<std::string, sqlite3_int64>genres_map;

    sqlite3 *db = nullptr;
    int sq_err;

    struct SQLStatements {
        sqlite3_stmt* insertSong;
        sqlite3_stmt* insertArtist;
        sqlite3_stmt* insertSongsArtists;
        sqlite3_stmt* insertAlbum;
        sqlite3_stmt* insertSongsAlbums;
        sqlite3_stmt* insertGenre;
        sqlite3_stmt* insertSongsGenres;
        //TODO:
        sqlite3_stmt* insertThumbnail;
        sqlite3_stmt *selectSong;
        sqlite3_stmt *selectAlbum;
        sqlite3_stmt *selectGenre;
        sqlite3_stmt *selectArtist;
    };
    struct SQLStatements stmts;





    DatabaseManager(const fs::path &db_path) {
        if (!fs::exists(db_path)) {
            std::cout << "sqlite database path " << db_path << "DOES NOT EXIST"; 
            assert(false);
        }
        if (sqlite3_open(db_path.string().c_str(), &db) != SQLITE_OK) {
            std::cout << "unable to sqlite3_open database\n"; 
            assert(false);
        } 
        if (!prepareStatements()) {
            assert(false);
        }
    }

    DatabaseManager() {
        if (db) {
            //TODO: finalize all statements
            sqlite3_close(db);
        }
        db = nullptr;
        std::cout << "query manager destructor\n";
    }


    bool prepareStatements() {
        std::cout << "preparing sqlite statements .... \n";

        const char* insertSongs = "INSERT INTO Songs (URI, is_downloaded, last_modified, title, track, year) VALUES (?, ?, ?, ?, ?, ?);";
        std::vector<bool> prepared_statements {
            checkSqliteErr("prepare insert Songs ",      sqlite3_prepare_v2(db, insertSongs, -1, &stmts.insertSong, NULL)),
            checkSqliteErr("prepare insert Artists",     sqlite3_prepare_v2(db, "INSERT INTO Artists(artist_name) VALUES (?);", -1, &stmts.insertArtist, NULL)),
            checkSqliteErr("prepare insert Albums",      sqlite3_prepare_v2(db, "INSERT INTO Albums(album_name) VALUES(?);", -1, &stmts.insertAlbum, NULL)),
            checkSqliteErr("prepare insert Genres",      sqlite3_prepare_v2(db, "INSERT INTO Genres(genre_name) VALUES(?);", -1, &stmts.insertGenre, NULL)),
            checkSqliteErr("prepare insert Thumbnails",  sqlite3_prepare_v2(db, "INSERT INTO Thumbnails(thumbnail_path) VALUES(?);", -1, &stmts.insertThumbnail, NULL)),
            checkSqliteErr("prepare insert SongsGenres", sqlite3_prepare_v2(db, "INSERT INTO SongsGenres VALUES(?, ?);" , -1, &stmts.insertSongsGenres, NULL)),
            checkSqliteErr("prepare insert SongsAlbums", sqlite3_prepare_v2(db, "INSERT INTO SongsAlbums VALUES(?, ?);" , -1, &stmts.insertSongsAlbums, NULL)), 
            checkSqliteErr("prepare insert SongsArtist", sqlite3_prepare_v2(db, "INSERT INTO SongsArtists VALUES(?, ?);", -1, &stmts.insertSongsArtists, NULL))
        };
        for (int i = 0; i < prepared_statements.size(); i++) {
            bool prepare_result = prepared_statements[i];
            if (!prepare_result) {
                std::cout << "Unable to prepare all statements, check the statement at " << i << " in prepareStatements() function\n";
                return false;
            }
        }
        std::cout << "Prepared all statements for sqlite \n";
        return true;
    }

    bool checkSqliteErr(const std::string &attempt_msg, int err) {
        if (err != SQLITE_OK) {
            std::cout << "ERROR DURING: " << attempt_msg << "\n";
            std::cout << "sqlite3 last error :" << sqlite3_errmsg(db) << "\n";
            std::cout << "\n";
            return false;
        }
        return true;
    }
    //TODO: iterate multiple music directories
    void iterateMusicDirs() {
        fs::path music_dir = fs::absolute("../test_music");
        if (!fs::exists(music_dir)) {
            // std::cout << " Unable to find music directory\n";
            assert(false);
        }

        sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL);
        for (const fs::directory_entry &dir_entry : fs::recursive_directory_iterator(music_dir)) {
            fs::path p = dir_entry.path();
            if (!fs::is_regular_file(p)) {
                continue;
            }
            if (p.has_extension()) {
                std::string ext_string = p.extension().string();
                if (ext_set.find(ext_string) == ext_set.end()) {
                    // std::cout << p.filename().u8string() << "is being skipped for indexing, it is not a music file\n";
                    continue;
                }
            }
            // std::cout << "Adding file " << dir_entry.path().filename().u8string() << "\n";
            extractSongMetadataToDB(dir_entry);
        }
        sqlite3_exec(db, "COMMIT;", NULL, NULL, NULL);
    }

    //TODO: map integration, and return values -1 error, 0 for no db change, 1 for successful db put
    void extractSongMetadataToDB(const fs::directory_entry &dir_entry) {
        fs::path filepath = dir_entry.path();
        auto native_path_str = path_to_native_string(filepath);

        TagLib::FileRef fileref(native_path_str.c_str());
        if (fileref.isNull() || !fileref.tag()) {
            std::cout << "Failed to create FileRef for \n";
            print_native_path(native_path_str);
            return;
        }

        TagLib::Tag *basic_tags = fileref.tag();
        if (!basic_tags || basic_tags->isEmpty()) {
            std::cout << " Tags dont't exist or simply empty, skipping ....\n";
            return;
        }

        auto ftime = fs::last_write_time(filepath);
        auto epoch_time = std::chrono::duration_cast<std::chrono::seconds>(ftime.time_since_epoch());
        auto last_modified = epoch_time.count();

        int track_num = basic_tags->track();
        int year =  basic_tags->year();
        TagLib::String title = basic_tags->title().stripWhiteSpace();
        TagLib::String artist = basic_tags->artist().stripWhiteSpace();
        TagLib::String album = basic_tags->album().stripWhiteSpace();
        TagLib::String genre = basic_tags->genre().stripWhiteSpace();
        const char *URI  = filepath.generic_u8string().c_str();

        auto e = filepath.u8string();
        auto s = std::string(reinterpret_cast<char const *>(e.data()), e.size());
        


        //TODO: insert text child table data into the map as we go along 

        checkSqliteErr("insertSongs bind URI ", sqlite3_bind_text(
           stmts.insertSong,
           1,
           s.c_str(),
           -1,
           SQLITE_TRANSIENT)
        );
        checkSqliteErr("insertSong bind is_downloaded", sqlite3_bind_int(stmts.insertSong, 2, 1));
        checkSqliteErr("insertSongs bind last_modified", sqlite3_bind_int(stmts.insertSong,  3, last_modified));
        checkSqliteErr("insertSongs bind song title", sqlite3_bind_text(
            stmts.insertSong,
            4,
            title.toCString(),
            -1,
            SQLITE_STATIC)
        );
        checkSqliteErr(
            "insertSong binding Track ",
            track_num 
              ? sqlite3_bind_int(stmts.insertSong,  5, track_num)
              : sqlite3_bind_null(stmts.insertSong, 5)
        );
        checkSqliteErr("insertSong binding Year", sqlite3_bind_int(stmts.insertSong,  6, year));

        checkSqliteErr("execute insertSongs",     sqlite3_step(stmts.insertSong));
        checkSqliteErr("reset insertSongs",       sqlite3_reset(stmts.insertSong));
    }
    

    sqlite3_int64 getArtistId(const std::string &artist);
    sqlite3_int64 getAlbumId(const std::string &album_name);
    sqlite3_int64 getGenreId(const std::string &genre_name);
};


#endif // DB_MANAGER_H_