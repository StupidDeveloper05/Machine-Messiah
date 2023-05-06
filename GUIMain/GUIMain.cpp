#include <qmessagebox.h>
#include <thread>

#include "GUIMain.h"
#include "OpenAI.h"
#include "util_fn.h"


GUIMain::GUIMain(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    // OpenAI initialize
    bool succssed = OpenAI::Init(
        "api key",
        "org-1XK4EGAKbk9RBmHca7zf6HLK"
    );
    if (!succssed)
        QMessageBox::critical(this, "Critical Error", "Failed to Initialize OpenAI!");

    PostHelper->Get()->SetUserPointer(&ui);
    //HttpContext->SetWriteFunction();

    // microphone setting
    mic = new MIC::MicRecord(1, 16000, 128);

    // connect widget event function
    connect(ui.pushButton, &QPushButton::clicked, this, &GUIMain::ButtonClicked);
}

GUIMain::~GUIMain()
{
    if (mic != nullptr)
        delete mic;
}

void GUIMain::ButtonClicked()
{
    if (!recording && !gpt_processing)
    {
        recording = true;
        if (mic == nullptr)
            mic = new MIC::MicRecord(1, 16000, 128);
        mic->Start();
        ui.pushButton->setText("입력 종료");
    }
    else if (recording)
    {
        recording = false;
        mic->Pause();
        delete mic;
        mic = nullptr;
        ui.pushButton->setText("처리 중...");
        ui.gpt_answer->setText("AI : ");
        std::thread(gptFromNatural, ui.user_input, &gpt_processing).detach();
    }
}