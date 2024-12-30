#debug build of sqlite3 with fts for windows, must be run from Developer Powershell
# NOTE: in the release build you need to set -DSQLITE_RELEASE, this will be set later
sqlite3_debug:
	cl -Zi -DSQLITE_DEBUG -DSQLITE_THREADSAFE=1 -DSQLITE_ENABLE_FTS5 /MDd sqlite3.c -c -Fo:sqlite3.obj
	lib -out:sqlite3.lib sqlite3.obj