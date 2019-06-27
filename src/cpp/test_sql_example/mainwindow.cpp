#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLayout>
#include <QSqlError>
#include <QMessageBox>
#include <QDesktopWidget>
#include <QSqlRecord>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), mDBOpend(false)
{
    ui->setupUi(this);

    connect(this, SIGNAL(sigError(QString)),
            this, SLOT(slotShowError(QString)));

    mDBOpend = createConnection();
    if (!mDBOpend)
    {
        emit sigError(QString::fromLocal8Bit("数据库未正确打开，请检查"));
    }

    //尺寸/居中显示
    QDesktopWidget *desktopWidget = QApplication::desktop();
    int screenWidth = desktopWidget->rect().width();
    int screenHeight = desktopWidget->rect().height();
    int winWidth = 1000;
    int winHeight = 500;
    this->setGeometry((screenWidth - winWidth) / 2,
                      (screenHeight - winHeight) / 2,
                      winWidth, winHeight);

    initStrMap();
    mTableViewp = new QTableView(this);
    mPbAddp = new QPushButton(QString::fromLocal8Bit("添加"), this);
    mPbQueryp = new QPushButton(QString::fromLocal8Bit("查询"), this);
    mPbReview = new QPushButton(QString::fromLocal8Bit("恢复"), this);
    mPbDeletep = new QPushButton(QString::fromLocal8Bit("删除"),this);
    mPbIncreasingSortp = new QPushButton(QString::fromLocal8Bit("增序"), this);
    mPbDecreasingSortp = new QPushButton(QString::fromLocal8Bit("降序"), this);
    mComboBoxTypep = new QComboBox(this);
    mSearchLineEditp = new QLineEdit(this);
    initUIAddGrp();

    initUILayout();

    modelp = new QSqlTableModel();
    initTableView(*modelp);

    connect(mPbQueryp, SIGNAL(clicked()),
            this, SLOT(slotPbQueryp()));
    connect(mPbReview, SIGNAL(clicked()),
            this, SLOT(slotReview()));

    connect(mPbIncreasingSortp, SIGNAL(clicked()),
            this, SLOT(slotCallAscendingSort()));
    connect(mPbDecreasingSortp, SIGNAL(clicked()),
            this, SLOT(slotCallDescendingSort()));
    connect(this, SIGNAL(sigCallSort(int)),
            this, SLOT(slotSort(int)));
    connect(mPbDeletep, SIGNAL(clicked()),
            this, SLOT(slotDelete()));
    connect(mPbAddp, SIGNAL(clicked()),
            this, SLOT(slotAdd()));
}

MainWindow::~MainWindow()
{
    if (modelp)
    {
        delete modelp;
        modelp = Q_NULLPTR;
    }
    delete ui;
}

//初始化映射表
void MainWindow::initStrMap()
{
    mStrMap.clear();
    mStrMap.insert(CID, "id");
    mStrMap.insert(CNAME, "name");
    mStrMap.insert(CMARK, "mark");
    mStrMap.insert(CADDRESS, "address");
    mStrMap.insert(CSELLPRICE, "sellPrice");
    mStrMap.insert(CREGIONID, "regionId");
    mStrMap.insert(CREGIONNAME, "regionName");
}

void MainWindow::initUIAddGrp()
{
    const int colSize = mStrMap.size();
    for (int i = 0; i < colSize; i++)
    {
        mAddLabelList.append(new QLabel(mStrMap[i],this));
        mAddLineEditList.append(new QLineEdit(this));
        mComboBoxTypep->addItem(mStrMap[i]);
    }

    mComboBoxTypep->addItem("All");
}

void MainWindow::initUILayout()
{
    QWidget *nCentralWidget = new QWidget(this);

    QVBoxLayout *topVLayout = new QVBoxLayout();
    topVLayout->addWidget(mComboBoxTypep, 1);
    topVLayout->addWidget(mSearchLineEditp, 3);
    topVLayout->addWidget(mPbQueryp, 1);
    topVLayout->addWidget(mPbReview, 1);
    topVLayout->addWidget(mPbIncreasingSortp, 1);
    topVLayout->addWidget(mPbDecreasingSortp, 1);

    QHBoxLayout *rightHLayout = new QHBoxLayout();
    for (int i = 0; i < mStrMap.size(); i++)
    {
        QVBoxLayout *tNewVLayout = new QVBoxLayout();
        tNewVLayout->addWidget(mAddLabelList[i], 1);
        tNewVLayout->addWidget(mAddLineEditList[i], 3);
        rightHLayout->addLayout(tNewVLayout, 1);
    }
    QVBoxLayout *tBotVLayout = new QVBoxLayout();
    tBotVLayout->addWidget( mPbAddp, 1);
    tBotVLayout->addWidget( mPbDeletep, 1);
    rightHLayout->addLayout(tBotVLayout, 1);

    QHBoxLayout *leftAllLayout = new QHBoxLayout();
    leftAllLayout->addLayout(topVLayout, 1);
    leftAllLayout->addWidget(mTableViewp, 5);

    QVBoxLayout *allLayout = new QVBoxLayout();
    allLayout->addLayout(leftAllLayout, 5);
    allLayout->addLayout(rightHLayout, 1);

    nCentralWidget->setLayout(allLayout);
    this->setCentralWidget(nCentralWidget);
}

