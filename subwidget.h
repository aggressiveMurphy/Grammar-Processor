#ifndef SUBWIDGET_H
#define SUBWIDGET_H

#include <QWidget>
#include<QTextBrowser>
#include"processor.h"

namespace Ui {
class SubWidget;
}

class SubWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SubWidget(QWidget *parent = nullptr);
    ~SubWidget();

private slots:

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

private:
    Ui::SubWidget *ui;
    QString filePath;
    Processor *processor = NULL;
    Processor *processor1 = NULL;
    //获取指定输出
    string getProcessedGrammers();
    void outputFirst();
    void outputFollow();
    void outputLog();
    void outputFA();
    //判断文法类型
    void Transform1();
    void Transform2();
    void Transform3();
    bool isTransform1();
    bool isTransform2();
    bool isTransform3();
    //换位字符串的比较
    bool strCompare(string a,string b);
    //将正规文法转换成正则表达式
    void GrammarToExp1();
    void GrammarToExp2();
    void GrammarToExp3();
    void GrammarToExp4();
    void GrammarToExp();
    //将正则表达式转换成FA
};

#endif // SUBWIDGET_H
