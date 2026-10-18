#ifndef EXAMPLES_H
#define EXAMPLES_H

#include <QWidget>

namespace Ui {
class CExamples;
}

class CExamples : public QWidget
{
    Q_OBJECT

public:
    explicit CExamples(QWidget *parent = nullptr);
    ~CExamples();

private:
    Ui::CExamples *ui;
};

#endif // EXAMPLES_H
