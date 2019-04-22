#include <QGuiApplication>
#include <QFile>
#include <QDir>
#include <QImage>
#include <QDebug>

#include <exception>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    try {
        QImage img(1080, 1920, QImage::Format_RGB32);
        QFile f(argv[1]);
        const QString fn = QDir::homePath() + QStringLiteral("/fb.png");
        bool saveOk = false;

        img.fill(Qt::black);

        if (f.open(QFile::ReadOnly)) {

            qint64 sz = f.size();
            const qint64 idealSize = 1920*1080*4;
            const qint64 sizeDifference = sz - idealSize;
            const qint64 diffPerLine = sizeDifference / 1920;

            qDebug() << "Each line has " << diffPerLine << "extra bytes";

            int x = 0;
            int y = 0;
            for (qint64 i = 0; i < sz; i++) {

                unsigned char data[4] = {0, 0, 0, 0};
                qint64 nRead = f.read(reinterpret_cast<char *>(data), 4);

                if (nRead < 4) {
                    throw std::runtime_error("I/O error");
                }

                i += nRead;

                QColor color(data[0], data[1], data[2], data[3]);
                img.setPixelColor(x, y, color);

                x++;
                if (x >= 1080) {
                    // move to next line, but read extra bytes before
                    char c;
                    for (int eb = 0; eb < diffPerLine; eb++) {
                        f.read(&c, 1);
                    }
                    x = 0;
                    y++;
                }
            }

            qDebug() << img.isNull();
            qDebug() << img.isGrayscale();
            qDebug() << img.size() << img.sizeInBytes() << sz;
            qDebug() << fn;

            saveOk = img.save(fn, "PNG");

            f.close();

            qDebug() << "save ok: " << saveOk;
        }
    } catch(std::exception &e) {
        qDebug() << e.what();
    }
}
