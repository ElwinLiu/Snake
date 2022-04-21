#include "snake.h"
#include <stdlib.h>
#define COL 40
#define ROW 24
#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

bool LessSort(fNode* a, fNode* b) { return (a->F < b->F); }

snake::snake(QWidget *parent)
    : QWidget(parent), foodCNT(0), score(0),PFood(true), Auto(false)
{
    ui.setupUi(this);
    this->setFixedSize(2000, 1200);
    //���ñ���
    this->setWindowTitle(tr("̰����"));
    
    //��ȡ������
    QFile file("word.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))//���ļ�
    {
        return;
    }
    QTextStream in(&file);
    QString line = in.readLine();
    while (!line.isNull())
    {
        words.push_back(line.split(" "));   //�Կո�ָ���š����ʡ����ԡ�����
        line = in.readLine();
    }

    generate_food(); //����ʳ������

    //���ñ���
    QPalette palette;
    palette.setBrush(QPalette::Background, QBrush(QPixmap("background.jpg").scaled(this->size())));
    this->setPalette(palette);

    //����
    score_label = new QLabel("����" + QString::number(score), this);
    ft.setPointSize(16);
    score_label->setFont(ft);
    score_label->setStyleSheet("color:red");
    score_label->setGeometry(20, 8, 200, 35);
    //score_lineEdit = new QLabel(QString::number(score), this);
    //ft.setPointSize(16);
    //score_lineEdit->setFont(ft);
    //score_lineEdit->setAlignment(Qt::AlignRight);
    //score_lineEdit->setGeometry(190, 8, 80, 35);
    //score_lineEdit->setStyleSheet("color:red");

    //������ʾ
    Chinese = words[score][2] + " " + words[score][3];
    word_label = new QLabel(Chinese, this);
    ft.setPointSize(16);
    word_label->setFont(ft);
    word_label->setStyleSheet("color:red");
    word_label->setGeometry(700, 8, 1000, 100);

    srand((unsigned int)(time(NULL)));
    snake_pos.resize(960); //Ԥ���Ű���ʮ���ڵ�
    //��ͷ���λ��
    snake_pos[0].first = qrand() % COL;    //�д洢
    snake_pos[0].second = qrand() % ROW;   //�д洢
    direction = qrand() % 4;               //�������
    timer = new QTimer(this);

    //��ť
    stopbtn = new QPushButton("ֹͣ", this);
    stopbtn->setGeometry(1800, 150, 100, 100);
    stopbtn->setStyleSheet("QPushButton{border-image: url(:/snake/stop.jpg);}");

    startbtn = new QPushButton(this);
    startbtn->setGeometry(1800, 300, 100, 100);
    startbtn->setStyleSheet("QPushButton{border-image: url(:/snake/start.png);}");

    autobtn = new QPushButton(this);
    autobtn->setGeometry(1800, 450, 100, 100);
    autobtn->setStyleSheet("QPushButton{border-image: url(:/snake/auto.png);}");


    connect(timer, SIGNAL(timeout()), this, SLOT(timeroutslot()));
    connect(startbtn, SIGNAL(clicked(bool)), this, SLOT(startbtn_click()));
    connect(stopbtn, SIGNAL(clicked(bool)), this, SLOT(stopbtn_click()));
    connect(autobtn, SIGNAL(clicked(bool)), this, SLOT(autobtn_click()));

}

void snake::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
 
    if (PFood)
    {
        /*generate_food(painter);*/
        PFood = false; //��ס�������ظ�����ʳ��
    }

    //���·�����Ŀ������
    QString tmp = "������" + QString::number(score);
    score_label->setText(tmp);
    QString text = "";
    for (int i = 3; i < words[score].size(); i++)
    {
        text = text + words[score][i] + " ";
    }
    word_label->setText(text);
    tmp = "";

    //����ʳ��
    QString apple = "Apple";
    for (int i = 0; i < words[score][1].length(); i++)
    {//�������ʣ�����������
        apple = apple + words[score][1][i] + ".png"; //����ͼƬ����
        painter.drawImage(QRectF(food_pos[i].first * 50, food_pos[i].second * 50, 50, 50), QImage(apple));
        apple = "Apple";
    }

    //��ͷ
    switch (direction)
    {
    case 0: //����
        painter.drawImage(QRectF(snake_pos[0].first * 50, snake_pos[0].second * 50, 50, 50), QImage("headup.png"));
        break;
    case 1: //����
        painter.drawImage(QRectF(snake_pos[0].first * 50, snake_pos[0].second * 50, 50, 50), QImage("headdown.png"));
        break;
    case 2: //����
        painter.drawImage(QRectF(snake_pos[0].first * 50, snake_pos[0].second * 50, 50, 50), QImage("headleft.png"));
        break;
    case 3: //����
        painter.drawImage(QRectF(snake_pos[0].first * 50, snake_pos[0].second * 50, 50, 50), QImage("headright.png"));
        break;
    default: break;
    }

    //��������
    for (int i = 1; i < foodCNT; i++)
    {
        //����
        if ((snake_pos[i].second == snake_pos[i - 1].second && snake_pos[i - 1].first < snake_pos[i].first &&
            snake_pos[i + 1].first == snake_pos[i].first && snake_pos[i + 1].second < snake_pos[i].second) ||
            (snake_pos[i - 1].first == snake_pos[i].first && snake_pos[i - 1].second < snake_pos[i].second &&
                snake_pos[i].second == snake_pos[i + 1].second && snake_pos[i + 1].first < snake_pos[i].first))
        {
            painter.drawImage(QRectF(snake_pos[i].first * 50, snake_pos[i].second * 50, 50, 50), QImage("br_corner.png"));
        }//����
        else if ((snake_pos[i].first == snake_pos[i - 1].first && snake_pos[i].second < snake_pos[i - 1].second &&
            snake_pos[i + 1].second == snake_pos[i].second && snake_pos[i + 1].first < snake_pos[i].first) ||
            (snake_pos[i - 1].second == snake_pos[i].second && snake_pos[i - 1].first < snake_pos[i].first &&
                snake_pos[i].first == snake_pos[i + 1].first && snake_pos[i].second < snake_pos[i + 1].second))
        {
            painter.drawImage(QRectF(snake_pos[i].first * 50, snake_pos[i].second * 50, 50, 50), QImage("tr_corner.png"));
        }//����
        else if ((snake_pos[i].first < snake_pos[i - 1].first && snake_pos[i].second == snake_pos[i - 1].second &&
            snake_pos[i + 1].second < snake_pos[i].second && snake_pos[i].first == snake_pos[i + 1].first) ||
            (snake_pos[i - 1].second < snake_pos[i].second && snake_pos[i].first == snake_pos[i - 1].first &&
                snake_pos[i].first < snake_pos[i + 1].first && snake_pos[i].second == snake_pos[i + 1].second))
        {
            painter.drawImage(QRectF(snake_pos[i].first * 50, snake_pos[i].second * 50, 50, 50), QImage("bl_corner.png"));
        }
        //����
        else if ((snake_pos[i].first == snake_pos[i - 1].first && snake_pos[i].second < snake_pos[i - 1].second &&
            snake_pos[i].second == snake_pos[i + 1].second && snake_pos[i].first < snake_pos[i + 1].first) ||
            (snake_pos[i].second == snake_pos[i - 1].second && snake_pos[i].first < snake_pos[i - 1].first &&
                snake_pos[i].first == snake_pos[i + 1].first && snake_pos[i].second < snake_pos[i + 1].second))
        {
            painter.drawImage(QRectF(snake_pos[i].first * 50, snake_pos[i].second * 50, 50, 50), QImage("tl_corner.png"));
        }/*ˮƽ����*/
        else if (snake_pos[i].second == snake_pos[i - 1].second && snake_pos[i].first != snake_pos[i - 1].first)
        {
            painter.drawImage(QRectF(snake_pos[i].first * 50, snake_pos[i].second * 50, 50, 50), QImage("h_body.png"));
        }/*��ֱ����*/
        else if (snake_pos[i].second != snake_pos[i - 1].second && snake_pos[i].first == snake_pos[i - 1].first)
        {
            painter.drawImage(QRectF(snake_pos[i].first * 50, snake_pos[i].second * 50, 50, 50), QImage("v_body.png"));
        }
    }
    //������β
    if (foodCNT > 0)
    {
        if ((snake_pos[foodCNT].first == snake_pos[foodCNT - 1].second && snake_pos[foodCNT].first < snake_pos[foodCNT - 1].first))
        {
            painter.drawImage(QRectF(snake_pos[foodCNT].first * 50, snake_pos[foodCNT].second * 50, 50, 50), QImage("tailright.png"));
        }
        else if (snake_pos[foodCNT].second == snake_pos[foodCNT - 1].second && snake_pos[foodCNT - 1].first < snake_pos[foodCNT].first)
        {
            painter.drawImage(QRectF(snake_pos[foodCNT].first * 50, snake_pos[foodCNT].second * 50, 50, 50), QImage("tailleft.png"));
        }
        else if ((snake_pos[foodCNT].first == snake_pos[foodCNT - 1].first && snake_pos[foodCNT].second < snake_pos[foodCNT - 1].second))
        {
            painter.drawImage(QRectF(snake_pos[foodCNT].first * 50, snake_pos[foodCNT].second * 50, 50, 50), QImage("taildown.png"));
        }
        else if ((snake_pos[foodCNT].first == snake_pos[foodCNT - 1].first && snake_pos[foodCNT - 1].second < snake_pos[foodCNT].second))
        {
            painter.drawImage(QRectF(snake_pos[foodCNT].first * 50, snake_pos[foodCNT].second * 50, 50, 50), QImage("tailup.png"));
        }
    }
}

