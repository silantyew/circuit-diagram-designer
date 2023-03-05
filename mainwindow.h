#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidgetItem>
#include <QList>
#include <QResizeEvent>
#include "circuitdiagrameditor.h"
#include "electroniccomponent.h"
#include "wire.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void placeNewComponentFromList(QTreeWidgetItem *item, int column);
    void searchTextFieldEdited(const QString &searchString);
    void createNewDiagram();
    void saveDiagramAs();
    void saveDiagram();
    void loadDiagram();
    void exportDiagramAsImage();
    void printDiagram();
    //void showHelp();
    void showAbout();
    void showAboutQt();

private:
    Ui::MainWindow *ui;
    CircuitDiagramEditor* viewer;
    ElectronicComponent* test;
    Wire* wire;
    QList<const ElectronicComponent*> sampleComponentsList;
    QString exportFilename;

    bool loadComponentsLibrary(); // false if failed
};
#endif // MAINWINDOW_H
