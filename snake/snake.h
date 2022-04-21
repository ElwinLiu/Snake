#pragma execution_character_set("utf-8")

#include <QtWidgets/QWidget>
#include <qpainter.h>
#include <qvector.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qtextstream.h>
#include <qfile.h>
#include <qpair.h>
#include <QKeyEvent>
#include <qkeyeventtransition.h>
#include <qmessagebox.h>
#include <qstack.h>
#include <qtimer.h>
#include <qqueue.h>
#include "ui_snake.h"

struct fNode
{
    int x;//������
    int y;//������
    int F;//f����
    int G;
    int H;
    fNode* parent;
    fNode():x(-1), y(-1), F(INT_MAX), G(INT_MAX), H(INT_MAX),  parent(NULL){}
};


class snake : public QWidget
{
    Q_OBJECT

public:
    snake(QWidget *parent = Q_NULLPTR);
    void paintEvent(QPaintEvent*);  //��ͼ
    void keyPressEvent(QKeyEvent* event);   //������Ӧ
    void generate_food();   //����ʳ��
    int AStart(int startX, int startY); //A*�㷨���Զ�Ѱ·
    bool isSelf(int x, int y);
    fNode* findIt(QVector<fNode*> set, int x, int y);  //����Ƿ���ĳlist��
    bool WrongFood(int x, int y); //����Ƿ�Ե��˴����ʳ��
private:
    Ui::snakeClass ui;
    QLabel* score_label;
    QLabel* word_label; //��Ӧ������ʾ
    QLabel* score_lineEdit; //����
    int score;  //����˼������ʾ��м���
    int direction; //����
    int foodCNT; //�Ե���ʳ������
    QVector<QPair<int, int>> food_pos;//ʳ������
    QVector<QStringList> words;  //������
    QString last;       //��һ������
    QString Chinese;    //������ʾ
    bool PFood; //��־���Ƿ񻭳�����ʳ��
    QFont ft;
    QTimer* timer;
    QVector<QPair<int, int>> snake_pos;//�ߵĽڵ�����
    //QVector<QPair<int, int>> snake_pos_tmp;//��ʱ���ߵĽڵ�����
    QPushButton* startbtn;
    QPushButton* stopbtn;
    QPushButton* autobtn;
    QStack<int> keyMsg; //A*����ó�����Ϣջ
    QVector<fNode*> openList;  //����⼯��
    QVector<fNode*> abList;    //��������
    QVector<fNode*> closeList; //�����ϼ���
    bool Auto;
private slots:
    void timeroutslot();
    void startbtn_click();  //��ʼ
    void stopbtn_click();   //��ͣ
    void autobtn_click();//�Զ�
};