void snake::keyPressEvent(QKeyEvent* event)
{
    switch (event->key())
    {
        // ���̵ķ����'��'���Сд'w/W'
    case Qt::Key_Up:
    case 'w':
    case 'W':
        direction = 0; break;
            // ���̵ķ����'��'���Сд's/S'
    case Qt::Key_Down:
    case 's':
    case 'S':
        direction = 1; break;
        // ���̵ķ����'��'���Сд'a/A'
    case Qt::Key_Left:
    case 'a':
    case 'A':
        direction = 2; break;
        // ���̵ķ����'��'���Сд'd/D'
    case Qt::Key_Right:
    case 'd':
    case 'D':
        direction = 3; break;
    }
}

void snake::generate_food()
{
    srand((unsigned int)(time(NULL)));
    for (int i = 0; i < words[score][1].length(); i++)
    {//�������ʣ�����������
        food_pos.push_back(qMakePair(qrand() % COL, qrand() % ROW));
    }
}

int snake::AStart(int startX, int startY)
{
    std::sort(openList.begin(), openList.end(), LessSort);
    fNode* min = openList[0];//ȡ��Fֵ��С��һ��
    bool dead = true; //����Ƿ񵽴��·
    if (food_pos[0].first == min->x && food_pos[0].second == min->y)
    {//�Ѿ��ҵ�·��
        return 0;
    }

    openList.erase(openList.begin());//��openList��ɾ��
    closeList.push_back(min);   //���뵽closeList��

    //�ھӽڵ�
    //���ھ�
    int x = min->x, y = min->y - 1;
    if (findIt(closeList, x, y) != NULL || y < 0 || isSelf(x, y) || WrongFood(x, y))
    {/*�����closeList�У����߲��ɷ��ʣ������*/}
    else
    {
        if (findIt(openList, x, y) == NULL)
        {//�������open list��
            fNode* up = new fNode;
            up->parent = min; //��ǰһ���ڵ���Ϊ����
            up->x = x;
            up->y = y;
            up->H = food_pos[0].first - x + food_pos[0].second - y;
            up->G = startX - x + startY - y;
            up->F = up->H + up->G;
            openList.push_back(up);
        }
        else
        {
            fNode* up = findIt(openList, x, y);
            if (min->G + 1 < up->G)
            {//���·�����ţ����¼���G��F
                up->G = min->G + 1;
                up->F = up->G + up->H;
                std::sort(openList.begin(), openList.end(), LessSort);
            }
        }
    }
    //���ھ�
    x = min->x; y = min->y + 1;
    if (findIt(closeList, x, y) != NULL || y >= ROW || isSelf(x, y) || WrongFood(x, y))
    {/*�����closeList�У����߲��ɷ��ʣ������*/
    }
    else
    {
        if (findIt(openList, x, y) == NULL)
        {//�������open list��
            fNode* down = new fNode;
            down->parent = min; //��ǰһ���ڵ���Ϊ����
            down->x = x;
            down->y = y;
            down->H = food_pos[0].first - x + food_pos[0].second - y;
            down->G = startX - x + startY - y;
            down->F = down->H + down->G;
            openList.push_back(down);
        }
        else
        {
            fNode* down = findIt(openList, x, y);
            if (min->G + 1 < down->G)
            {//���·�����ţ����¼���G��F
                down->G = min->G + 1;
                down->F = down->G + down->H;
                std::sort(openList.begin(), openList.end(), LessSort);
            }
        }
    }
    //���ھ�
    x = min->x - 1, y = min->y;
    if (findIt(closeList, x, y) != NULL || x < 0 || isSelf(x, y) || WrongFood(x, y))
    {/*�����closeList�У����߲��ɷ��ʣ������*/
    }
    else
    {
        if (findIt(openList, x, y) == NULL)
        {//�������open list��
            fNode* left = new fNode;
            left->parent = min; //��ǰһ���ڵ���Ϊ����
            left->x = x;
            left->y = y;
            left->H = food_pos[0].first - x + food_pos[0].second - y;
            left->G = startX - x + startY - y;
            left->F = left->H + left->G;
            openList.push_back(left);
        }
        else
        {
            fNode* left = findIt(openList, x, y);
            if (min->G + 1 < left->G)
            {//���·�����ţ����¼���G��F
                left->G = min->G + 1;
                left->F = left->G + left->H;
                std::sort(openList.begin(), openList.end(), LessSort);
            }
        }
    }
    //���ھ�
    x = min->x + 1, y = min->y;
    if (findIt(closeList, x, y) != NULL || x >= COL || isSelf(x, y) || WrongFood(x, y))
    {/*�����closeList�У����߲��ɷ��ʣ������*/
    }
    else
    {
        if (findIt(openList, x, y) == NULL)
        {//�������open list��
            fNode* right = new fNode;
            right->parent = min; //��ǰһ���ڵ���Ϊ����
            right->x = x;
            right->y = y;
            right->H = food_pos[0].first - x + food_pos[0].second - y;
            right->G = startX - x + startY - y;
            right->F = right->H + right->G;
            openList.push_back(right);
        }
        else
        {
            fNode* right = findIt(openList, x, y);
            if (min->G + 1 < right->G)
            {//���·�����ţ����¼���G��F
                right->G = min->G + 1;
                right->F = right->G + right->H;
                std::sort(openList.begin(), openList.end(), LessSort);
            }
        }
    }
    if (openList.isEmpty())
    {
        if (QMessageBox::question(this, tr("��ʾ"), tr("A* Ҳ����Ϊ��"),
            QMessageBox::Yes) == QMessageBox::Yes)
        {
            this->close();
            //delete this;    //�ͷ��Լ�����Դ
            return 1;         //�����������
        }
    }
    return 2;   //��Ҫ����ѭ��
    
}

