#pragma once

#include "ui_measurementwindow.h"

namespace Ui {
    class MeasurementWindow;
};

class MeasurementWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MeasurementWindow(QWidget *parent = nullptr);
    virtual ~MeasurementWindow();

private:
    Ui::MeasurementWindow *_ui;
};
