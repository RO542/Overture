
CREATE TABLE Songs (
    song_id         INT PRIMARY KEY NOT NULL,
    filepath        TEXT NOT NULL,
    title           TEXT NOT NULL,  -- used to create index for searching
    last_modified   INT,
    created         INT, 
    release_year    INT,
    track_number    INT,
    replay_gain     INT DEFAULT 0, -- boolean 
    play_count      INT DEFAULT 0,
    skip_count      INT DEFAULT 0,
    musicbrainz_id  TEXT,
    acoustbrainz_id TEXT,
    thumbnail_id    INT, -- every song has a single thumbnail in metadata
    album_id        INT, -- each song has a single album
    FOREIGN KEY (album_id)     REFERENCES Albums(album_id),
    FOREIGN KEY (thumbnail_id) REFERENCES Thumbnails(thumbnail_id)
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

CREATE TABLE Albums (
    album_id    INT PRIMARY KEY NOT NULL,
    album_name  TEXT NOT NULL,
    thumbnail_id INT,
    release_year INT
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
    xxhash          TEXT UNIQUE NOT NULL
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


CREATE INDEX idx_songs_title  ON  Songs(title);
CREATE INDEX idx_genres_name  ON  Genres(genre_name);
CREATE INDEX idx_artists_name ON  Artists(artist_name);

--TODO:
-- cascades on deletions of songs probably
-- triggers ? 
-- external api data like musicbrainz or chromaprint

-- paths checked on startup fully then periodically to refresh database
CREATE TABLE MusicDirectories (
    directory_id   INT  PRIMARY KEY,
    directory_path TEXT UNIQUE NOT NULL
);

CREATE TABLE UserOptions (
    save_queue_on_exit INT,

);

CREATE TABLE PreviousPlayerState (
    looping     INT, -- boolean 
    volume      INT, -- 0 - 100 %
    shuffle     INT, -- -1, 0, 1 have different meanings 
    query       TEXT, -- if the search bar was filled re-run it's last search
    song_time   INT,
    song_id     INT, -- song the user left off on
    FOREIGN KEY (song_id) REFERENCES Songs(song_id)
);

-- TODO: save and restore the state of  the playback queue 
