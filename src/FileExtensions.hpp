
#include <unordered_set>
#include <string>
//TODO: consider adding more obscure extensions later
//TODO: also maybe these should be std::filesystem::path::extension rather than std::string
//check later
const std::unordered_set<std::string>ext_set({
    ".mp3",
    ".m4a",
    ".alac",
    ".aiff",
    ".opus",
    ".flac",
    ".ogg",
    ".wv",
    ".wav",
    ".wvc",
    ".wma",
    ".ape",
    ".aac",
});
