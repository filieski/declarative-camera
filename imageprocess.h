#ifndef IMAGEPROCESS_H
#define IMAGEPROCESS_H
#include <QString>
#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QMessageBox>
#include <iostream>
#include <fstream>
#include <memory>
#include <ctime>
#include <string>
//#define CNN_USE_OMP
#define USE_OPENCL
#include "tiny_dnn/tiny_dnn.h"
using namespace tiny_dnn;
using namespace tiny_dnn::activation;
using namespace std;

class ImageProcess : public QObject
{
    Q_OBJECT
public slots:
    void imageSaved(const QString &path);
public:
    ImageProcess();
    Q_INVOKABLE void processImage(const QString path);
    Q_INVOKABLE void setPath(const QString newpath);
private:
    network<sequential> net;
    //vector<string> labels;
    bool loaded = false;
    bool loadNetwork();
    void sayText(const QString text);
    QString nnPath;

signals:
    void sendToast(QString text, int duration);
};

#endif // IMAGEPROCESS_H
