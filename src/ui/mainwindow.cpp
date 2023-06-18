#include "mainwindow.h"
#include <QFileDialog>
#include <QStandardPaths>
#include "ui_mainwindow.h"

void cleanup_function(void *ci)
{
    delete reinterpret_cast<cv::Mat *>(ci);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->pbLoad, &QPushButton::clicked, this, &MainWindow::onLoadImageClicked);

    connect(ui->sbCutOff, &QDoubleSpinBox::valueChanged, this, &MainWindow::createApplyFilter);
    connect(ui->outputImageType, &QComboBox::currentIndexChanged, this, &MainWindow::showImage);
}

void MainWindow::onLoadImageClicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open Image"),
                                                    QStandardPaths::standardLocations(
                                                        QStandardPaths::PicturesLocation)
                                                        .front(),
                                                    tr("Image Files (*.png *.jpg *.bmp)"));
    if (fileName.isEmpty())
        return;

    ui->sbCutOff->setEnabled(true);
    ui->outputImageType->setEnabled(true);

    loadImage(fileName);
    createFilter();
    applyFilter();
}

void MainWindow::createApplyFilter()
{
    createFilter();
    applyFilter();
}

void MainWindow::loadImage(QString path)
{
    m_input = cv::imread(path.toLatin1().data());
    //cv::cvtColor(m_input, m_input, cv::COLOR_BGR2GRAY);
    cv::cvtColor(m_input, m_input, cv::COLOR_BGR2HSV);
}

void MainWindow::createFilter()
{
    const auto start = std::chrono::high_resolution_clock::now();
    m_filter = createPassFilter(m_input.size(), ui->sbCutOff->value());
    const auto time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                             std::chrono::high_resolution_clock::now() - start)
                             .count();
    ui->lbFilterTime->setText(QString("%1ms").arg(time_ms));
}

void MainWindow::applyFilter()
{
    std::vector<cv::Mat> hsv;
    cv::split(m_input, hsv);
    //cv::Mat hsv = m_input;

    const auto start = std::chrono::high_resolution_clock::now();

    hsv[2] = applyHomomorphicFilter(hsv[2], m_filter);
    const auto time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                             std::chrono::high_resolution_clock::now() - start)
                             .count();
    ui->lbApplyTime->setText(QString("%1ms").arg(time_ms));

    cv::merge(hsv, m_output);
    //m_output = hsv;
    showImage();
}

void MainWindow::showImage()
{
    if (ui->outputImageType->currentIndex() == 0) {
        presentImage(m_output);
    } else if (ui->outputImageType->currentIndex() == 1) {
        presentImage(m_input);presentImage(m_input);
    }
}

cv::Mat resizeKeepAspectRatio(const cv::Mat &input,
                              const cv::Size &dstSize,
                              const cv::Scalar &bgcolor)
{
    cv::Mat output;

    double h1 = dstSize.width * (input.rows / (double) input.cols);
    double w2 = dstSize.height * (input.cols / (double) input.rows);
    if (h1 <= dstSize.height) {
        cv::resize(input, output, cv::Size(dstSize.width, h1));
    } else {
        cv::resize(input, output, cv::Size(w2, dstSize.height));
    }

    int top = (dstSize.height - output.rows) / 2;
    int down = (dstSize.height - output.rows + 1) / 2;
    int left = (dstSize.width - output.cols) / 2;
    int right = (dstSize.width - output.cols + 1) / 2;

    cv::copyMakeBorder(output, output, top, down, left, right, cv::BORDER_CONSTANT, bgcolor);

    return output;
}

void MainWindow::presentImage(cv::Mat in)
{
    cv::Mat tmp = resizeKeepAspectRatio(in,
                                        cv::Size(ui->lbImg->width(), ui->lbImg->height()),
                                        cv::Scalar(0, 0, 0));
    cv::cvtColor(tmp, tmp, cv::COLOR_HSV2RGB);
    cv::Mat *out_mat_tmp = new cv::Mat(tmp);
    QImage result = QImage(out_mat_tmp->ptr(),
                           out_mat_tmp->cols,
                           out_mat_tmp->rows,
                           out_mat_tmp->step1(),
                           QImage::Format_RGB888,
                           cleanup_function,
                           out_mat_tmp);
    ui->lbImg->setPixmap(QPixmap::fromImage(std::move(result)));
}

MainWindow::~MainWindow()
{
    delete ui;
}