void MainWindow::initTableView(QSqlTableModel &model)
{
    model.setTable("cinemas");
    model.setEditStrategy(QSqlTableModel::OnManualSubmit);
    if (!model.select())
        qDebug() << QString::fromLocal8Bit("数据库数据填入失败");

    //获取数据库表头字段
    QStringList headerList = queryDB("PRAGMA table_info(cinemas)", "name");
    for (int i = 0; i < headerList.size(); i++)
    {
        model.setHeaderData(i, Qt::Horizontal, headerList.at(i));
    }

    mTableViewp->setModel(&model);
    mTableViewp->setSortingEnabled(true);
    mTableViewp->setAlternatingRowColors(true); //交错颜色
    mTableViewp->setSelectionBehavior(QTableView::SelectRows);
}

bool MainWindow::checkDBStatus()
{
    if (!mDBOpend)
    {
        emit sigError("数据库未正确打开，请检查");
        return false;
    }
    else
        return true;
}

//查询数据库内容
QStringList MainWindow::queryDB(QString queryStr, QString valStr)
{
    QSqlQuery query;
    if (!mDBOpend)
    {
        qDebug() << QString::fromLocal8Bit("数据库尚未打开");
        return QStringList();
    }

    if (!query.exec(queryStr))
    {
        qDebug() << QString::fromLocal8Bit("数据库执行错误的条件为：") << query.lastError().text();
        return QStringList();
    }
    else
    {
        QStringList list;
        while (query.next())
        {
            if (valStr.isEmpty())
                list.append(query.value(0).toString());
            else
                list.append(query.value(valStr).toString());
        }
        return list;
    }
}

void MainWindow::slotShowError(QString str)
{
    QMessageBox::warning(this, QString::fromLocal8Bit("错误"), str);
}

void MainWindow::slotPbQueryp()
{
    if (!checkDBStatus())
        return;

    QString str = mSearchLineEditp->text();
    int indexOfComboBox = mComboBoxTypep->currentIndex();
    QString optionStr;
    if (mStrMap.contains(indexOfComboBox))
    {
        optionStr = mStrMap[indexOfComboBox];
    }
    else
        optionStr = "";

    QString totalQueryStr = optionStr + " LIKE " + QString("'%%1%'").arg(str); //包含性选择
    qDebug() << totalQueryStr;
    modelp->setFilter(totalQueryStr);
    modelp->select();
}

void MainWindow::slotReview()
{
    modelp->setTable("cinemas");
    modelp->select();
}

void MainWindow::slotSort(int sortType)
{
    int indexOfComboBox = mComboBoxTypep->currentIndex();
    if (!mStrMap.contains(indexOfComboBox))
        return;

    modelp->sort(indexOfComboBox,sortType == FLAG_INC ? Qt::AscendingOrder : Qt::DescendingOrder);
    modelp->select();
}

void MainWindow::slotCallAscendingSort()
{
    emit sigCallSort(FLAG_INC);
}

void MainWindow::slotCallDescendingSort()
{
    emit sigCallSort(FLAG_DEC);
}

void MainWindow::slotDelete()
{
    int curRow = mTableViewp->currentIndex().row();
    qDebug() << curRow;
    if (curRow >= 0)
    {
        modelp->removeRow(curRow);
    }

    int ok = QMessageBox::warning(this, QString::fromLocal8Bit("删除当前该行"), QString::fromLocal8Bit("是否确定"),
                                  QMessageBox::Yes, QMessageBox::No);
    if (ok == QMessageBox::Yes)
    {
        modelp->submitAll();
    }
    else
        modelp->revertAll();

}

void MainWindow::slotAdd()
{
    for (int i = 0; i < mAddLineEditList.size(); i++)
    {
        if (mAddLineEditList.at(i)->text().isEmpty())
        {
            emit sigError(QString::fromLocal8Bit("请将信息补全再添加"));
            return;
        }
    }

    QString cIDStr = mAddLineEditList.at(CID)->text();
    QString cRegionIdStr = mAddLineEditList.at(CREGIONID)->text();
    bool tOk1, tOk2;
    int tCID = cIDStr.toInt(&tOk1);
    int tCRID = cRegionIdStr.toInt(&tOk2);
    if (!tOk1 || !tOk2)
    {
        emit sigError(QString::fromLocal8Bit("没有输入正确的id，请检查"));
        return;
    }

    //20190626
    QSqlRecord newRecord = modelp->record();
    for (int i = 0; i < mAddLineEditList.size(); i++)
    {
        QString s = mAddLineEditList.at(i)->text();
        bool tOk;
        int tint = s.toFloat(&tOk);
        QVariant var = tOk ? QVariant(tint) : QVariant(s);
        newRecord.setValue(mStrMap[i], var);
    }

    if (modelp->insertRecord(modelp->rowCount(), newRecord))
    {
        QMessageBox::information(this, QString::fromLocal8Bit("插入成功"), QString::fromLocal8Bit("记录插入成功"));
    }
    else
        emit sigError(QString::fromLocal8Bit("插入失败"));

    modelp->submitAll();
}
