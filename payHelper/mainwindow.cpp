#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowFlags(this->windowFlags() | Qt::WindowStaysOnTopHint);
}

MainWindow::~MainWindow()
{
    delete ui;
}
