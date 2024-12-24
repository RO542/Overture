#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdio>
#include <iomanip>

// #include <set>
// #include <array>

// #include <QApplication>
// #include <QMainWindow>
// #include <QWidget>
// #include <QVBoxLayout>

// #include <QGridLayout>
// #include <QPixmap>
// #include <QScrollArea>
// #include <QListView>
// #include <QStandardItemModel>


// #include "PlaybackBar.hpp"
// #include "Player.hpp"
// #include "SongList.hpp"
#include "taglib/tpropertymap.h"
#include "taglib/tstringlist.h"
#include "taglib/tvariant.h"
#include "taglib/fileref.h"
#include "taglib/tag.h"

namespace fs = std::filesystem;

void img_extractor(const std::string &path) {
    TagLib::FileRef fileref(path.c_str());

    if (fileref.isNull() || !fileref.tag()) {
        std::cout << "unable to parse fileref\n";
        return;
    } 

    TagLib::Tag *basic_tags = fileref.tag();
    if (!basic_tags || basic_tags->isEmpty()) {
        std::cout << "Tag doesn't exist or is empty, skipping ....";
        return;
    }

    std::cout << "BASIC TAGS\n";
    std::cout << basic_tags->artist()  << "\n";
    std::cout << basic_tags->album()  << "\n";
    std::cout << basic_tags->genre()  << "\n";
    std::cout << basic_tags->year()  << "\n";

    for (const auto &complexPropKey : fileref.complexPropertyKeys()) {
        const auto properties = fileref.complexProperties(complexPropKey);
        for (const auto &property : properties) {
            std::cout << "complexKey    " << complexPropKey << "\n";
            for(const auto &[key, value] : property) {
                std::cout << "  " << std::left << std::setfill(' ') << std::setw(11) << key << " - ";
                if(value.type() == TagLib::Variant::ByteVector) {
                    std::cout << "(" << value.value<TagLib::ByteVector>().size() << " bytes)" << std::endl;
                    std::ofstream picture;
                    TagLib::String fn(path);
                    int slashPos = fn.rfind('/');
                    int dotPos = fn.rfind('.');
                    if(slashPos >= 0 && dotPos > slashPos) {
                        fn = fn.substr(slashPos + 1, dotPos - slashPos - 1);
                    }
                    fn += ".jpg";
                    picture.open(fn.toCString(), std::ios_base::out | std::ios_base::binary);
                    picture << value.value<TagLib::ByteVector>();
                    picture.close();
                }
                else {
                    std::cout << value << std::endl;
                }
            }
        }
    }

}

void iterateDirectory(const fs::path &in_path) {
    for (const fs::directory_entry &p : fs::recursive_directory_iterator(in_path)) {
        std::cout << "Got path " << p  << " \n";
        // print_metadata(p);
    } 
}


int main(int argc, char *argv[]) {
    
    img_extractor("C:/Users/richa/Code_Projects/prototype-music-player/test_music/Bluebird.mp3");
    // std::cout << "taglib version" << TAGLIB_MAJOR_VERSION << TAGLIB_MINOR_VERSION << TAGLIB_PATCH_VERSION <<  "\n";
    /*
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
    std::set<std::string>ext_set({
        ".id3", 
        ".mp3",
        ".m4a",
        ".aiff",
        //vorbis comment 
        ".opus",
        ".flac",
        ".ogg",
        ".wv",
        ".wvc",
    });
    

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

    for (int i = 0; i < 10; i++) {
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
    print_metadata("../test_music/Bluebird.mp3");
    return app.exec();
    */
}