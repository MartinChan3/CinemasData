#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QPushButton>
#include <QLineEdit>
#include <QTableView>
#include "connection.h"
#include <QLabel>
#include <QComboBox>
#include <QSqlTableModel>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{   
    Q_OBJECT
public:
    enum {
      CNAME = 0,
      CID = 1,
      CMARK = 2,
      CADDRESS = 3,
      CSELLPRICE = 4,
      CREGIONID = 5,
      CREGIONNAME = 6
    };

    enum {
      FLAG_INC = 0,
      FLAG_DEC = 1
    };

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    bool mDBOpend;
    QTableView *mTableViewp;
    QPushButton *mPbQueryp, *mPbReview, *mPbAddp, *mPbDeletep, *mPbIncreasingSortp, *mPbDecreasingSortp;
    QList<QLabel*> mAddLabelList;
    QList<QLineEdit*> mAddLineEditList;
    QLineEdit *mSearchLineEditp;
    QMap<int, QString> mStrMap;
    QComboBox *mComboBoxTypep;
    QSqlTableModel *modelp;

    void initStrMap();
    void initUIAddGrp();
    void initUILayout();
    void initTableView(QSqlTableModel&);

    bool checkDBStatus();

    QStringList queryDB(QString queryStr, QString valStr = QString());

signals:
    void sigError(QString str);

    void sigCallSort(int);

public slots:
    void slotShowError(QString str);

    void slotPbQueryp();
    void slotReview();
    void slotSort(int);
    void slotCallAscendingSort();
    void slotCallDescendingSort();
    void slotDelete();
    void slotAdd();

};

#endif // MAINWINDOW_H
