#include "PlaybackBar.hpp"

PlaybackBar::PlaybackBar(QWidget *parent, MPVPlayer *p): QToolBar(parent) {
    this->player = p;
    this->setStyleSheet("QToolBar { background-color: #000000; }");
    this->setFixedHeight(85);
    this->addWidget(centralContainer);
    constexpr int button_size = 40;
    constexpr int volume_scroll_factor = 1;


    pauseButton->setFixedSize(button_size, button_size);
    nextButton->setFixedSize(button_size, button_size);
    prevButton->setFixedSize(button_size, button_size);

    // primary vbox, progress slider on top other playback controls below 
    QVBoxLayout *barVbox = new QVBoxLayout();
    barVbox->setContentsMargins(0, 0, 0, 0);
    barVbox->setSpacing(0);
    centralContainer->setLayout(barVbox);

    // add playback progress slider
    QSpacerItem *negative_space = new QSpacerItem(0, -10, QSizePolicy::Minimum, QSizePolicy::Fixed);
    barVbox->addItem(negative_space);
    playbackSlider->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    barVbox->addWidget(playbackSlider);


    QWidget *playbackControls = new QWidget(this);
    QHBoxLayout *playbackControlsLayout = new QHBoxLayout();
    playbackControls->setLayout(playbackControlsLayout);
    barVbox->addWidget(playbackControls);

    playbackControlsLayout->addWidget(timeLabel);
    playbackControlsLayout->addWidget(durationLabel);
    playbackControlsLayout->addWidget(titleLabel);
    playbackControlsLayout->addWidget(artistLabel);

    playbackControlsLayout->addStretch();
    QWidget *controlClusterWidget = new QWidget();
    QHBoxLayout *controlCluster = new QHBoxLayout();
    controlCluster->setContentsMargins(0, 0, 0, 0);
    controlCluster->setSpacing(0);
    controlClusterWidget->setLayout(controlCluster);
    controlCluster->addWidget(prevButton, 0, Qt::AlignLeft);
    controlCluster->addWidget(pauseButton, 0, Qt::AlignRight);
    controlCluster->addWidget(nextButton, 0 , Qt::AlignRight);
    playbackControlsLayout->addWidget(controlClusterWidget);
    playbackControlsLayout->addStretch();
    playbackControlsLayout->addWidget(volumeSlider, 0, Qt::AlignRight);

    timeLabel->setText("0:00");
    volumeSlider->setValue(player->m_volume);
    pauseButton->setIcon(startIcon);
    prevButton->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::MediaSkipBackward));
    nextButton->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::MediaSkipForward));



    // incoming signals from MPVEventGenerator
    connect(player->worker, &MPVEventGenerator::timeChanged,     this, &PlaybackBar::receivedTime);
    connect(player->worker, &MPVEventGenerator::metadataChanged, this, &PlaybackBar::receivedMetadata);
    connect(player->worker, &MPVEventGenerator::durationChanged, this, &PlaybackBar::receivedDuration);
    connect(player->worker, &MPVEventGenerator::fileLoaded,      this, &PlaybackBar::receivedFileLoaded);
    connect(player->worker, &MPVEventGenerator::fileEnded,       this, &PlaybackBar::receivedFileEnded);
    connect(player->worker, &MPVEventGenerator::pauseChanged,    this, &PlaybackBar::receivedPauseChanged);
    connect(player->worker, &MPVEventGenerator::percentChanged, playbackSlider, &PlaybackSlider::onPercentReceived);



    //TODO: in order to add support for keyboard shortcuts it might be easier to make QActions which respond to either kb input or gui inputs
    // Everything here and blow is something which the UI uses to control the player
    connect(volumeSlider, &QSlider::sliderReleased, [&]() {
       player->setVolume(volumeSlider->value());
    });

    connect(volumeSlider, &QSlider::sliderMoved, [&]() {
        player->setVolume(volumeSlider->value());
    });


    connect(volumeSlider, &VolumeSlider::scrollUp, [&]() {
        int next_vol = clamp(volumeSlider->value() + volume_scroll_factor, 0, 100);
        player->setVolume(next_vol);
        volumeSlider->setValue(next_vol);
    });

    connect(volumeSlider, &VolumeSlider::scrollDown, [&]() {
        int next_vol = clamp(volumeSlider->value() - volume_scroll_factor, 0, 100);
        player->setVolume(next_vol);
        volumeSlider->setValue(next_vol);
    });

    connect(pauseButton, &QToolButton::pressed, [&]() { 
        player->cyclePause();
    });

    // prevent the playbackSlider from being moved by the Player when the user
    connect(playbackSlider, &QSlider::sliderPressed, [&]() {
        playbackSlider->m_block_progress = true;
    });

    connect(playbackSlider, &QSlider::sliderReleased, [&]() {
        double desired_seek_percent = playbackSlider->value() / 100.0;
        player->seekAbsolutePercent(desired_seek_percent);
        playbackSlider->m_block_progress = false;
    });

    //FIXME: call player playlist playlist next
    connect(nextButton, &QToolButton::pressed, [&]() {
        player->seekRelative(+5);
    });

    connect(prevButton, &QToolButton::pressed, [&]() {
        player->seekRelative(-5);
    });
}