#include "pch.h"

#include "measurementwindow.h"

MeasurementWindow::MeasurementWindow(QWidget *parent) : QMainWindow(parent), _ui(new Ui::MeasurementWindow) {
    _ui->setupUi(this);
}

MeasurementWindow::~MeasurementWindow() {
    delete _ui;
}
