#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSlider>
#include <QToolBar>
#include <QWidget>
#include <QToolButton>
#include <QIcon>
#include <QObject>
#include <QSpacerItem>
#include <QLabel>
#include <QString>

#include "Player.hpp"
#include "PlaybackSlider.hpp"
#include "VolumeSlider.hpp"

class PlaybackBar : public QToolBar {
public:
    PlaybackBar(QWidget *parent, MPVPlayer *p);
    ~PlaybackBar() = default;
    MPVPlayer *player; // the player is global so public or private isn't relevant anyway


private:
    QWidget *centralContainer = new QWidget(this);
    QToolButton *pauseButton = new QToolButton(this);
    QToolButton *nextButton = new QToolButton(this);
    QToolButton *prevButton = new QToolButton(this);

    PlaybackSlider *playbackSlider = new PlaybackSlider(Qt::Orientation::Horizontal, this);
    VolumeSlider *volumeSlider = new VolumeSlider(Qt::Orientation::Horizontal, this);
    QLabel *timeLabel = new QLabel(this);

    QLabel *artistLabel = new QLabel(this);
    QLabel *titleLabel = new QLabel(this);
    QLabel *durationLabel = new QLabel(this);


    QIcon pauseIcon = QIcon::fromTheme(QIcon::ThemeIcon::MediaPlaybackPause);
    QIcon startIcon = QIcon::fromTheme(QIcon::ThemeIcon::MediaPlaybackStart);

public slots:
    void receivedFileLoaded() {
        //TODO: new_file_id might not be useful in the PlaybackBar, maybe elsewhwer
        pauseButton->setIcon(pauseIcon);
    }

    void receivedFileEnded() {
        pauseButton->setIcon(startIcon);
        playbackSlider->setValue(0);
        timeLabel->setText("0:00");
    }

    void receivedPauseChanged(int pause_state) {
        pause_state == 1 
            ? pauseButton->setIcon(startIcon)
            : pauseButton->setIcon(pauseIcon);

        //TODO: if a user seeks by dragging the slider during playback they will hear garbled audio
        // to avoid this maybe the slider needs to track if playback is not paused and it isn't 
        // cyclePause before and after seeking although this may be a premature optimization
        // another option is to cache current volume setVolume to 0 without triggering a signal and setting 
        // it back at the end 
        // playbackSlider->m_pause = pause_state;
        
    }

    void receivedTime(double in_time) {
        int rounded_time = static_cast<int>(in_time);
        int minutes = rounded_time / 60;
        int seconds = rounded_time % 60;
        QString s2 = QString::number(minutes) + ":" + (seconds < 10 ? "0" : "") + QString::number(seconds) + "/";
        timeLabel->setText(s2);
    }

    void receivedMetadata(const char *in_artist, const char *in_title) {
        // std::cout << "Got artist: " << in_artist << "Title" << in_title << "\n";
        titleLabel->setText(QString(in_title) + " - ");
        artistLabel->setText(QString(in_artist));
    }

    void receivedDuration(double duration) {
        int rounded_time = static_cast<int>(duration);
        int minutes = rounded_time / 60;
        int seconds = rounded_time % 60;

        QString duration_str = QString::number(minutes) + ":" + (seconds < 10 ? "0" : "") + QString::number(seconds); 
        durationLabel->setText(duration_str);
    }
};