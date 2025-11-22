#include <QApplication>
#include <QWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <cmath>
#include <stdexcept>
#include <stack>
#include <sstream>

// تبدیل درجه به رادیان
double toRad(double x, bool deg){
    return deg ? x * M_PI / 180.0 : x;
}

// یک ارزیاب ساده برای چهار عمل اصلی
double evalExpr(const std::string &expr){
    std::istringstream iss(expr);
    std::stack<double> values;
    std::stack<char> ops;

    auto applyOp = [&](char op){
        double b = values.top(); values.pop();
        double a = values.top(); values.pop();
        switch(op){
            case '+': values.push(a+b); break;
            case '-': values.push(a-b); break;
            case '*': values.push(a*b); break;
            case '/': 
                if(b==0) throw std::runtime_error("Division by zero");
                values.push(a/b); 
                break;
        }
    };

    char ch;
    while(iss >> ch){
        if(isdigit(ch) || ch=='.'){
            iss.putback(ch);
            double val; iss >> val;
            values.push(val);
        }
        else if(ch=='+'||ch=='-'||ch=='*'||ch=='/'){
            while(!ops.empty() && (ops.top()=='*'||ops.top()=='/'||ch=='+'||ch=='-')){
                applyOp(ops.top());
                ops.pop();
            }
            ops.push(ch);
        }
    }
    while(!ops.empty()){
        applyOp(ops.top());
        ops.pop();
    }
    return values.top();
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QWidget win;
    win.setWindowTitle("Engineering Calculator");
    win.setFixedSize(400, 500);

    auto *layout = new QGridLayout(&win);

    auto *display = new QLineEdit();
    display->setReadOnly(true);
    display->setFixedHeight(50);
    display->setStyleSheet("font-size: 22px; padding: 6px;");

    auto *modeLabel = new QLabel("Mode:");
    auto *mode = new QComboBox();
    mode->addItem("RAD");
    mode->addItem("DEG");

    layout->addWidget(display, 0, 0, 1, 4);
    layout->addWidget(modeLabel, 1, 0);
    layout->addWidget(mode, 1, 1);

    QString buttons[6][4] = {
        {"7","8","9","/"},
        {"4","5","6","*"},
        {"1","2","3","-"},
        {"0",".","=","+"},
        {"sin","cos","tan","sqrt"},
        {"log","exp","pow","C"}
    };

    for(int r=0;r<6;r++){
        for(int c=0;c<4;c++){
            QPushButton *btn = new QPushButton(buttons[r][c]);
            btn->setFixedHeight(55);
            btn->setStyleSheet("font-size:18px;");
            layout->addWidget(btn, r+2, c);

            QObject::connect(btn, &QPushButton::clicked, [&, btn](){
                QString text = btn->text();
                bool degMode = (mode->currentText() == "DEG");

                if(text == "="){
                    try{
                        double result = evalExpr(display->text().toStdString());
                        display->setText(QString::number(result));
                    }
                    catch(std::exception &e){
                        display->setText(QString("ERROR: ") + e.what());
                    }
                }
                else if(text=="sin" || text=="cos" || text=="tan" || text=="sqrt" ||
                        text=="log" || text=="exp" || text=="pow"){
                    double val = display->text().toDouble();
                    if(text=="sin")  val = sin(toRad(val, degMode));
                    if(text=="cos")  val = cos(toRad(val, degMode));
                    if(text=="tan")  val = tan(toRad(val, degMode));
                    if(text=="sqrt") val = sqrt(val);
                    if(text=="log")  val = log(val);
                    if(text=="exp")  val = exp(val);
                    if(text=="pow")  val = pow(val,2);
                    display->setText(QString::number(val));
                }
                else if(text=="C"){
                    display->clear();
                }
                else{
                    display->setText(display->text() + text);
                }
            });
        }
    }

    win.show();
    return a.exec();
}
