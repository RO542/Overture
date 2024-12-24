#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdio>
#include <iomanip>

#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>

#include <QGridLayout>
#include <QPixmap>
#include <QScrollArea>
#include <QListView>
#include <QStandardItemModel>

#include <taglib/tpropertymap.h>
#include <taglib/tstringlist.h>
#include <taglib/tvariant.h>
#include <taglib/fileref.h>
#include <taglib/tag.h>

#include "FileExtensions.hpp"

#include "PlaybackBar.hpp"
#include "Player.hpp"
#include "SongList.hpp"




#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize2.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


/*
     stbir_resize_uint8_srgb( input_pixels,  input_w,  input_h,  input_stride_in_bytes,
                              output_pixels, output_w, output_h, output_stride_in_bytes,
                              pixel_layout_enum )

     stbir_resize_uint8_linear( input_pixels,  input_w,  input_h,  input_stride_in_bytes,
                                output_pixels, output_w, output_h, output_stride_in_bytes,
                                pixel_layout_enum )

     stbir_resize_float_linear( input_pixels,  input_w,  input_h,  input_stride_in_bytes,
                                output_pixels, output_w, output_h, output_stride_in_bytes,
                                pixel_layout_enum )
*/

namespace fs = std::filesystem;

//NOTE: stbi_uc is just unsigned char 

void downsizeThumbnailAndSave(const TagLib::ByteVector& inputData, const TagLib::String& outputPath) {
    int in_width, in_height, in_channels;
    const stbi_uc *inputBytes = (const stbi_uc *)inputData.data();
    if (!inputBytes) {
        std::cout << "input images could not be converted to u8 bytes \n";
        return;
    }

    stbi_uc *rawBytes = stbi_load_from_memory(inputBytes, inputData.size(), &in_width, &in_height, &in_channels, 0);
    if (!rawBytes) {
        std::cout << "Unable to use stbi_load_from_memory for this TagLib::ByteVector\n";
        return;
    }

    unsigned char *resizedBytes = stbir_resize_uint8_linear(
        rawBytes, in_width, in_height, 0,
        NULL, 256, 256, 0,
        (stbir_pixel_layout)(in_channels)
    );
    if (!resizedBytes) {
        std::cout << "Unable to perform stb image resize \n";
        stbi_image_free(rawBytes);
        return;
    }
    TagLib::String final_path = outputPath + ".jpg";

    int write_result = stbi_write_jpg(final_path.toCString(true), 256, 256, in_channels ,(void *)resizedBytes, 100);
    std::cout << "Result " << write_result << " for:  " << outputPath << "\n";

    std::cout << "Correctly resized image to 256 x 256\n"; 
    std::cout << "Final path ? " << outputPath << "\n";
    stbi_image_free(rawBytes);
    free(resizedBytes);
}


void tag_extractor(const fs::path &in_path) {
    std::string path = in_path.string();
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

    // std::cout << "BASIC TAGS\n";
    // std::cout << basic_tags->title() << "\n";
    // std::cout << basic_tags->artist()  << "\n";
    // std::cout << basic_tags->album()  << "\n";
    // std::cout << basic_tags->genre()  << "\n";
    // std::cout << basic_tags->year()  << "\n";
    // std::cout << basic_tags->track() << "\n";

    if (fileref.complexPropertyKeys().contains("PICTURE")) {
        // std::cout << "Found PICTURE in " << in_path << "\n";
        const TagLib::List<TagLib::VariantMap> picture_list_maps = fileref.complexProperties("PICTURE");
        for (const auto &property_map : picture_list_maps) {
            for (auto &[key, value] : property_map) {
                if (value.type() == TagLib::Variant::ByteVector) {
                    // std::cout << "Found Image Byte Vector\n";
                    const TagLib::ByteVector vec = value.value<TagLib::ByteVector>();
                    std::string e = "C:/Users/richa/Code_Projects/prototype-music-player/test_images/";
                    TagLib::String out = e + basic_tags->title();
                    downsizeThumbnailAndSave(vec, out);
                    // std::cout << out << "\n";
                }
            }
        }
    }
}
    

void iterateDirectory(const fs::path &in_path) {
    for (const fs::directory_entry &p : fs::recursive_directory_iterator(in_path)) {
        if (p.is_regular_file() && ext_set.count(p.path().extension().string()) > 0) {
            tag_extractor(p);
        }
    } 
}


int main(int argc, char *argv[]) {
    
    std::cout << "taglib version" << TAGLIB_MAJOR_VERSION << TAGLIB_MINOR_VERSION << TAGLIB_PATCH_VERSION <<  "\n";
    iterateDirectory("../test_music");

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