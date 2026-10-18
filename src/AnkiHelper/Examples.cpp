#include "Examples.h"
#include "ui_Examples.h"

CExamples::CExamples(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CExamples)
{
    ui->setupUi(this);
}

CExamples::~CExamples()
{
    delete ui;
}
