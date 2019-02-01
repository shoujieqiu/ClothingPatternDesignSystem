#include <QtWidgets>
#include <QMessageBox>
#include <QtDebug>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "painterarea.h"
#include "mypath.h"
#include "dialog/dialogms.h"
#include "dialog/dialogmm.h"
#include "dialog/dialogdesign.h"
#include "data/mypathdata.h"
//#include <QHeaderView>


MainWindow::MainWindow() :
    QMainWindow(),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    dialogMS = new DialogMS(this);
    dialogMM = new DialogMM(this);
    dialogDesign = new DialogDesign(this);

    setWindowTitle(tr("Lancy PDS 2018"));

    painterArea = new PainterArea(this);
    setCentralWidget(painterArea);

    setDockNestingEnabled(true); //允许嵌套dock
    //记录所有的dock指针
    m_docks.append(ui->dockPointData);
    m_docks.append(ui->dockPathData);
    numberDocks = 2;

    labelScaling = new QLabel(tr("放大倍数 %1 ").arg(painterArea->scalingMulti));
    statusBar()->addPermanentWidget(labelScaling);

    modelPoints = new QStandardItemModel(this);
    modelPaths = new QStandardItemModel(this);

    modelPoints->setItem(0, 0, new QStandardItem(" "));
    modelPoints->setItem(0, 1, new QStandardItem(" "));
    modelPoints->setItem(0, 2, new QStandardItem(" "));
    modelPoints->setItem(0, 3, new QStandardItem(" "));
    ui->tablePoints->setModel(modelPoints);
    modelPoints->setHeaderData(0,Qt::Horizontal, tr("x坐标"));
    modelPoints->setHeaderData(1,Qt::Horizontal, tr("y坐标"));
    modelPoints->setHeaderData(2,Qt::Horizontal, tr("别名"));
    modelPoints->setHeaderData(3,Qt::Horizontal, tr("曲线相关"));
    ui->tablePaths->setModel(modelPaths);
    ui->tablePaths->horizontalHeader()->hide();
    ui->tablePaths->setEditTriggers(QAbstractItemView::NoEditTriggers);//设为只读
    ui->tablePaths->setSelectionMode(QAbstractItemView::SingleSelection);//只能单选

    connect(painterArea,SIGNAL(mouseCoordinateChanged()),this,SLOT(setStatusMouseCoordinate()));
    connect(painterArea,SIGNAL(scalingMultiChanged()),this,SLOT(setStatusScalingMulti()));
    connect(painterArea,SIGNAL(resetModel()),this,SLOT(resetModel()));
    connect(dialogMS,SIGNAL(typeSangChanged(int,int)),painterArea,SLOT(setTypeSang(int,int)));
    connect(ui->tablePaths->model(),SIGNAL(itemChanged(QStandardItem*)),painterArea,SLOT(changePath()));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete painterArea;
    delete dialogMS;
    delete dialogMM;

}