bool snake::isSelf(int x, int y)
{
    for (int i = 1; i < snake_pos.size(); i++)
    {//�����ߵ�����ڵ�����
        if (x == snake_pos[i].first && y == snake_pos[i].second)
            return true;    //�ýڵ�����������
    }
    return false;
}

fNode* snake::findIt(QVector<fNode*> set, int x, int y)
{
    for (auto i = 0; i < set.size(); i++)
    {
        if (x == set[i]->x && y == set[i]->y)
        {
            return set[i];
        }
    }
    return NULL;
}

bool snake::WrongFood(int x, int y)
{
    for (int i = 1; i < food_pos.size(); i++)
    {
        if (x == food_pos[i].first && y == food_pos[i].second)
        {
            return true;
        }
    }
    return false;
}

void snake::startbtn_click()
{
    timer->start(150);
}

void snake::stopbtn_click()
{
    timer->stop();
}

void snake::autobtn_click()
{
    timer->start(150);
    for (int i = 0; i < openList.size(); i++)
    {
        delete openList[i];
    }
    for (int i = 0; i < closeList.size(); i++)
    {
        delete closeList[i];
    }
    for (int i = 0; i < abList.size(); i++)
    {
        delete abList[i];
    }
    keyMsg.clear();
    openList.clear();
    closeList.clear();
    abList.clear();
    Auto = true;

    fNode* start = new fNode;
    start->x = snake_pos[0].first;
    start->y = snake_pos[0].second;
    start->H = food_pos[0].first - start->x + food_pos[0].second - start->x;
    start->G = 0;
    start->F = start->H + start->G;
    openList.push_back(start);
    int s = AStart(start->x, start->y);
    while (s == 2)
    {
        s = AStart(start->x, start->y);
    }
    if (s == 0)
    {
        fNode* min = openList[0];//ȡ���յ�
        fNode* p;
        int dx, dy;

        while (!(min->x == start->x && min->y == start->y))
        {
            p = min->parent;
            dx = p->x - min->x;
            dy = p->y - min->y;
            if (dx == -1 && dy == 0)
            {
                keyMsg.push(RIGHT);
            }
            else if (dx == 1 && dy == 0)
            {
                keyMsg.push(LEFT);
            }
            else if (dx == 0 && dy == -1)
            {
                keyMsg.push(DOWN);
            }
            else if (dx == 0 && dy == 1)
            {
                keyMsg.push(UP);
            }
            min = p;
        }
        int l = 1;
    }
    if (s == 1)
    {
        return;
    }
}

