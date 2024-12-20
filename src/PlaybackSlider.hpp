#include <QSlider>
#include <iostream>
class PlaybackSlider : public QSlider {
public:
    bool m_block_progress = false;
    PlaybackSlider(Qt::Orientation orientation, QWidget *parent) : QSlider(orientation, parent) {
        this->setRange(0, 100 * 100);
    }

    ~PlaybackSlider() = default;

public slots:
    void onPercentReceived(double percent) {
        if (m_block_progress)
            return;
        this->setValue(static_cast<int>(percent * 100));
    }
};