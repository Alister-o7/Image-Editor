#include "SliderDialog.h"

#include <QLabel>
//#include <QSlider>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QDebug>
#include <QString>

//heap is managed by parent Widget (no leak), implementation with smart ptr is not suitable in this case.
//objects are used over the scope of this class.

SliderDialog::SliderDialog(const QList<QString>& fields, int min, int max, int step, QWidget *parent) : QDialog(parent),
    fields(fields), min(min), max(max), step(step)
{
    QFormLayout* layout = new QFormLayout(this);

    for(const auto &field : fields) {
        title = field;
        label = new QLabel(field, this);
        QSlider* edit = new QSlider(Qt::Horizontal);
        edit->setMinimum(min);
        edit->setMaximum(max);
        edit->setSingleStep(step);
        edit->setValue(0);
        connect(edit, &QSlider::valueChanged, this, &SliderDialog::updateLabel);
        layout->addRow(label, edit);
        values << edit;
    }

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                                       Qt::Horizontal, this);

    layout->addWidget(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted,
            this, &SliderDialog::accept);

    connect(buttonBox, &QDialogButtonBox::rejected,
            this, &SliderDialog::reject);

    setLayout(layout);
}

void SliderDialog::updateLabel(int value) {
    label->setText(QString("%1: %2").arg(title).arg(value));
}

QList<int> SliderDialog::getFields(QWidget *parent, QList<QString> fields, int min, int max, int step, bool *ok)
{
    SliderDialog* dialog = new SliderDialog(fields, min, max, step, parent);

    QList<int> list;

    bool okInput = dialog->exec();

    if(okInput) {
        *ok = okInput;
        foreach(auto value, dialog->values) {
            list << value->value();
        }
    }

    dialog->deleteLater();
    return list;
}
