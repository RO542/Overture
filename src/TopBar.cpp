#include <QWidget>

#include "TopBar.hpp"

TopBar::TopBar(QWidget *parent):
    QToolBar(parent)
{
    this->setStyleSheet("QToolBar { background-color: #000000; }");
    this->setFixedHeight(65);

    // create a container widget structured with an hbox
    QWidget *container = new QWidget(this);
    this->addWidget(container);
    this->hboxContainerWidget = container;
    this->hbox = new QHBoxLayout(this->hboxContainerWidget);
    this->hboxContainerWidget->setLayout(this->hbox);


    //TODO: back button, search bar, options
    this->searchBar = new QLineEdit(this);
    this->hbox->addWidget(this->searchBar);

}

