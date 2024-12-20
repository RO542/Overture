//TopBar.hpp
#include <QToolBar>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "../deps/sqlite3/sqlite3.h"

class TopBar : public QToolBar
{
public:
    TopBar(QWidget *parent);
    ~TopBar() = default;

private:
    QWidget *hboxContainerWidget;
    QHBoxLayout *hbox;
    QLineEdit *searchBar;
};