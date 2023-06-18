#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "../homomorphic_filter.hpp"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void onLoadImageClicked();
    void createApplyFilter();

    void loadImage(QString path);
    void createFilter();
    void applyFilter();

    void showImage();
    void presentImage(cv::Mat in);

    cv::Mat m_input;
    cv::Mat m_filter;
    cv::Mat m_output;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
