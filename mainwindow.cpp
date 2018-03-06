#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <QVector>
#include <QFile>
#include <QTextStream>
#include <QThread>
#include <QTime>
#include <QApplication>

const static QString filesFilePath = "D:\\files.txt";
const static QString settingsFilePath = "D:\\settings.txt";

QVector<QString> text, files, settings;

QFile filesFile(filesFilePath);
QFile settingsFile(settingsFilePath);
QTextStream filesStream(&filesFile);
QTextStream settingsStream(&settingsFile);

int textPos = 0;

const QVector<QChar> letters =
{
    '.', ',', ';'
};

int shortDelay = 150;
int longDelay = shortDelay*2;

int delay = shortDelay;

bool appendFromFile(QFile &file, QTextStream &stream, QVector<QString> &vector)
{
    if (file.open(QIODevice::ReadOnly))
    {
        vector.clear();
        while (!stream.atEnd())
        {
            QString line = stream.readLine();
            for (int i = 0; i < line.split(" ").size(); i++)
            {
                vector.append(line.split(" ").at(i));
            }
        }
        file.close();
    }else {
        return false;
    }
    return true;
}

void addFilesToList(Ui::MainWindow *ui)
{
    for (int i = 0; i < files.size(); i++)
    {
        ui->fileList->addItem(files.at(i));
    }

}

void addFileToList(QString filePath, Ui::MainWindow *ui)
{
    files.append(filePath);
    ui->fileList->addItem(filePath);

    if (filesFile.open(QIODevice::Append))
    {
        filesStream << filePath << endl;
    }
    filesFile.close();
}

void setFirstLabels(Ui::MainWindow *ui)
{
    if (text.size() > 0)
    {
        ui->TextLabel1->setText(text.at(0));
    } else {
        ui->TextLabel1->setText("");
    }

    if (text.size() > 1)
    {
        ui->TextLabel2->setText(text.at(1));
    } else {
        ui->TextLabel2->setText("");
    }

    ui->TextLabel0->setText("");
}

void getFile(QString filePath, Ui::MainWindow *ui)
{

    QFile textFile(filePath);
    QTextStream textStream(&textFile);

    if (appendFromFile(textFile, textStream, text))
    {
        setFirstLabels(ui);

        bool add = true;
        for (int i = 0; i < files.size(); i++)
        {
           if (files.at(i) == filePath)
           {
               add = false;
               break;
           }
        }

        if (add)
        {
           addFileToList(filePath, ui);
        }
    }
}

bool shouldDelay()
{
    for (int i = 0; i < letters.size(); i++)
    {
        if (text.at(textPos).endsWith(letters.at(i)))
            return true;
    }
    return false;
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->FilePathLineEdit->setText("File Path");

    QObject::connect(ui->speedSlider, SIGNAL(valueChanged(int)), ui->speedSpinBox, SLOT(setValue(int)));
    QObject::connect(ui->speedSpinBox, SIGNAL(valueChanged(int)), ui->speedSlider, SLOT(setValue(int)));

    ui->speedSlider->setRange(25, 200);
    ui->speedSpinBox->setRange(25, 200);
    ui->speedSlider->setValue(100);

    appendFromFile(filesFile, filesStream, files);
    appendFromFile(settingsFile, settingsStream, settings);
    addFilesToList(ui);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_StartButton_clicked()
{
    QTime time;
    time.start();

    while (textPos < text.size())
    {
        ui->StartButton->setText(QString::number(static_cast<int>(delay / (ui->speedSlider->value()/100))));
        if (time.elapsed() >= static_cast<int>(delay / (ui->speedSlider->value()/100)))
        {
            if (textPos > 0)
                ui->TextLabel0->setText(text.at(textPos - 1));
            ui->TextLabel1->setText(text.at(textPos));
            if (textPos < text.size() - 1)
                ui->TextLabel2->setText(text.at(textPos + 1));

            if (shouldDelay())
                delay = longDelay;
            else
                delay = shortDelay;

            textPos++;
            time.restart();
        }
        QApplication::processEvents();
    }
}

void MainWindow::on_OpenFileButton_clicked()
{
    getFile(ui->FilePathLineEdit->text(), ui);
}

void MainWindow::on_fileList_itemClicked(QListWidgetItem *item)
{
    getFile(item->text(), ui);
}

