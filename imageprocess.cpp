#include "imageprocess.h"
#include <QDebug>
#include <QTextToSpeech>
#include <QUrl>

ImageProcess::ImageProcess()
{
    if(loadNetwork())
        this->loaded = true;
}

void preprocess(const image<float>& img,
                 const image<float>& mean,
                 int width,
                 int height,
                 vec_t* dst)
{
    image<float> resized = resize_image(img, width, height);

    image<> resized_uint8(resized);

    if (!mean.empty()) {
        image<float> normalized = subtract_scalar(resized, mean);
        *dst = normalized.to_vec();
    }
    else {
        *dst = resized.to_vec();
    }
}

void ImageProcess::sayText(const QString text)
{
    QTextToSpeech *tts = new QTextToSpeech();
    tts->say(text);
    //free(tts);
}

vector<string> get_label_list(const QString &label_file)
{
    QFile file(label_file);

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        throw runtime_error("failed to open:" + label_file.toStdString());

    QTextStream in(&file);
    vector<string> lines;
    while (!in.atEnd())
        lines.push_back(in.readLine().toStdString());

    return lines;
}
void ImageProcess::setPath(const QString newpath)
{
    nnPath = QUrl(newpath).toLocalFile();;
    qDebug() << "Path set to: " << nnPath;
}

bool ImageProcess::loadNetwork()
{
    QFileInfo check_file(nnPath);
    if(!check_file.exists())
    {
        qDebug() << "File " << nnPath << " doesn't exist";
        return false;
    }
    cout << "Loading network." << endl;
    clock_t load_begin = clock();
    net.load(nnPath.toStdString().c_str());
    clock_t load_end = clock();
    double load_secs = double(load_end - load_begin) / CLOCKS_PER_SEC;
    cout << "Load time(s): " << load_secs << endl;
    return true;
}

void ImageProcess::processImage(const QString path)
{
    if(!loaded)
    {
        if(loadNetwork())
        {
            loaded = true;
        }
        else
        {
            return;
        }
    }
    int height = net[0]->in_data_shape()[0].height_;
    int width = net[0]->in_data_shape()[0].width_;
    image<float> mean; //intentionally empty
    auto labels = get_label_list(":/synset_words.txt");

    image<float> img(path.toStdString().c_str(), image_type::bgr);
    vec_t vec;

    preprocess(img, mean, width, height, &vec);

    clock_t predict_begin = clock();
    auto result = net.predict(vec);
    clock_t predict_end = clock();
    double predict_secs = double(predict_end - predict_begin) / CLOCKS_PER_SEC;
    cout << "Predict time(s): " << predict_secs << endl;

    vector<tiny_dnn::float_t> sorted(result.begin(), result.end());
    int top_n = 3;
    partial_sort(sorted.begin(), sorted.begin()+top_n, sorted.end(), greater<tiny_dnn::float_t>());

    for (int i = 0; i < top_n; i++) {
        size_t idx = distance(result.begin(), find(result.begin(), result.end(), sorted[i]));
        cout << labels[idx] << "," << sorted[i] << endl;
        //sayText(QString::fromStdString(labels[idx]));
        emit sendToast(QString::number(i+1) + ". " + QString::fromStdString(labels[idx]) + " - " + QString::number((double)sorted[i]), 10000);
    }
}

void ImageProcess::imageSaved(const QString &path)
{
    qDebug() << path;
    processImage(path);
}
