#include "menupage.h"
#include "ui_menupage.h"
#include "xml.h"
extern int page_change;
extern QString file_str;
extern resolve_xml XML;

MenuPage::MenuPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MenuPage)
{
    ui->setupUi(this);
    QPixmap pixmap(":/new/china.png");

    ui->label->setPixmap(pixmap);

    ui->label->setScaledContents(true);
\
    connect(ui->chengdu, &QPushButton::clicked, this, &MenuPage::to_store_ways);
    connect(ui->shanghai, &QPushButton::clicked, this, &MenuPage::to_store_ways2);

}

void MenuPage::on_backButton_clicked()
{
    emit goBackToMainWindow();
}

void MenuPage::to_store_ways()
{
    file_str = ":/new/chengdu.osm";
    XML.imortant();
    emit storeWay();
}


void MenuPage::to_store_ways2()
{
    file_str = ":/new/shanghai_2.osm";
    XML.imortant();
    emit storeWay();
}
MenuPage::~MenuPage()
{
    delete ui;
}
