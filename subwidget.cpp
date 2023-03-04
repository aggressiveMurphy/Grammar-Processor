#include "subwidget.h"
#include "ui_subwidget.h"
#include "mainwindow.h"
#include "process.h"
#include <string>
#include <iostream>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QDebug>
#include <QFile>
#include <QString>
#include <QPainter>
#include <QPaintEvent>
SubWidget::SubWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SubWidget)
{
    ui->setupUi(this);
}

SubWidget::~SubWidget()
{
    delete ui;
}

void SubWidget::on_pushButton_clicked()
{//点击“打开文法规则”按钮，读取文件内容，并将内容显示在textEdit
    QString file_path = QFileDialog::getOpenFileName(this,"选择一个代码文件", ".", "Text(*.txt);;all(*.*)");
    ui->lineEdit->setText(file_path);
    ui->textEdit->clear();
    QFile file(file_path);
    if(!file.open(QFile::ReadOnly | QFile::Text)) return;
    QTextStream in(&file);
    while(!in.atEnd())
    {//读取文件内容
        QString line = in.readLine();
        ui->textEdit->append(line);//展示文法规则
    }
    file.close();
}

void SubWidget::outputFirst()
{//将First集合元素显示在页面中
    //先清除上一次留下的内容
    ui->tableWidget->clear();
    //设置表格中所有单元格不可以编辑
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
   //设置表头样式
    ui->tableWidget->horizontalHeader()->setStyleSheet(
    "QHeaderView::section{background-color:rgb(225,225,225);"
    "font:10pt '宋体';color: black;};");
    //设置列数
    int column = 2;
    ui->tableWidget->setColumnCount(column);
    //表头内容:非终结符+First集合元素
    QStringList headList;
    headList<<"非终结符";
    headList<<"First集合元素";
    //设置水平表头
    ui->tableWidget->setHorizontalHeaderLabels(headList);
    //设置行数，行数为非终结符个数
    int row = this->processor->nonFinalWord.size();
    ui->tableWidget->setRowCount(row);
    //一行一行绘制
    for(int i = 0; i < row; i++)
    {//i为行号
        //每一行的第0列为非终结符
         ui->tableWidget->setItem(i,0,new QTableWidgetItem(QString::fromStdString(this->processor->nonFinalWord[i])));
         //设置表格内容居中
         ui->tableWidget->item(i,0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
         string s = "";
         s += "{";
         set<string> firstSet = this -> processor->first[this->processor->nonFinalWord[i]];
         set<string>::iterator it = firstSet.begin();
         bool flag = false;
         for(; it != firstSet.end(); it++)
         {
             if(flag)
             s += ",";
             s += *it;
             flag = true;
         }
         s += "}";
         ui->tableWidget->setItem(i,1,new QTableWidgetItem(QString::fromStdString(s)));
         //设置表格内容居中
         ui->tableWidget->item(i,1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
     }
}

void SubWidget::outputFollow()
{//将Follow集合元素显示在页面中
    //先清除上一次留下的内容
    ui->tableWidget_2->clear();
    //设置表格中所有单元格不可以编辑
    ui->tableWidget_2->setEditTriggers(QAbstractItemView::NoEditTriggers);
   //设置表头样式
    ui->tableWidget_2->horizontalHeader()->setStyleSheet(
    "QHeaderView::section{background-color:rgb(225,225,225);"
    "font:10pt '宋体';color: black;};");
    //设置列数
    int column = 2;
    ui->tableWidget_2->setColumnCount(column);
    //表头内容:非终结符+Follow集合元素
    QStringList headList;
    headList<<"非终结符";
    headList<<"Follow集合元素";
    //设置水平表头
    ui->tableWidget_2->setHorizontalHeaderLabels(headList);
    //设置行数，行数为非终结符个数
    int row = this->processor->nonFinalWord.size();
    ui->tableWidget_2->setRowCount(row);
    //一行一行绘制
    for(int i = 0; i < row; i++)
    {//i为行号
        //每一行的第0列为非终结符
         ui->tableWidget_2->setItem(i,0,new QTableWidgetItem(QString::fromStdString(this->processor->nonFinalWord[i])));
         //设置表格内容居中
         ui->tableWidget_2->item(i,0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
         string s = "";
         s += "{";
         set<string> followSet = this -> processor->follow[this->processor->nonFinalWord[i]];
         set<string>::iterator it = followSet.begin();
         bool flag = false;
         for(; it != followSet.end(); it++)
         {
             if(flag)
             s += ",";
             s += *it;
             flag = true;
         }
         s += "}";
         ui->tableWidget_2->setItem(i,1,new QTableWidgetItem(QString::fromStdString(s)));
         //设置表格内容居中
         ui->tableWidget_2->item(i,1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
     }
}

void SubWidget::Transform1()
{//判断是否为1型文法
    for(size_t i = 0; i < this->processor1->grammar.size(); i++)
    {//遍历每一条文法规则
        if(this->processor1->grammar[i].left.length()<=this->processor1->grammar[i].right[0].size())
        {//1型文法:文法规则的右边字符个数不小于左边字符个数
            this->processor1->grammar[i].level=1;
        }
    }
}

void SubWidget::Transform2()
{//判断是否为2型文法
    for(size_t i = 0; i < this->processor1->grammar.size(); i++)
    {//遍历每一条文法规则
        if(this->processor1->grammar[i].level==1)
        {//2型文法:在1型文法的基础上，文法规则的左部只有一个非终结符
            bool n=this->processor1->isFinalWord(this->processor1->grammar[i].left);
            if((this->processor1->grammar[i].left.length()==1)&&(n==false))
            {
                this->processor1->grammar[i].level=2;
            }
        }
    }
}

void SubWidget::Transform3()
{//判断是否为3型文法
    /*在2型文法的基础上，文法规则右边的字符数不超过2个，
     * 要么是一个单终结符,要么是一个终结符加一个非终结符，
     * 且非终结符要么都在终结符左边，要么都在终结符右边*/
    int l=0;//判断是否都是左递归
    int r=0;//判断是否都是右递归
    for(size_t i = 0; i < this->processor1->grammar.size(); i++)
    {//遍历每一条文法规则，对右部为单一终结符的规则先赋level=3，标记左递归和右递归
        bool n=this->processor1->isFinalWord(this->processor1->grammar[i].right[0][0]);
        int irsize=this->processor1->grammar[i].right[0].size();
        if(this->processor1->grammar[i].level==2)
        {
            if((irsize==1)&&(n==true))
            {//单一终结符
                this->processor1->grammar[i].level=3;
            }
            if((irsize==2)&&(n==false))
            {//左递归：如A->Ax
                l=1;
                this->processor1->grammar[i].t=7;//记为左递归类型
            }
            if((irsize==2)&&(n==true))
            {//右递归：如x->xU
                r=1;
                this->processor1->grammar[i].t=8;//记为右递归类型
            }
        }
    }
    if(l==1&&r==0)
    {//都为左递归
        for(size_t i = 0; i < this->processor1->grammar.size(); i++)
        {//每一条左递归规则都赋level=3
            if(this->processor1->grammar[i].t==7)
            {
                this->processor1->grammar[i].level=3;
            }
        }
    }
    if(l==0&&r==1)
    {//都为右递归
        for(size_t i = 0; i < this->processor1->grammar.size(); i++)
        {//每一条右递归规则都赋level=3
            if(this->processor1->grammar[i].t==8)
            {
                this->processor1->grammar[i].level=3;
            }
        }
    }
}

bool SubWidget::isTransform3()
{
    int flag=0;
    for(size_t i = 0; i < this->processor1->grammar.size(); i++)
    {//遍历每一条文法规则
        if(this->processor1->grammar[i].level==3)
        {
            flag=1;
        }
        else
        {
            flag=0;
            break;
        }
    }
    if(flag==1)
    {
        return true;
    }else
    {
        return false;
    }
}

bool SubWidget::isTransform2()
{//判断是不是为2型文法
    int flag=0;
    for(size_t i = 0; i < this->processor1->grammar.size(); i++)
    {//遍历每一条文法规则
        if(this->processor1->grammar[i].level==2||this->processor1->grammar[i].level==3)
        {
            flag=1;
        }
        else
        {
            flag=0;
            break;
        }
    }
    if(flag==1)
    {
        return true;
    }else
    {
        return false;
    }
}
bool SubWidget::isTransform1()
{//判断是不是为1型文法
    int flag=0;
    for(size_t i = 0; i < this->processor1->grammar.size(); i++)
    {//遍历每一条文法规则
        if(this->processor1->grammar[i].level==1||this->processor1->grammar[i].level==2||this->processor1->grammar[i].level==3)
        {
            flag=1;
        }
        else
        {
            flag=0;
            break;
        }
    }
    if(flag==1)
    {
        return true;
    }else
    {
        return false;
    }
}
void SubWidget::GrammarToExp1()
{//正规文法到正则表达式的转换1
    /*合并文法中形如S->aS S->bS为S->aS|bS;
     * S->Sa S->Sb为S->Sa|Sb;
     * S->aB S->bB为S->aB|bB;
     * S->Ba S->Bb为S->Ba|Bb;
     * S->a S->b为S->a|b ;
     * 即合并左部相同、右部第一个为非终结符且相同的文法规则，
     * 合并左部相同、右部第二个为非终结符且相同的文法规则，
     * 合并左部相同、右部为单个终结符的文法规则 */
    int i,j;
    int n=this->processor1->grammar.size();
    for(i=0;i<n-1;i++)
    {
        for(j=i+1;j<n;j++)
        {//取两条文法规则来进行处理
            int irsize=processor1->grammar[i].right[0].size();
            int jrsize=processor1->grammar[j].right[0].size();
            vector<vector<string>>::iterator it1 = processor1->grammar[j].right.begin();
            vector<vector<string>>::iterator it2 = processor1->grammar[j].right.begin();
            if((processor1->grammar[i].left!="\0")&&(irsize==2)&&(jrsize==2)&&(processor1->grammar[i].left==this->processor1->grammar[j].left)&&(this->processor1->grammar[i].left==processor1->grammar[i].right[0][0]&&this->processor1->grammar[j].left==processor1->grammar[j].right[0][0])&&(processor1->grammar[i].right[0][0]==processor1->grammar[j].right[0][0]))
            {//合并左部相同、右部第一个为非终结符且相同的文法规则
               this->processor1->grammar[i].t=1;//记录合并后的文法规则为1型，即Sa|Sb这种类型
               vector<string> r;//第i条规则的右部变成Sa|Sb,即right这个二维数组中有两行:[Sa],[Sb]
               r.push_back(it1->at(0));
               r.push_back(it1->at(1));
               processor1->grammar[i].right.push_back(r);//插入新行
               this->processor1->grammar[j].left="\0";//标志第j条规则是被合并的
            }
            if((this->processor1->grammar[i].left!="\0")&&(irsize==2)&&(jrsize==2)&&(this->processor1->grammar[i].left==this->processor1->grammar[j].left)&&(this->processor1->grammar[i].left==processor1->grammar[i].right[0][1]&&this->processor1->grammar[j].left==processor1->grammar[j].right[0][1])&&(processor1->grammar[j].right[0][1]==processor1->grammar[j].right[0][1]))
            {//合并左部相同、右部第二个为非终结符且相同的文法规则
               this->processor1->grammar[i].t=2;//记录合并后的文法规则为2型，即aS|bS这种类型
               vector<string> r;//第i条规则的右部变成aS|bS,即right这个二维数组中有两行:[aS],[bS]
               r.push_back(it1->at(0));
               r.push_back(it1->at(1));
               processor1->grammar[i].right.push_back(r);//插入新行
               this->processor1->grammar[j].left="\0";//标志第j条规则是被合并的
            }
            if((this->processor1->grammar[i].left!="\0")&&(this->processor1->grammar[i].left==this->processor1->grammar[j].left)&&(irsize==1)&&(jrsize==1))
            {//合并左部相同、右部为单个终结符的文法规则
                this->processor1->grammar[i].t=3;//记录合并后的文法规则为3型，即a|b这种类型
                vector<string> r;
                r.push_back(it1->at(0));
                processor1->grammar[i].right.push_back(r);//插入新行
                this->processor1->grammar[j].left="\0";//标志第j条规则是被合并的
            }
        }
    }
    qDebug()<<"GrammarToExp1";
    GrammarToExp2();
}

void SubWidget::GrammarToExp2()
{//正规文法到正则表达式的转换2
    /*S->Sa|Sb变为S->S(a|b);
     * S->Ba|Bb变为S->B(a|b);
     * 将S->aS|bS变为S->(a|b)S;
     * S->aB|bB变为S->(a|b)B;*/
    int i,j;
    int n=this->processor1->grammar.size();
    for(i=0;i<n;i++)
    {
        vector<vector<string>>::iterator it1 = this->processor1->grammar[i].right.begin();
        if((this->processor1->grammar[i].left!="\0")&&(this->processor1->grammar[i].t==1))
        {//将S->Sa|Sb变为S->S(a|b);S->Ba|Bb变为S->B(a|b);
            int row=this->processor1->grammar[i].right.size();//获取行数
            string s="(";
            for(j=0;j<row-1;)
            {//第i条规则的右部变成S(a|b|...),即right这个二维数组中前两行为:[S],[(a|b|...)]
                s+=it1[j][1];
                s+="|";
                it1[j][1]="";
                j++;
            }
            if(j==row-1)
            {
                s+=it1[j][1];
                s+=")";
            }
            it1[1][0]=s;
            this->processor1->grammar[i].t=4;
        }
        if((this->processor1->grammar[i].left!="\0")&&(this->processor1->grammar[i].t==2))
        {//将S->aS|bS变为S->(a|b)S;S->aB|bB变为S->(a|b)B;
            int row=this->processor1->grammar[i].right.size();//获取行数
            string s="(";
            for(j=0;j<row-1;)
            {//第i条规则的右部变成(a|b|...)S,即right这个二维数组中前两行为:[(a|b|...)],[S]
                s+=it1[j][0];
                s+="|";
                j++;
            }
            if(j==row-1)
            {
                s+=it1[j][0];
                s+=")";
            }
            it1[0][0]=s;
            it1[1][0]=it1[0][1];
            it1[0][1]="";
            this->processor1->grammar[i].t=4;
        }
    }
    qDebug()<<"GrammarToExp2";
    GrammarToExp3();
}

bool SubWidget::strCompare(string a,string b)
{//换位字符串的比较，换位字符串指长度相同，字符的类型和个数相同，但字符位置不同的字符串
    if (a.size() != b.size()) return false;
        int Count[256];
        memset(Count, 0, sizeof(Count));

        for (unsigned int i=0; i<a.size(); ++i)
        {
            ++Count[a[i]];
            --Count[b[i]];
        }

        for (unsigned int i=0; i<a.size(); ++i)
        {
            if (Count[a[i]]!=0) return false;
        }
}

void SubWidget::GrammarToExp3()
{//正规文法到正则表达式的转换3
    /*合并类似S->a|b S->S(a|b) 为S->(a|b)*
     * S->a|b S->(a|b)S 为S->(a|b)**/
    int i,j;
    int n=this->processor1->grammar.size();
    for(i=0;i<n-1;i++)
    {
        for(j=i+1;j<n;j++)
        {//取两条文法规则来进行处理
            vector<vector<string>>::iterator it1 = this->processor1->grammar[i].right.begin();
            vector<vector<string>>::iterator it2 = this->processor1->grammar[j].right.begin();
            if((this->processor1->grammar[i].left!="\0")&&(this->processor1->grammar[i].t==3)&&(this->processor1->grammar[j].t==4)&&(this->processor1->grammar[i].left==this->processor1->grammar[j].left))
            {//规则i为S->a|b|...,即右边无非终结符的文法规则
                int row=this->processor1->grammar[i].right.size();//获取行数
                int k;
                string s="(";
                for(k=0;k<row-1;)
                {
                    s+=it1[k][0];
                    s+="|";
                    it1[k][0]="";
                    k++;
                }
                if(k==row-1)
                {
                    s+=it1[k][0];
                    s+=")";
                }
                if((this->strCompare(s,this->processor1->grammar[j].right[0][0])==true)||(this->strCompare(s,this->processor1->grammar[j].right[0][1])==true))
                {//规则j为S->(a|b|...)S或S->S(a|b|...)
                    s+="*";
                    it2[0][0]=s;
                    it2[0][1]="";
                    this->processor1->grammar[j].t=6;
                    this->processor1->grammar[j].left="\0";
                }
            }
        }
    }
    qDebug()<<"GrammarToExp3";
    GrammarToExp4();
}
void SubWidget::GrammarToExp4()
{//正规文法到正规式的转换4
    /*合并S->aA A->a|b 为S->a(a|b);
     * 合并S->Aa A->a|b 为S->(a|b)a*/
    int i,j;
    int n=this->processor1->grammar.size();
    for(i=0;i<n-1;i++)
    {
        for(j=i+1;j<n;j++)
        {//取两条文法规则来进行处理
            vector<vector<string>>::iterator it1 = this->processor1->grammar[i].right.begin();
            if((this->processor1->grammar[i].left!="\0")&&(this->processor1->grammar[i].t==3)&&(this->processor1->grammar[j].t==4)&&(this->processor1->grammar[i].left!=this->processor1->grammar[j].left))
            {//规则i为A->a|b|...,即右边无非终结符的文法规则
                int row=this->processor1->grammar[i].right.size();//获取行数
                int k;
                string s="(";
                for(k=0;k<row-1;)
                {
                    s+=it1[k][0];
                    s+="|";
                    it1[k][0]="";
                    k++;
                }
                if(k==row-1)
                {
                    s+=it1[k][0];
                    s+=")";
                }
                if(this->processor1->grammar[i].left==this->processor1->grammar[i].right[0][1])
                {//规则j为S->aA
                    it1[0][1]=s;
                    this->processor1->grammar[j].t=5;
                    this->processor1->grammar[j].left="\0";
                }
                if(this->processor1->grammar[i].left==this->processor1->grammar[i].right[0][0])
                {//规则j为S->Aa
                    it1[0][0]=s;
                    this->processor1->grammar[j].left="\0";
                }
            }
        }
    }
    qDebug()<<"GrammarToExp4";
    GrammarToExp();
}
void SubWidget::GrammarToExp()
{//正规文法到正规式的转换5
    //进行最后的合并和输出
    string s="";
    int flag=0;
    for( size_t i= 0; i < this->processor1->grammar.size(); i++)
    {//判断是否所有文法右边均为终结符
        qDebug()<<this->processor1->grammar[i].right[0].size();
        int irsize=this->processor1->grammar[i].right[0].size();
        if(irsize==1)
        {//右边为单一终结符
            flag=0;
        }
        if((this->processor1->grammar[i].t==3)||(this->processor1->grammar[i].t==5)||(this->processor1->grammar[i].t==6))
        {
            flag=0;
        }
        if(this->processor1->grammar[i].right[0].size()==2)
        {//右边为两个字符，含有非终结符时
            flag=1;
            qDebug()<<"nonfinal";
            break;
        }
    }
    if(flag==0)
    {//所有文法右边均为终结符,合并输出
        size_t i= 0;
        for( ; i < (this->processor1->grammar.size()-1); i++)
        {
            qDebug()<<this->processor1->grammar[i].right[0].size();
            s+=this->processor1->grammar[i].right[0][0];
            s+="|";
        }
        if(i==this->processor1->grammar.size()-1)
        {
            s+=this->processor1->grammar[i].right[0][0];
        }
    }
    else
    {//文法右边含有非终结符时，循环
        GrammarToExp1();
    }
    qDebug()<<QString::fromStdString(s)<<endl;
    qDebug()<<"GrammarToExp";
}


void SubWidget::outputFA()
{//将正规文法转换成的FA状态转换表展示出来
    //将正规文法的终结符作为NFA状态
    //建立弧
}

void SubWidget::on_pushButton_2_clicked()
{//点击“保存文法规则”按钮，将处理好的文法规则保存在用户指定的文件中
    QString file_path = QFileDialog::getSaveFileName(this,"保存文件","","Text(*.txt);;all(*.*)");
    QFile f1(file_path);
    if(!f1.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text)){
        QMessageBox::warning(this,tr("错误"),tr("打开文件失败,数据保存失败"));
        return ;
    }
    else{
        QTextStream stream(&f1);
        QString line=ui->textEdit_4->toPlainText();
        stream <<line<< endl;
    }
    f1.close();
}

void SubWidget::on_pushButton_3_clicked()
{//点击“处理”按钮，进行文法问题的处理
    std::string analyzerFilePath = ui->lineEdit->text().toStdString();
    //未输入文法
    if(analyzerFilePath.empty())
    {
        QMessageBox::about(NULL,"Warning",
                           "Please select a file as an input");
        return;
    }
    processor = new Processor(analyzerFilePath);
    //输入到日志中
    processor->log << "-------------原本语法如下---------------" << '\n';
    processor->printGrammars();
    //去除左递归
    processor->dealLeftRecursion();
    //将去除左递归后的文法规则输出到文本框中
    QTextEdit *textEdit1 = this->ui->textEdit_2;
    textEdit1->setText("");
    for(size_t i = 0; i < this->processor->grammar.size(); i++)
    {
        string str="";
        const string &left = this->processor->grammar[i].left;
        const vector<vector<string>> &right = this->processor->grammar[i].right;
        str+=left;
        str+="->";
        for(size_t i = 0; i < right[0].size(); i++)
        {
            str+=right[0][i];
            textEdit1->append(QString::fromStdString(str));
        }
     }
    //输入到日志中
    processor->log << "-------------去除左递归后---------------" << '\n';
    processor->printGrammars();
    //化简文法
    processor->simplify();
    //将化简后的文法规则输出到文本框中
    textEdit1 = this->ui->textEdit_3;
    textEdit1->setText("");
    for(size_t i = 0; i < this->processor->grammar.size(); i++)
    {
        string str="";
        const string &left = this->processor->grammar[i].left;
        const vector<vector<string>> &right = this->processor->grammar[i].right;
        str+=left;
        str+="->";
        for(size_t i = 0; i < right[0].size(); i++)
        {
            str+=right[0][i];
            textEdit1->append(QString::fromStdString(str));
        }
     }
    //输入到日志中
    processor->log << "-------------去除多余项后---------------" << '\n';
    processor->printGrammars();
    //去除左公因子
    processor->dealLeftCommonFacotr();
    //将去除左公因子后的文法规则输出到文本框中
    textEdit1 = this->ui->textEdit_4;
    textEdit1->setText("");
    for(size_t i = 0; i < this->processor->grammar.size(); i++)
    {
        string str="";
        const string &left = this->processor->grammar[i].left;
        const vector<vector<string>> &right = this->processor->grammar[i].right;
        str+=left;
        str+="->";
        for(size_t i = 0; i < right[0].size(); i++)
        {
            str+=right[0][i];
            textEdit1->append(QString::fromStdString(str));
        }
     }
    //输入到日志中
    processor->log << "-------------去除左公因子后---------------" << '\n';
    processor->printGrammars();
    //求First集合
    processor->getFirst();
    //求Follow集合
    processor->getFollow();
    //关闭输入流
    processor->log.close();
    //输出First集合
    this->outputFirst();
    //输出Follow集合
    this->outputFollow();
    //判断文法类型
    processor1 = new Processor(analyzerFilePath);//重新读取文法
    this->Transform1();
    this->Transform2();
    this->Transform3();
    this->ui->textEdit_5->clear();
    if(this->isTransform3()==true)
    {//3型文法
        this->ui->textEdit_5->setText("该文法为3型文法");
    }
    if(this->isTransform2()==true&&this->isTransform3()==false)
    {//2型文法
        this->ui->textEdit_5->setText("该文法为2型文法");
    }
    if(this->isTransform1()==true&&this->isTransform2()==false&&this->isTransform3()==false)
    {//1型文法
        this->ui->textEdit_5->setText("该文法为1型文法");
    }
    if(this->isTransform1()==false&&this->isTransform2()==false&&this->isTransform3()==false)
    {//0型文法
        this->ui->textEdit_5->setText("该文法为0型文法");
    }
}

void SubWidget::on_pushButton_4_clicked()
{//点击“返回”按钮，关闭子窗口，切换到主窗口
    this->close();
    MainWindow *w=new MainWindow();
    w->show();
}
