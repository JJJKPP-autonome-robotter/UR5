#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <array>

#include "Data_saver.hpp" // Ensure this defines the data_saver class

class MainWindow : public QWidget {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
<<<<<<< HEAD
=======
    void refreshImage(); // Slot to refresh the image
>>>>>>> fa16e00c7df50819730b8a2fda7f181fc79d9716

private:
    std::array<QPushButton *, 6> toggles; // Array of toggle buttons
    QVBoxLayout *layout;                 // Layout for toggle buttons
    std::array<QString, 6> colorNames = {"red", "orange", "yellow", "green", "blue", "brown"};
    QLabel *imageLabel;                  // Label to display the image
    ConfigFile config;

    void setupUI();
    void toggleColor(int index);

private slots:
    void saveConfig();   // Declare saveConfig
<<<<<<< HEAD
    void refreshImage(); // Slot to refresh the image
=======
    
>>>>>>> fa16e00c7df50819730b8a2fda7f181fc79d9716
};

#endif // MAINWINDOW_HPP
