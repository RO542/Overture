
#include <filesystem>

#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>

#include <QGridLayout>
#include <QPixmap>
#include <QScrollArea>
#include <QListView>
#include <QStandardItemModel>

#include "DatabaseManager.hpp"
#include "PlaybackBar.hpp"
#include "Player.hpp"
#include "SongList.hpp"
#include "TopBar.hpp"

#include <filesystem>
namespace fs = std::filesystem;

int main(int argc, char *argv[]) {
    /*    
    std::cout << "taglib version " << TAGLIB_MAJOR_VERSION << "." << TAGLIB_MINOR_VERSION << "." << TAGLIB_PATCH_VERSION << "\n";
    std::cout << "sqlite version " << SQLITE_VERSION_NUMBER << "\n";
    std::cout << "mpv version " << MPV_CLIENT_API_VERSION << "\n";
    std::cout << "xxhash version " << XXH_VERSION_NUMBER << "\n";
    // std::cout << "FTS5 found ? " << found_fts5 << std::endl;
    */


    fs::path db_path = "C:/Users/richa/Code_Projects/prototype-music-player/src/db/metadata.db";
    assert(fs::exists(db_path));
    DatabaseManager qm = DatabaseManager(db_path);
    qm.iterateMusicDirs();
    /*
    QApplication app(argc, argv);
    QMainWindow mainWindow;
    mainWindow.resize(960, 540);
    // mainWindow.resize(1920, 1080);
    mainWindow.setWindowTitle("Overture");
    QWidget *centralWidget = new QWidget(&mainWindow);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    MPVPlayer *player = new MPVPlayer(&mainWindow);
    TopBar *topBar = new TopBar(centralWidget);
    PlaybackBar *playbackBar = new PlaybackBar(centralWidget, player);

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
    // print_metadata("../test_music/Bluebird.mp3");
    return app.exec();
    */
}
