#include "../headers/MainWindow.hpp"

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("Toggle Switches");
    resize(1280, 720);
    setupUI();
}

void MainWindow::setupUI()
{
    layout = new QVBoxLayout();
    layout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    layout->setSpacing(20);

    for (int i = 0; i < toggles.size(); ++i)
    {
        QPushButton *btn = new QPushButton(colorNames[i], this);
        btn->setCheckable(true);
        btn->setChecked(true); // Start checked
        btn->setFixedSize(300, 100);
        btn->setStyleSheet(R"(
            QPushButton {
                background-color: gray;
                color: white;
                font-size: 24px;
                border: none;
                border-radius: 12px;
            }
            QPushButton:checked {
                background-color: )" +
                           colorNames[i] + R"(;
                color: black;
            }
        )");
        layout->addWidget(btn);
        toggles[i] = btn;
    }

    setLayout(layout);
}
