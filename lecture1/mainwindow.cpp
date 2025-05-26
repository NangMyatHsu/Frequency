#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QDir>
//#include <Qtex
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setStyleSheet("QMainWindow { background-image: url(:/new/prefix1/image/color.jpg); background-repeat: no-repeat; background-position: center; }");
    ui->plotter->addGraph();
    ui->plotter->graph(0)->setScatterStyle(QCPScatterStyle::ssCircle);
    ui->plotter->graph()->setLineStyle(QCPGraph::lsLine);
    ui->plotter->xAxis->setLabel("X");
    ui->plotter->yAxis->setLabel("Y");
    ui->plotter->xAxis->setRange(-50,1000);
    ui->plotter->yAxis->setRange(0,500);
    ui->plotter->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    QVector<double> x={1,2,3,4,5,6,7,8,9,10},y={4,6,8,2,5};
    ui->plotter->graph(0)->setData(x,y);
    ui->plotter->rescaleAxes();
    ui->plotter->replot();
    ui->plotter->update();
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        ui->devicesList->addItem(info.portName());
    }
    ui->baudrateList->addItem("9600");
    ui->baudrateList->addItem("19200");
    ui->baudrateList->addItem("38400");
    ui->baudrateList->addItem("57600");
    ui->baudrateList->addItem("115200");
    ui->red->setEnabled(0);
    ui->green->setEnabled(0);
    ui->blue->setEnabled(0);
    // Set default selections
    ui->devicesList->setCurrentIndex(0);
    ui->baudrateList->setCurrentText("9600");

}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::on_ConnectButton_clicked(bool checked)
{
    if (!isConnected) {
        if (serial == nullptr) {
            serial = new QSerialPort(this);
        }

        QString portName = "/dev/" + (ui->devicesList->currentText());
        int baudRate = ui->baudrateList->currentText().toInt();
        serial->setPortName(portName);
        serial->setBaudRate(baudRate);
        serial->setDataBits(QSerialPort::Data8);
        serial->setParity(QSerialPort::NoParity);
        serial->setStopBits(QSerialPort::OneStop);
        serial->setFlowControl(QSerialPort::NoFlowControl);

        if (serial->open(QIODevice::ReadWrite)) {
            connect(serial, &QSerialPort::readyRead, this, &MainWindow::onSerialDataReceived);
            qDebug() << "Connected to" << portName << "at" << baudRate << "baud.";
            isConnected = true;
            ui->ConnectButton->setText("Disconnect");
            ui->red->setEnabled(1);
            ui->green->setEnabled(1);
            ui->blue->setEnabled(1);
        } else {
            qDebug() << "Failed to connect:" << serial->errorString();
        }
    }
    else {
        disconnect(serial, &QSerialPort::readyRead, this, &MainWindow::onSerialDataReceived);
        serial->close();
        serial->deleteLater();
        serial = nullptr;
        isConnected = false;
        ui->ConnectButton->setText("Connect");
        ui->red->setEnabled(false);
        ui->green->setEnabled(false);
        ui->blue->setEnabled(false);
        qDebug() << "Disconnected.";
    }

}
void MainWindow::onSerialDataReceived()
{
    serialBuffer.append(serial->readAll());

    while (serialBuffer.contains('\n')) {
        int endIndex = serialBuffer.indexOf('\n');
        QByteArray line = serialBuffer.left(endIndex).trimmed();
        serialBuffer.remove(0, endIndex + 1);

        QString lineStr = QString::fromUtf8(line);
        ui->textEdit->append(lineStr);

        QRegularExpression regex("Analog Value\\s*:\\s*(-?\\d+)");
        QRegularExpressionMatch match = regex.match(lineStr);

        if (match.hasMatch()) {
            int value = match.captured(1).toInt();

            ui->plotter->graph(0)->addData(xCounter, value);

            // Set the X range to show the last 100 values
            if (xCounter > 20) {
                ui->plotter->xAxis->setRange(xCounter - 20, xCounter);
            } else {
                ui->plotter->xAxis->setRange(0, 20);
            }

            ui->plotter->graph(0)->rescaleValueAxis(true);  // Autoscale Y only
            ui->plotter->replot();

            xCounter++;
        }
    }
}



