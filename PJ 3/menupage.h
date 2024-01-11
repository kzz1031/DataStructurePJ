#ifndef MENUPAGE_H
#define MENUPAGE_H

#include <QWidget>

namespace Ui {
class MenuPage;
}

class MenuPage : public QWidget
{
    Q_OBJECT

public:
    explicit MenuPage(QWidget *parent = nullptr);
    ~MenuPage();
    void on_backButton_clicked();
    void to_store_ways();
    void to_store_ways2();
signals:
    void goBackToMainWindow();
    void storeWay();
private:
    Ui::MenuPage *ui;
};

#endif // MENUPAGE_H


