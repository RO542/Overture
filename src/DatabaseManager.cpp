#include "DatabaseManager.hpp"

sqlite3_int64 DatabaseManager::getArtistId(const std::string &artist){
    auto artist_kv = artists_map.find(artist);
    if (artist_kv == artists_map.end()) {
        sqlite3_bind_text(stmts.insertArtist, 1, artist.c_str(), -1,  SQLITE_TRANSIENT);
        sqlite3_step(stmts.insertArtist);
        int64_t rowid = sqlite3_last_insert_rowid(db);
        artists_map[artist] = rowid;
        return rowid;
    }
    return artist_kv->second;
}

sqlite3_int64 DatabaseManager::getAlbumId(const std::string &album_name) {
    auto album_kv = albums_map.find(album_name);
    if (album_kv == albums_map.end()) {
        std::cout << "adding new album: " << album_name << " to database\n";
        sqlite3_bind_text(stmts.insertAlbum, 1, album_name.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_step(stmts.insertAlbum);
        sqlite3_int64 album_id = sqlite3_last_insert_rowid(db);
        albums_map[album_name] = album_id;
        return album_id;
    }
    return album_kv->second;
    
}

sqlite3_int64 DatabaseManager::getGenreId(const std::string &genre_name) {
    auto genre_kv = genres_map.find(genre_name);
    if (genre_kv == genres_map.end()) {
        std::cout << "Adding new genre " << genre_name << "to genres_map \n";
        sqlite3_bind_text(stmts.insertGenre, 1, genre_name.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_step(stmts.insertGenre);
        sqlite3_int64 genre_id = sqlite3_last_insert_rowid(db);
        genres_map[genre_name] = genre_id;
        return genre_id;
    }
    return genre_kv->second;
}