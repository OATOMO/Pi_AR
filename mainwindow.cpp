#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "grabthread.h"
#include <unistd.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

//	grabThread * gThread = new grabThread();
//	gThread->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}
