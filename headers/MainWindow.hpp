#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <array>

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    std::array<QPushButton *, 6> toggles; // Array of toggle buttons
    QVBoxLayout *layout;                 // Layout for toggle buttons
    std::array<QString, 6> colorNames = {"red", "orange", "yellow", "green", "blue", "brown"};
    QLabel *imageLabel;                  // Label to display the image

    void setupUI();
    void toggleColor(int index);

public slots:
    void refreshImage(); // Slot to refresh the image
};

#endif // MAINWINDOW_HPP
