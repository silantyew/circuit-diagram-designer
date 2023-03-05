#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QXmlStreamReader>
#include <QMessageBox>
#include <QFileDialog>
#include <QPrinter>
#include <QPrintDialog>
#include "parameters.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), exportFilename(QString())
{
    ui->setupUi(this);
    QObject::connect(this->ui->componentsTree, \
                     SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), \
                     this, \
                     SLOT(placeNewComponentFromList(QTreeWidgetItem*, int)));

    QObject::connect(this->ui->searchTextEdit, \
                     SIGNAL(textEdited(const QString&)), \
                     this, \
                     SLOT(searchTextFieldEdited(const QString&)));


    this->viewer = new CircuitDiagramEditor(this);
    this->centralWidget()->layout()->addWidget(this->viewer);

    if(!this->loadComponentsLibrary())
    {
        QMessageBox::critical(this, \
                              "Error", \
                              "Error while loading components library!");
    }


    QObject::connect(this->ui->menubarAction_New, SIGNAL(triggered()), this, SLOT(createNewDiagram()));
    QObject::connect(this->ui->menubarAction_Save, SIGNAL(triggered()), this, SLOT(saveDiagram()));
    QObject::connect(this->ui->menubarAction_SaveAs, SIGNAL(triggered()), this, SLOT(saveDiagramAs()));
    QObject::connect(this->ui->menubarAction_Open, SIGNAL(triggered()), this, SLOT(loadDiagram()));
    QObject::connect(this->ui->menubarAction_ExportAsImage, SIGNAL(triggered()), this, SLOT(exportDiagramAsImage()));
    QObject::connect(this->ui->menubarAction_Print, SIGNAL(triggered()), this, SLOT(printDiagram()));
    QObject::connect(this->ui->menubarAction_Help, SIGNAL(triggered()), this, SLOT(showHelp()));
    QObject::connect(this->ui->menubarAction_About, SIGNAL(triggered()), this, SLOT(showAbout()));
    QObject::connect(this->ui->menubarAction_AboutQt, SIGNAL(triggered()), this, SLOT(showAboutQt()));
    QObject::connect(this->ui->menubarAction_ZoomIn, SIGNAL(triggered()), this->viewer, SLOT(zoomIn()));
    QObject::connect(this->ui->menubarAction_ZoomOut, SIGNAL(triggered()), this->viewer, SLOT(zoomOut()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::placeNewComponentFromList(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);

    QString componentId = item->data(0, Qt::UserRole).toString();
    for(const auto& c: this->sampleComponentsList)
    {
        if(componentId == c->id())
        {
            QRect viewportRect(0, 0, this->viewer->viewport()->width(), this->viewer->viewport()->height());
            QRectF viewerVisibleRect = this->viewer->mapToScene(viewportRect).boundingRect();
            this->viewer->placeComponent(c->copy(), viewerVisibleRect.topLeft().toPoint());
            break;
        }
    }
}

bool MainWindow::loadComponentsLibrary()
{
    QFile* libraryFile = new QFile(componentsLibraryPath);
    if(!libraryFile->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        delete libraryFile;
        return false;
    }

    QXmlStreamReader *xmlReader = new QXmlStreamReader(libraryFile);
    QTreeWidgetItem *newSection = nullptr, *newComponent = nullptr;
    ElectronicComponent* newComponentSample = nullptr;

    while(!xmlReader->atEnd())
    {
        if(xmlReader->hasError())
        {
            delete xmlReader;
            delete libraryFile;
            return false;
        }

        QXmlStreamReader::TokenType token = xmlReader->readNext();

        // If token is StartElement - read it
        if(token == QXmlStreamReader::StartElement)
        {

            if(xmlReader->name() == "library")
            {
                continue;
            }
            else if(xmlReader->name() == "section")
            {
                newSection = new QTreeWidgetItem(QStringList {xmlReader->attributes().value("label").toString()});
                this->ui->componentsTree->addTopLevelItem(newSection);
            }
            else if(xmlReader->name() == "component")
            {
                if(newSection)
                {
                    newComponent = new QTreeWidgetItem(QStringList {xmlReader->attributes().value("label").toString()});
                    newComponent->setData(0, Qt::UserRole, QVariant(xmlReader->attributes().value("id").toString()));
                    newSection->addChild(newComponent);

                    if(newComponentSample)
                    {
                        this->sampleComponentsList.append(newComponentSample);
                    }

                    newComponentSample = new ElectronicComponent(xmlReader->attributes().value("id").toString(), \
                                                                 xmlReader->attributes().value("icon").toString(),
                                                                 QSize(xmlReader->attributes().value("width").toUInt(), \
                                                                       xmlReader->attributes().value("height").toUInt()));

                    newComponentSample->label()->setPlainText(xmlReader->attributes().value("acronym").toString());
                }
            }
            else if(xmlReader->name() == "pin")
            {
                if(newComponentSample)
                {
                    newComponentSample->addJunction(QPointF(xmlReader->attributes().value("x").toFloat(), \
                                                            xmlReader->attributes().value("y").toFloat()));
                }
            }
        }
    }

    if(newComponentSample) // add the last loaded component
    {
        this->sampleComponentsList.append(newComponentSample);
    }

    delete xmlReader;
    delete libraryFile;
    return true;
}

void MainWindow::searchTextFieldEdited(const QString &searchString)
{
    auto allItems = this->ui->componentsTree->findItems("", Qt::MatchContains | Qt::MatchRecursive);
    auto foundItems = this->ui->componentsTree->findItems(searchString, Qt::MatchContains | Qt::MatchRecursive);

    // Hide all items
    for(auto& i: allItems)
    {
        i->setHidden(true);
    }

    // Show only suitable items and their parents
    for(auto& i: foundItems)
    {
        i->setHidden(false);

        QTreeWidgetItem* parent = i->parent();
        while(parent)
        {
            parent->setHidden(false);
            parent = parent->parent();
        }
    }
}

void MainWindow::createNewDiagram()
{
    // TODO: check if current digram is empty and if not, suggest to save it
    this->viewer->clear();
    this->exportFilename = QString();
}

void MainWindow::saveDiagramAs()
{
    this->exportFilename = QFileDialog::getSaveFileName(this);
    if(this->exportFilename.isEmpty()) // cancelled
    {
        return;
    }

    this->saveDiagram();
}

void MainWindow::saveDiagram()
{
    if(this->exportFilename.isEmpty())
    {
        if((this->exportFilename = QFileDialog::getSaveFileName(this)).isEmpty())
        {
            return; // cancelled
        }
    }

    QFile* diagramFile = new QFile(this->exportFilename);
    QXmlStreamWriter *xmlWriter;
    if(!diagramFile->open(QIODevice::WriteOnly| QIODevice::Text))
    {
        goto failed;
    }

    xmlWriter = new QXmlStreamWriter(diagramFile);
    xmlWriter->setAutoFormatting(true);
    xmlWriter->writeStartDocument();
    xmlWriter->writeStartElement("diagram");

    for(const auto& item: this->viewer->items())
    {
        if(xmlWriter->hasError())
        {
            goto failed;
        }


        if(Wire* wire = dynamic_cast<Wire*>(item)) // if item is a wire
        {
            xmlWriter->writeStartElement("wire");
            xmlWriter->writeStartElement("vertexes");
            for(const auto& v: wire->vertexes())
            {
                xmlWriter->writeStartElement("vertex");

                xmlWriter->writeAttribute("x", QString::number(v.x()));
                xmlWriter->writeAttribute("y", QString::number(v.y()));

                xmlWriter->writeEndElement();
            }
            xmlWriter->writeEndElement();

            xmlWriter->writeStartElement("junctions");
            for(const auto& j: wire->junctions())
            {
                xmlWriter->writeStartElement("junction");

                xmlWriter->writeAttribute("x", QString::number(j.x()));
                xmlWriter->writeAttribute("y", QString::number(j.y()));

                xmlWriter->writeEndElement();
            }
            xmlWriter->writeEndElement();
            xmlWriter->writeEndElement();
        }
        else if(ElectronicComponent* component = \
                dynamic_cast<ElectronicComponent*>(item)) // if item is an electronic component
        {
            xmlWriter->writeStartElement("component");

            xmlWriter->writeAttribute("id", component->id());
            xmlWriter->writeAttribute("x", QString::number(component->x()));
            xmlWriter->writeAttribute("y", QString::number(component->y()));
            xmlWriter->writeAttribute("angle-x", QString::number(component->rotation(Qt::XAxis)));
            xmlWriter->writeAttribute("angle-y", QString::number(component->rotation(Qt::YAxis)));
            xmlWriter->writeAttribute("angle-z", QString::number(component->rotation(Qt::ZAxis)));


            xmlWriter->writeStartElement("label");

            xmlWriter->writeAttribute("x", QString::number(component->label()->x()));
            xmlWriter->writeAttribute("y", QString::number(component->label()->y()));
            xmlWriter->writeCharacters(component->label()->toPlainText());

            xmlWriter->writeEndElement();
            xmlWriter->writeEndElement();
        }
    }

    xmlWriter->writeEndElement();
    xmlWriter->writeEndDocument();

    delete xmlWriter;
    delete diagramFile;
    return; // success

    failed:
    delete xmlWriter;
    delete diagramFile;
    QMessageBox::critical(this, \
                          "Error", \
                          "Error during file export!");
}

void MainWindow::loadDiagram()
{
    // TODO: check if current digram is empty and if not, suggest to save it
    QString filename = QFileDialog::getOpenFileName(this);
    if(filename.isEmpty()) // cancelled
    {
        return;
    }

    QFile* diagramFile = new QFile(filename);
    QXmlStreamReader *xmlReader;
    if(!diagramFile->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        goto failed;
    }

    xmlReader = new QXmlStreamReader(diagramFile);

    while(!xmlReader->atEnd())
    {
        if(xmlReader->hasError())
        {
            goto failed;
        }

        QXmlStreamReader::TokenType token = xmlReader->readNext();
        if(token == QXmlStreamReader::StartElement)
        {
            if(xmlReader->name() == "component")
            {
                ElectronicComponent* component;
                QString componentId = xmlReader->attributes().value("id").toString();
                for(const auto& c: this->sampleComponentsList)
                {
                    if(componentId == c->id())
                    {
                        component = c->copy();
                        component->rotate(xmlReader->attributes().value("angle-x").toInt(), Qt::XAxis);
                        component->rotate(xmlReader->attributes().value("angle-y").toInt(), Qt::YAxis);
                        component->rotate(xmlReader->attributes().value("angle-z").toInt(), Qt::ZAxis);
                        this->viewer->placeComponent(component, QPoint(xmlReader->attributes().value("x").toInt(), \
                                                                 xmlReader->attributes().value("y").toInt()));

                        if(xmlReader->readNext() == QXmlStreamReader::StartElement && \
                                xmlReader->name() == "label")
                        {
                            component->label()->setPos(xmlReader->attributes().value("x").toInt(), \
                                                       xmlReader->attributes().value("y").toInt());
                            component->label()->setPlainText(xmlReader->readElementText());
                        }

                        break;
                    }
                }
            }
            else if(xmlReader->name() == "wire")
            {
                Wire* wire = new Wire;
                this->viewer->placeWire(wire);

                for(QXmlStreamReader::TokenType inToken = QXmlStreamReader::NoToken; \
                    !(inToken == QXmlStreamReader::EndElement && xmlReader->name() == "wire"); \
                    inToken = xmlReader->readNext())
                {
                    if(inToken == QXmlStreamReader::StartElement)
                    {
                        if(xmlReader->name() == "vertex")
                        {
                            wire->addVertex(QPointF(xmlReader->attributes().value("x").toInt(), \
                                                    xmlReader->attributes().value("y").toInt()));
                        }
                        else if(xmlReader->name() == "junction")
                        {
                            wire->addJunction(QPointF(xmlReader->attributes().value("x").toInt(), \
                                                      xmlReader->attributes().value("y").toInt()));
                        }
                    }
                }
            }
        }
    }

    delete xmlReader;
    delete diagramFile;
    return; // success

    failed:
    delete xmlReader;
    delete diagramFile;
    QMessageBox::critical(this, \
                          "Error", \
                          "Error during file import!");

}

void MainWindow::showAboutQt()
{
    QMessageBox::aboutQt(this);
}

void MainWindow::showAbout()
{
    QMessageBox::about(this, "About program", \
                       "Curcuit diagram designer\n" \
                       "Author: Silantyev Daniel, ISPU\n" \
                       "Year: 2022");
}

void MainWindow::exportDiagramAsImage()
{
    QString filename;
    if(!(filename = QFileDialog::getSaveFileName(this, "", "", \
                                    "Image formats (*.png *.jpg)")).isEmpty())
    {
        this->viewer->toImage().save(filename);
    }
}

void MainWindow::printDiagram()
{
    QPrinter printer;
    QPrintDialog *printDialog = new QPrintDialog(&printer, 0);

    if(printDialog->exec() == QDialog::Accepted)
    {
        QPainter painter(&printer);
        painter.drawImage(QPoint(0, 0), this->viewer->toImage());
        painter.end();
    }

    delete printDialog;
}