///
/// \brief 移除并隐藏所有的dock
///
void MainWindow::removeAllDock()
{
    for(int i=0;i<numberDocks;++i)
    {
        removeDockWidget(m_docks[i]);
    }
}
///
/// \brief 显示指定序号的dock
/// \param index 指定序号，如果不指定，则会显示所有
///
void MainWindow::showDock(const QList<int> &index)
{
    if (index.isEmpty())
    {
        for(int i=0;i<9;++i)
        {
            m_docks[i]->show();
        }
    }
    else
    {
        foreach (int i, index) {
            m_docks[i]->show();
        }
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    switch(QMessageBox::question(this,tr("提示"),tr("你确定退出?")))
    {
    case QMessageBox::Yes:
        event->accept();
        break;
    default:
        event->ignore();
        break;
    }
}

void MainWindow::setStatusMouseCoordinate(){
    this->statusBar()->showMessage(painterArea->stringTempStatus,0);
}

void MainWindow::setStatusScalingMulti(){
    labelScaling->setText(tr("放大倍数 %1 ").arg(painterArea->scalingMulti));
    this->statusBar()->showMessage("",0);
}

//根据myPathData修改tablePoints和tablePaths---------------
void MainWindow::resetModel(){
    MyPathData *data = painterArea->myPathData;
    int nPaths = data->numberPath, i,j=0,
        nPoints = data->numberPoint;
    for(i=0;i<nPoints;++i)
    {
        CurvePoint *curvePoint = data->pointData[i].point;
        if(curvePoint->pre!=nullptr)
        {
            setPointModel(j,curvePoint->pre); j++;
        }
        setPointModel(j,curvePoint); j++;
        while(curvePoint->next!=nullptr)
        {
            curvePoint = curvePoint->next;
            setPointModel(j,curvePoint);j++;
        }
    }
    for(i=0;i<nPaths;i++)
    {
        PathData path = data->pathData[i];
        QString type = path.isLine?tr("直线"):tr("曲线");
        QString point = data->stringOf(data->pointData[path.startPoint].point);
        modelPaths->setItem(i, 0, new QStandardItem(type));
        modelPaths->setItem(i, 1, new QStandardItem(point));
    }
    ui->tablePoints->resizeColumnsToContents();  //根据内容自动调整所有列的列宽
    ui->tablePaths->resizeColumnsToContents();
}

void MainWindow::setPointModel(int i,CurvePoint *curvePoint){
    MyPathData *data = painterArea->myPathData;
    QString x = QString::number(curvePoint->x,'f',2);
    QString y = QString::number(curvePoint->y,'f',2);
    QString name = data->findName(curvePoint->x,curvePoint->y);
    QString note = curvePoint->isFirst?tr("起点"):(curvePoint->isLast?tr("终点"):(curvePoint->isCtrlPoint?tr("锚点"):""));
    if(note==""){
        if(curvePoint->next!=nullptr)
            note=tr("途经点");
    }
    modelPoints->setItem(i, 0, new QStandardItem(x));
    modelPoints->setItem(i, 1, new QStandardItem(y));
    modelPoints->setItem(i, 2, new QStandardItem(name));
    modelPoints->setItem(i, 3, new QStandardItem(note));
}

void MainWindow::on_action_M_S_triggered()
{
    dialogMS->exec();
}

void MainWindow::on_action_F_S_triggered()
{
//    //截取painterArea
//    int aLeft = 60, aTop = 60,
//        aRight = aLeft+440+painterArea->pantsH/4,
//        aBottom = aTop+painterArea->pantsL+50;
//    qreal oldScalingMulti = painterArea->scalingMulti;
//    int oldIntUp = painterArea->intUp, oldIntLeft = painterArea->intLeft;
//    painterArea->scalingMulti = 5.0;
//    painterArea->intUp = 0;
//    painterArea->intLeft = 0;
//    painterArea->update();
//    QPixmap pixMap = painterArea->grab(QRect(5*aLeft,5*aTop,5*aRight,5*aBottom));
//    painterArea->scalingMulti = oldScalingMulti;
//    painterArea->intUp = oldIntUp;
//    painterArea->intLeft = oldIntLeft;
//    painterArea->update();

    //保获取路径
    QString filePath = QDir::currentPath() + "/" + painterArea->myPathData->name;
//    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
//                       filePath + ".dxf",
//                       tr("%1 Files (*.%2)")
//                       .arg(QString::fromLatin1("DXF"))
//                       .arg(QString::fromLatin1("dxf")));
//    qDebug()<<"save to: "<<fileName;

//    //保存为jpg文件
//    bool boolPixMapSaved = pixMap.save(fileName,"JPG");
//    if(boolPixMapSaved)
//        QMessageBox::information(nullptr,"Save","The jpg file has saved successfully.");

    //保存为dxf文件
    bool boolDXFSaved=painterArea->writeDXF();


    //输出txt文件
    if(painterArea->myPathData->saveTo(filePath+".txt") && boolDXFSaved)
        QMessageBox::information(nullptr,"Save","The jpg and txt files have saved successfully.");
    else
        QMessageBox::information(nullptr,"Save","The jpg and txt files failed to save!");
}

void MainWindow::on_action_M_M_triggered()
{
    if(dialogMM->exec()==QDialog::Accepted)
    {
        painterArea->pantsCrotchH=dialogMM->Cro;
        painterArea->pantsH=dialogMM->H;
        painterArea->pantsHeight=dialogMM->Height;
        painterArea->pantsL=dialogMM->L;
        painterArea->pantsW=dialogMM->W;
        painterArea->update();
    }
}

void MainWindow::on_tablePaths_clicked(const QModelIndex &index)
{
    int id = index.row();
    showPath(id);
}

void MainWindow::showPath(int id)
{
    PathData pathData = painterArea->myPathData->pathData[id];
    QPainterPath yellowPath;
    QList<QPointF> points;
    if(pathData.isLine)
    {
        CurvePoint *sp = painterArea->myPathData->pointData[pathData.startPoint].point;
        CurvePoint *ep = painterArea->myPathData->pointData[pathData.endPoint].point;
        yellowPath.moveTo(QPointF(sp->x,sp->y));
        yellowPath.lineTo(QPointF(ep->x,ep->y));
        painterArea->yellowPath = yellowPath;
        painterArea->setCenterToYellowPath();
        points<<QPointF(sp->x,sp->y)<<QPointF(ep->x,ep->y);
        showPoints_L(points);
    }
    else
    {
        CurvePoint *p = painterArea->myPathData->pointData[pathData.startPoint].point;
        QPointF startPoint=QPointF(p->x,p->y);
        CurvePoint *c1 = p->pre;
        QList<QPointF> points;
        points.append(QPointF(p->x,p->y));
        while(!p->next->isCtrlPoint)
        {
            p=p->next;
            points.append(QPointF(p->x,p->y));
        }
        MyPath path(painterArea);
        path.curveThrough_data(points,QPointF(c1->x,c1->y),QPointF(p->next->x,p->next->y));
        yellowPath = *(path.myPath);
        painterArea->yellowPath = yellowPath;
        painterArea->setCenterToYellowPath();
        showPoints_C(startPoint);
    }
    ui->labelPathLen->setText("Length:"+QString::number(yellowPath.length())+"mm");
    painterArea->update();
}

void MainWindow::on_tablePaths_activated(const QModelIndex &index)
{
    qDebug()<<"on_tablePaths_activated";
}

//从tablePoints中选择与点point对应的点。type:0所有位置符合的点，1直线上的点，2曲线上的点
void MainWindow::selectTablePoint(QPointF point,int type)
{
    ui->tablePoints->setSelectionMode(QAbstractItemView::MultiSelection);
    int i, numRows=modelPoints->rowCount();
    for(i=0;i<numRows;i++)
    {
        qreal x=modelPoints->item(i,0)->text().toDouble();
        qreal y=modelPoints->item(i,1)->text().toDouble();
        QString note=modelPoints->item(i,3)->text();
        qreal dx=x-point.x(), dy=y-point.y(), E=0.1;
        if(dx<E && dx>-E && dy>-E && dy<E)
        {
            if(type==1 && note==""){
                ui->tablePoints->selectRow(i);
                break;
            }
            else if(type==2 && note==tr("途经点")){
                ui->tablePoints->selectRow(i);
                if(modelPoints->item(i-1,3)->text()==tr("起点"))
                {
                    ui->tablePoints->selectRow(i-1);
                    ui->tablePoints->selectRow(i-2);
                }
                if(modelPoints->item(i+1,3)->text()==tr("终点"))
                {
                    ui->tablePoints->selectRow(i+1);
                    ui->tablePoints->selectRow(i+2);
                }
            }
            else if(type==0)
                ui->tablePoints->selectRow(i);
        }
    }
    ui->tablePoints->setSelectionMode(QAbstractItemView::ExtendedSelection);
}

void MainWindow::showPoints_L(QList<QPointF> points)
{
    painterArea->clearLabelPoints();
    ui->tablePoints->clearSelection();
    int numList=points.size(), i, j,
        numPointsData=painterArea->myPathData->numberPoint;
    bool b_equal=false;
    for(i=0;i<numList;i++)
    {
        QPointF p1 = points.takeAt(0);
        b_equal=false;
        //寻找pointData中与p1相等的点
        for(j=0;j<numPointsData;j++)
        {
            CurvePoint *p2 = painterArea->myPathData->pointData[j].point;
            if(p2->isFirst==false && MyPathData::equal(p1,p2,1.0)) //设置寻点的误差为1.0mm
            {
                b_equal=true; break;
            }
        }
        if(b_equal)
        {
            CurvePoint *cp=painterArea->myPathData->pointData[j].point;
            painterArea->setLabelPoint(j,cp);
            selectTablePoint(QPointF(cp->x,cp->y),1);
        }
    }
}

void MainWindow::showPoints_C(QPointF p1)
{
    painterArea->clearLabelPoints();
    ui->tablePoints->clearSelection();
    int j, numPointsData=painterArea->myPathData->numberPoint;
    for(j=0;j<numPointsData;j++)
    {
        CurvePoint *p2 = painterArea->myPathData->pointData[j].point;
        if(p2->isFirst==true && MyPathData::equal(p1,p2,1.0)) //设置寻点的误差为1.0mm
        {
            CurvePoint *cp=painterArea->myPathData->pointData[j].point;
            painterArea->setLabelPoint(j,cp);
            selectTablePoint(QPointF(cp->x,cp->y),2);
            while(cp->next->isCtrlPoint==false)
            {
                cp=cp->next;
                painterArea->setLabelPoint(-1,cp);  //没有存在于pointData数组中,id置为-1
                selectTablePoint(QPointF(cp->x,cp->y),2);
            }
            break;
        }
    }
}

void MainWindow::on_action_Design_triggered()
{
    dialogDesign->exec();
}

