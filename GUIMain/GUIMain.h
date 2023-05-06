#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_GUIMain.h"
#include "MicRecord.h"

class GUIMain : public QMainWindow
{
    Q_OBJECT

public:
    GUIMain(QWidget *parent = nullptr);
    ~GUIMain();

private slots:
    void ButtonClicked();

private:
    Ui::GUIMainClass ui;

private:
    MIC::MicRecord* mic;
    bool recording = false;
    bool gpt_processing = false;
};
