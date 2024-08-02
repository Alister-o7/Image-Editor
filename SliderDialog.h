#ifndef SLIDERDIALOG_H
#define SLIDERDIALOG_H

#include <QSlider>
#include <QDialog>
#include <QObject>

class QLabel;

class SliderDialog : public QDialog
{
    Q_OBJECT
public:
    static QList<int> getFields(QWidget *parent, QList<QString> fields, int min, int max, int step, bool *ok);
    void updateLabel(int value);
private:
    explicit SliderDialog(const QList<QString>& fields, int min=0, int max=100, int step=1, QWidget *parent = nullptr);
    QList<QSlider* > values;
    QList<QString> fields;
    QLabel *label;
    QString title;
    int min;
    int max;
    int step;
};

#endif // SLIDERDIALOG_H
