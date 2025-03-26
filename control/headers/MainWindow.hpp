#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <array>

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    std::array<QPushButton *, 6> toggles;
    QVBoxLayout *layout;
    std::array<QString, 6> colorNames = {"red", "orange", "yellow", "green", "blue", "brown"};

    void setupUI();
    void toggleColor(int index);
};

#endif // MAINWINDOW_HPP
