--DROP TABLE IF EXISTS Songs;
--DROP TABLE IF EXISTS Artists;
--DROP TABLE IF EXISTS Albums;
--DROP TABLE IF EXISTS Genres;
--DROP TABLE IF EXISTS Thumbnails;
--DROP TABLE IF EXISTS SongsArtists;
--DROP TABLE IF EXISTS SongsGenres;
--DROP TABLE IF EXISTS PlaylistsSongs;
--DROP TABLE IF EXISTS Playlists;
--DROP TABLE IF EXISTS AlbumsArtists;
--DROP TABLE IF EXISTS AlbumsGenres;

--DROP TABLE IF EXISTS SongsFTS;
--DROP TABLE IF EXISTS UserOptions;
--DROP TABLE IF EXISTS PreviousPlayerState;
--DROP TABLE IF EXISTS MusicDirectories;


CREATE TABLE Songs (
    song_id         INT PRIMARY KEY,
    URI             TEXT NOT NULL,
    is_downloaded   INT,
    last_modified   INT,
    title           TEXT NOT NULL,
    track           INT, 
    year            INT,

    server_id       INT DEFAULT NULL
);

CREATE TABLE SongsArtists (
    song_id   INT,
    artist_id INT,
    PRIMARY KEY (song_id, artist_id),
    FOREIGN KEY (song_id)   REFERENCES Songs(song_id),
    FOREIGN KEY (artist_id) REFERENCES Artists(artist_id)
);


CREATE TABLE SongsArtists (
    song_id   INT,
    artist_id INT,
    PRIMARY KEY (song_id, artist_id),
    FOREIGN KEY (song_id)   REFERENCES Songs(song_id),
    FOREIGN KEY (artist_id) REFERENCES Artists(artist_id)
);

CREATE TABLE SongsGenres (
    song_id  INT,
    genre_id INT,
    PRIMARY KEY (song_id, genre_id),
    FOREIGN KEY (song_id)   REFERENCES Songs(song_id),
    FOREIGN KEY (genre_id)  REFERENCES Genres(genre_id)
);


CREATE TABLE SongsAlbums(
    song_id  INT,
    album_id INT,
    PRIMARY KEY (song_id, album_id ),
    FOREIGN KEY (song_id)   REFERENCES Songs(song_id),
    FOREIGN KEY (album_id)  REFERENCES Albums(album_id)
);

CREATE TABLE Albums (
    album_id    INT PRIMARY KEY,
    album_name  TEXT NOT NULL,
    thumbnail_id INT,
    release_year INT DEFAULT NULL,
    FOREIGN KEY (thumbnail_id) REFERENCES Thumbnails(thumbnail_id)
);

CREATE TABLE AlbumsGenres (
    album_id INT,
    genre_id INT,
    PRIMARY KEY (album_id, genre_id),
    FOREIGN KEY (album_id) REFERENCES Albums(album_id),
    FOREIGN KEY (genre_id) REFERENCES Genres(genre_id)
);

CREATE TABLE AlbumsArtists (
    album_id  INT,
    artist_id INT,
    PRIMARY KEY (album_id, artist_id), 
    FOREIGN KEY (album_id)  REFERENCES Albums(album_id),
    FOREIGN KEY (artist_id) REFERENCES Artists(artist_id)
);

CREATE TABLE Artists (
    artist_id   INT  PRIMARY KEY,
    artist_name TEXT NOT NULL
);

CREATE TABLE Genres (
    genre_id PRIMARY KEY NOT NULL,
    genre_name TEXT NOT NULL
);

CREATE TABLE Thumbnails (
    thumbnail_id    INT PRIMARY KEY,
    thumbnail_path  TEXT NOT NULL,
    hash            TEXT UNIQUE NOT NULL
);

CREATE TABLE Playlists (
    playlist_id   INT PRIMARY KEY,
    playlist_name TEXT
);

CREATE TABLE PlaylistsSongs (
    playlist_id INT,
    song_id     INT,
    PRIMARY KEY (playlist_id, song_id),
    FOREIGN KEY (playlist_id) REFERENCES Playlists(playlist_id),
    FOREIGN KEY (song_id) REFERENCES Songs(song_id)
);


CREATE VIRTUAL TABLE SongsFTS USING fts5(
    title,
    artist_name, 
    album_name,
    genre_name,
    content='',
    tokenize="trigram"
);



CREATE TRIGGER insert_songs_fts AFTER INSERT ON Songs 
BEGIN
    INSERT INTO SongsFTS(rowid, title, artist_name, album_name, genre_name)
    VALUES(
        new.song_id,
        new.title,
        SELECT artist_name FROM Artists WHERE artist_id = (SELECT artist_id FROM SongsArtists WHERE SongsArtists(song_id) = new.song_id),
        SELECT album_name FROM Albums WHERE album_id = (SELECT album_id FROM SongsAlbums WHERE SongsAlbums(song_id) = new.song_id),
        SELECT genre_name FROM Genres WHERE genre_id = (SELECT genre_id FROM SongsGenres WHERE SongsGenres(song_id) = new.song_id)
    );
END;

-- CREATE TRIGGER insert_songs_fts AFTER INSERT ON Songs 
-- BEGIN
--     INSERT INTO SongsFTS(rowid, title, artist_name, album_name, genre_name)
--     VALUES(
--         new.song_id,
--         new.title,
--         (SELECT artist_name from Artists WHERE artist_id = new.artist_id),
--         (SELECT album_name from Albums WHERE album_id = new.album_id),
--         (SELECT genre_name from Genres WHERE genre_id = new.genre_id)
--     );
-- END;

--TODO: check if this even matters 
-- CREATE INDEX idx_album_id  ON Albums(album_id);
-- CREATE INDEX idx_artist_id ON Artists(artist_id);
-- CREATE INDEX idx_genre_id  ON Genres(genre_id);
-- CREATE INDEX idx_thumbnail_id ON Thumbnails(thumbnail_id);

/*  Possible bulk building of songs fts table
INSERT INTO SongsFTS(rowid, title, artist_name, album_name, genre_name)
SELECT 
    song_id, 
    title, 
    (SELECT artist_name FROM Artists WHERE artist_id = Songs.artist_id),
    (SELECT album_name FROM Albums WHERE album_id = Songs.album_id),
    (SELECT genre_name FROM Genres WHERE genre_id = Songs.genre_id)
FROM Songs;
*/


--TODO: add trigger for update case 
--TODO: VACUUM AND OPTIMIZE FTS5 tables periodically

CREATE TRIGGER delete_songs_fts AFTER DELETE ON Songs
BEGIN
    DELETE FROM SongsFTS WHERE rowid = old.song_id;
END;

CREATE TABLE MusicDirectories (
    directory_id   INT  PRIMARY KEY,
    directory_path TEXT UNIQUE NOT NULL
);

--TODO: add other user options as needed
CREATE TABLE UserOptions (
    save_queue_on_exit INT,
    save_player_state_on_exit INT
);

CREATE TABLE PreviousPlayerState (
    looping     INT,
    volume      INT,
    shuffle     INT,
    query       TEXT,
    song_time   INT default NULL,
    song_id     INT default NULL,
    FOREIGN KEY (song_id) REFERENCES Songs(song_id)
);
--TODO: Directory might need to be it's own entity too since we have to handle the case that from their settings
-- a user deletes a directory so all Songs inside it and references to them have to be deleted
--TODO: technically an album might also have a year or multiple artists as well we ignore this for now 