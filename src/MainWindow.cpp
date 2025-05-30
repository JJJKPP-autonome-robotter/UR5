#include "../headers/MainWindow.hpp"
#include <QLabel>
#include <QPixmap>
#include <QHBoxLayout>
#include <QFile>

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent),
      config("../config.yaml")
{
    setWindowTitle("Toggle Switches");
    resize(1280, 720);
    setupUI();
}

void MainWindow::setupUI()
{
    QHBoxLayout *mainLayout = new QHBoxLayout(); // Main layout for left and right sections

    // Left section: Toggle buttons
    QVBoxLayout *leftLayout = new QVBoxLayout();
    leftLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    leftLayout->setSpacing(20);

    // Read the initial state from the config file
    std::vector<std::string> initialColors = config.get<std::vector<std::string>>("cvCfg", "colorToPick");

    for (int i = 0; i < toggles.size(); ++i)
    {
        QPushButton *btn = new QPushButton(colorNames[i], this);
        btn->setCheckable(true);

        // Set the initial state based on the config
        bool isChecked = std::find(initialColors.begin(), initialColors.end(), colorNames[i].toStdString()) != initialColors.end();
        btn->setChecked(isChecked);

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
        leftLayout->addWidget(btn);
        toggles[i] = btn;

        // Connect the toggled signal to saveConfig
        connect(btn, &QPushButton::toggled, this, &MainWindow::saveConfig);
    }

    // Right section: Image display
    imageLabel = new QLabel(this);
    QPixmap pixmap;
    if (QFile::exists("show.jpg")) { // Check if the file exists
    
        pixmap.load("show.jpg"); // Load the image
        imageLabel->setPixmap(pixmap.scaled(800, 800, Qt::KeepAspectRatio, Qt::SmoothTransformation)); // Scale the image
    }
    else {
        imageLabel->setText("Image not found"); // Display a placeholder text
        imageLabel->setAlignment(Qt::AlignCenter);
    }

    // Add left and right sections to the main layout with stretch factors
    QWidget *leftWidget = new QWidget(this);
    leftWidget->setLayout(leftLayout);
    mainLayout->addWidget(leftWidget, 1); // Left section takes 1/3 of the width
    mainLayout->addWidget(imageLabel, 2); // Right section takes 2/3 of the width

    setLayout(mainLayout); // Set the main layout
}

// saves the current state of the toggles to the config file
void MainWindow::saveConfig() {
    std::vector<std::string> selectedColors;

    for (int i = 0; i < toggles.size(); ++i) {
        if (toggles[i]->isChecked()) {
            selectedColors.push_back(colorNames[i].toStdString());
        }
    }

    // Save the selected colors as a single array entry in the config
    config.set("cvCfg", "colorToPick", selectedColors);
    config.save();
}

void MainWindow::refreshImage()
{
    QPixmap pixmap("show.jpg");
    if (!pixmap.isNull()) // Check if the image exists and is valid
    {
        imageLabel->setPixmap(pixmap.scaled(800, 800, Qt::KeepAspectRatio, Qt::SmoothTransformation)); // Scale the image to larger dimensions
    }
    else
    {
        imageLabel->setText("Image not found"); // Display a placeholder text if the image is missing
        imageLabel->setAlignment(Qt::AlignCenter);
    }
}