void snake::timeroutslot()
{
    if (!keyMsg.isEmpty())
    {
        direction = keyMsg.top(); //����ȡ��ģ�������Ϣ

        while (true)
        {
            //���Υ��
            if ((isSelf(snake_pos[0].first + 1, snake_pos[0].second) ||
                WrongFood(snake_pos[0].first + 1, snake_pos[0].second) ||
                snake_pos[0].first + 1 >= COL
                ) && direction == RIGHT)
            {
                autobtn_click();
                if (!keyMsg.isEmpty())
                {
                    direction = keyMsg.top(); //ȡ�����º��ģ�������Ϣ
                }
                continue; //ѭ�����
            }
            else if ((isSelf(snake_pos[0].first - 1, snake_pos[0].second) ||
                WrongFood(snake_pos[0].first - 1, snake_pos[0].second) ||
                snake_pos[0].first - 1 < 0
                ) && direction == LEFT)
            {
                autobtn_click();
                if (!keyMsg.isEmpty())
                {
                    direction = keyMsg.top(); //ȡ�����º��ģ�������Ϣ
                }
                continue; //ѭ�����
            }
            else if ((isSelf(snake_pos[0].first, snake_pos[0].second - 1) ||
                WrongFood(snake_pos[0].first, snake_pos[0].second - 1) ||
                snake_pos[0].second - 1 < 0
                ) && direction == UP)
            {
                autobtn_click();
                if (!keyMsg.isEmpty())
                {
                    direction = keyMsg.top(); //ȡ�����º��ģ�������Ϣ
                }
                continue; //ѭ�����
            }
            else if ((isSelf(snake_pos[0].first, snake_pos[0].second + 1) ||
                WrongFood(snake_pos[0].first, snake_pos[0].second + 1) ||
                snake_pos[0].second + 1 >= ROW
                ) && direction == DOWN)
            {
                autobtn_click();
                if (!keyMsg.isEmpty())
                {
                    direction = keyMsg.top(); //ȡ�����º��ģ�������Ϣ
                }
                continue; //ѭ�����
            }
            if (!keyMsg.isEmpty()) keyMsg.pop(); //��ջ
            break; //����
        }
    }

        int n = 0;
        while (n < food_pos.size())
        {
            if (snake_pos[0] == food_pos[n])
            {
                if (n != 0)
                {
                    if (QMessageBox::question(this, tr("��ʾ"), tr("GAME OVER"),
                        QMessageBox::Yes) == QMessageBox::Yes)
                    {
                        this->close();
                        //delete this;    //�ͷ��Լ�����Դ
                        return;         //�����������
                    }
                }
                foodCNT++;
                //ɾ����ǰ�����ĸ
                food_pos.erase(food_pos.begin());
                words[score][1].remove(n, 1);
                if (food_pos.isEmpty())
                {//���ʳ�ﱻ�Թ���
                    score++;    //��һ��
                    for (int i = 0; i < words[score][1].length(); i++)
                    {//����һ�����ʣ��������������
                        QPair<int, int> tmp = qMakePair(qrand() % COL, qrand() % ROW);
                        if (tmp == snake_pos[0])
                        {//������������ͷ��������������
                            srand((unsigned int)(time(NULL)));
                            i--;
                            continue;
                        }
                        food_pos.push_back(tmp);
                    }
                    PFood = true;
                }
            }
            n++;
        }

        if (keyMsg.isEmpty() && Auto)
        {
            autobtn_click();
        }

        //�ı��ߵ�ÿһ����������
        for (int i = foodCNT; i > 0; i--)
        {
            snake_pos[i] = snake_pos[i - 1];
        }

        //��ͷ����ı�
        switch (direction)
        {
        case 0:
            snake_pos[0].second--;
            break;
        case 1:
            snake_pos[0].second++;
            break;
        case 2:
            snake_pos[0].first--;
            break;
        case 3:
            snake_pos[0].first++;
            break;
        default: break;
        }

        //�߽߱���
        if (snake_pos[0].first < 0 || snake_pos[0].first >= COL || snake_pos[0].second < 0 || snake_pos[0].second >= ROW)
        {
            if (QMessageBox::question(this, tr("��ʾ"), tr("������ˣ�"),
                QMessageBox::Yes) == QMessageBox::Yes)
            {
                this->close();
                //delete this;    //�ͷ��Լ�����Դ
                return;         //�����������
            }
        }

        //�Ե��Լ����
        for (auto it = snake_pos.begin(); it != snake_pos.end(); it++)
        {
            if (snake_pos[0] == *it && it != snake_pos.begin())
            {
                if (QMessageBox::question(this, tr("��ʾ"), tr("��Ե������Լ���"),
                    QMessageBox::Yes) == QMessageBox::Yes)
                {
                    this->close();
                    //delete this;    //�ͷ��Լ�����Դ
                    return;         //�����������
                }
            }
        }
    this->update();
}