#include <QWheelEvent>
#include <QSlider>

class VolumeSlider : public QSlider {
    Q_OBJECT
public:
    VolumeSlider(Qt::Orientation orientation, QWidget *parent) : QSlider(orientation, parent) {
        //FIXME: maybe allow intermediate numbers in the range here like the playback slider 
        this->setRange(0, 100);
    }

signals:
    void scrollUp();
    void scrollDown();

protected:
    void wheelEvent(QWheelEvent *event) override {
        //TODO: possibly make the scroll direction a config option
        event->angleDelta().y() < 0 ? emit scrollDown() : emit scrollUp();
        QSlider::wheelEvent(event);
    }

};

static inline int clamp(int num, int min, int max) {
    return num < min ? min : (num > max ? max : num);
}



