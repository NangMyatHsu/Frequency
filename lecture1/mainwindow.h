#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_ConnectButton_clicked(bool checked);
    void on_devicesList_activated(const QString &arg1);
    void onSerialDataReceived(); // 👈 New slot for incoming data

    void on_red_clicked();
    void on_blue_clicked();
    void on_green_clicked();
    void on_closeButton_clicked();

    void on_plotButton_clicked();

    void on_clearButton_clicked();

    void on_lineStyleComboBox_currentIndexChanged(int index);

    void on_plotTypeComboBox_currentIndexChanged(int index);

private:
    Ui::MainWindow *ui;
    QSerialPort *serial = nullptr;
    QByteArray serialBuffer;
    int xCounter = 0;
    bool isConnected = false;
};

#endif // MAINWINDOW_H