void MainWindow::on_devicesList_activated(const QString &arg1)
{
    return;
}
void MainWindow::on_red_clicked()
{
    serial->write("R");
}
void MainWindow::on_blue_clicked()
{
    serial->write("B");
}
void MainWindow::on_green_clicked()
{
    serial->write("G");
}
void MainWindow::on_closeButton_clicked()
{

}
void MainWindow::on_plotButton_clicked()
{
    QVector<double> x={1,2,3,4,5},y={4,6,8,2,5};
    ui->plotter->graph(0)->setData(x,y);
    ui->plotter->rescaleAxes();
    ui->plotter->replot();
    ui->plotter->update();
}
void MainWindow::on_clearButton_clicked()
{
    ui->plotter->graph(0)->data()->clear();
    ui->plotter->replot();
    xCounter = 0;
}

void MainWindow::on_lineStyleComboBox_currentIndexChanged(int index){
    switch (index){
    case 0:
        ui->plotter->graph(0)->setLineStyle(QCPGraph::lsNone);
        break;
    case 1:
        ui->plotter->graph(0)->setLineStyle(QCPGraph::lsLine);
        break;
    case 2:
        ui->plotter->graph(0)->setLineStyle(QCPGraph::lsStepLeft);
        break;
    case 3:
        ui->plotter->graph(0)->setLineStyle(QCPGraph::lsStepRight);
        break;
    case 4:
        ui->plotter->graph(0)->setLineStyle(QCPGraph::lsStepCenter);
        break;
    case 5:
        ui->plotter->graph(0)->setLineStyle(QCPGraph::lsImpulse);
        break;
    default:
        ui->plotter->graph(0)->setLineStyle(QCPGraph::lsNone);
        break;
    }
    ui->plotter->replot();
    ui->plotter->update();
}
void MainWindow::on_plotTypeComboBox_currentIndexChanged(int index)
{
    switch (index){
    case 0:
        ui->plotter->graph(0)->setScatterStyle(QCPScatterStyle::ssNone);
        break;
    case 1:
        ui->plotter->graph(0)->setScatterStyle(QCPScatterStyle::ssDot);
        break;
    case 2:
        ui->plotter->graph(0)->setScatterStyle(QCPScatterStyle::ssCross);
        break;
    case 3:
        ui->plotter->graph(0)->setScatterStyle(QCPScatterStyle::ssPlus);
        break;
    case 4:
        ui->plotter->graph(0)->setScatterStyle(QCPScatterStyle::ssCircle);
        break;

    case 5:
        ui->plotter->graph(0)->setScatterStyle(QCPScatterStyle::ssDisc);
        break;
    case 6:
        ui->plotter->graph(0)->setScatterStyle(QCPScatterStyle::ssPlusSquare);
        break;
    case 7:
        ui->plotter->graph(0)->setScatterStyle(QCPScatterStyle::ssCrossCircle);
        break;
    case 8:
        ui->plotter->graph(0)->setScatterStyle(QCPScatterStyle::ssPlusCircle);
        break;
    case 9:
        ui->plotter->graph(0)->setScatterStyle(QCPScatterStyle::ssStar);
        break;
    case 10:
        ui->plotter->graph(0)->setScatterStyle(QCPScatterStyle::ssPeace);
      break;
    case 11:
        ui->plotter->graph(0)->setScatterStyle(QCPScatterStyle::ssTriangleInverted);
        break;
  default:
        ui->plotter->graph(0)->setScatterStyle(QCPScatterStyle::ssNone);
        break;
    }
    ui->plotter->replot();
    ui->plotter->update();
}


