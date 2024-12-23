#include <iostream>
#include <filesystem>

#include <set>
#include <array>

#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>

#include <QGridLayout>
#include <QPixmap>
#include <QScrollArea>
#include <QListView>
#include <QStandardItemModel>


//#include "../deps/taglib/tpropertymap.h"
//#include "../deps/taglib/tstringlist.h"
//#include "../deps/taglib/tvariant.h"
//#include "../deps/taglib/fileref.h"
//#include "../deps/taglib/tag.h"


#include "TopBar.hpp"
#include "PlaybackBar.hpp"
#include "Player.hpp"
#include "SongList.hpp"

#include "C:/pkg_static/include/taglib/tpropertymap.h"
#include "C:/pkg_static/include/taglib/tstringlist.h"
#include "C:/pkg_static/include/taglib/tvariant.h"
#include "C:/pkg_static/include/taglib/fileref.h"
#include "C:/pkg_static/include/taglib/tag.h"

#include "platform/string_compare_insensitive.h"

namespace fs = std::filesystem;
void print_metadata(const fs::path &path) {
    TagLib::FileRef file_ref(path.c_str());

    if (file_ref.isNull() || !file_ref.file()->isValid()) {
        std::cout << "Could not obtain FileRef for " << path.c_str() << "\n";
        return;
    }
    
    if (file_ref.tag()) {
        TagLib::Tag *tag = file_ref.tag();
        std::cout << "title   - " << tag->title() << "\"" << std::endl;
        std::cout << "artist  - " << tag->artist() << "\"" << std::endl;
        std::cout << "album   - " << tag->album() << "\"" << std::endl;
    }

    std::string file_ext = path.extension().string();
    TagLib::ByteVector img_data;
    if (file_ext == ".opus") {
        std::cout << "found opus file\n";
    }


    if (file_ext == ".flac") {
        std::cout << "found flac file\n";
    }



    if (file_ext == ".ogg") {
        std::cout << "found ogg file\n";
    }



    if (file_ext == ".mp3") {
        std::cout << "found mp3 file\n";
        /*
        if (auto *id3v2Tag = dynamic_cast<TagLib::ID3v2::Tag *>(file_ref.file()->tag())) {
            auto frameList = id3v2Tag->frameList("APIC");
            if (!frameList.isEmpty()) {
                auto *pictureFrame = dynamic_cast<TagLib::ID3v2::AttachedPictureFrame *>(frameList.front());
                if (pictureFrame) {
                    img_data = pictureFrame->picture();
                }
            }
        }
        */
    }



    if (file_ext == ".m4a") {
        std::cout << "found m4a file\n";
    }



    if (file_ext == ".wv") {
        std::cout << "found wv file\n";
    }


    if (file_ext == ".aac") {
        std::cout << "found aac file \n";
    }

    std::cout << "\n";
}

void iterateDirectory(const fs::path &path) {
    if (!fs::exists(fs::absolute(path))) {
        std::cout << "unable to iterate directory " << path << "it does not exist\n";
        return;
    }

    std::cout << "Desired directory exists\n";
    int count_files = 0;
    for (auto &entry : fs::recursive_directory_iterator(path)) {
        fs::path music_path = entry.path();
        if (fs::is_directory(music_path)) {
            continue;
        }
        print_metadata(music_path);
        count_files++;
    }

    std::cout << "\nFinal count of just files is " << count_files << "\n";
}


int main(int argc, char *argv[]) {
    
    std::cout << "taglib version" << TAGLIB_MAJOR_VERSION << TAGLIB_MINOR_VERSION << TAGLIB_PATCH_VERSION <<  "\n";
    QApplication app(argc, argv);
    QMainWindow mainWindow;
    mainWindow.resize(960, 540);
    // mainWindow.resize(1920, 1080);
    mainWindow.setWindowTitle("Player");
    QWidget *centralWidget = new QWidget(&mainWindow);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    MPVPlayer *player = new MPVPlayer(&mainWindow);
    TopBar *topBar = new TopBar(centralWidget);
    PlaybackBar *playbackBar = new PlaybackBar(centralWidget, player);

    //TODO: add more file formats here 
    std::array<std::string, 100> supported_file_ext = {
        //id3 
        ".mp3",
        ".m4a",
        ".aiff",
        //vorbis comment 
        ".opus",
        ".flac",
        ".ogg",

        ".wv",
        ".wvc",
    };

    std::set<std::string>ext_set(supported_file_ext.begin(), supported_file_ext.end());
    
    iterateDirectory("C:/Users/richa/Code_Projects/prototype-music-player/test_music");

    QPixmap thumbnail = QPixmap("../test_images/test_img.jpg");
    QListView *listView = new QListView();
    listView->setViewMode(QListView::IconMode);
    listView->setIconSize(QSize(256, 256));
    listView->setFlow(QListView::LeftToRight);
    listView->setResizeMode(QListView::Adjust);
    listView->setSpacing(15);
    listView->setWrapping(true);


    QStandardItemModel model;
    listView->setModel(&model);

    for (int i = 0; i < 10001; i++) {
        QStandardItem *item = new QStandardItem();
        item->setIcon(QIcon(thumbnail));
        model.appendRow(item);
    }
    


    mainLayout->addWidget(topBar);
    // mainLayout->addWidget(scrollArea);
    mainLayout->addWidget(listView);
    mainLayout->addWidget(playbackBar);

    centralWidget->setLayout(mainLayout);
    mainWindow.setCentralWidget(centralWidget);
    mainWindow.show();

    player->loadSong();
    return app.exec();
}