/*
  pair_view_main_window.cpp - Implements simple things

  Copyright (C) 2015  Chittaranjan Srinivas Swaminathan

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>
*/

#include "pair_view_main_window.h"
#include "ui_pair_view_main_window.h"

PairViewMainWindow::PairViewMainWindow(QWidget *parent, int argn, char *args[], char* env[]) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    srnp_core = new SrnpCore(this, argn, args, env);

    connect(ui->pushButtonQuit, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->postPairButton, SIGNAL(clicked()), this, SLOT(postPair()) );

}

void PairViewMainWindow::postPair()
{
    //ui->listWidget->addItem(ui->textEdit->toPlainText());
    //treeWidget_AddRoot(ui->textEdit->toPlainText(), "You Clicked Start");

    srnp::setPair(ui->postPairKey->toPlainText().toStdString(),
        ui->postPairValue->toPlainText().toStdString());

}

void PairViewMainWindow::treeWidget_AddRoot(const QString& Key, const QString& Value)
{
    QTreeWidgetItem* rootItem = new QTreeWidgetItem(ui->treeWidget);
    rootItem->setText(0, Key);
    rootItem->setText(1, Value);

    connect(ui->treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(displayPair(QTreeWidgetItem*, int)));
}

void PairViewMainWindow::displayPair(QTreeWidgetItem* item, int column)
{
    srnp::Pair pairToShow = SrnpCore::all_pairs[item->text(0)];
    ui->keyDisplay->setText(QString::fromStdString(pairToShow.getKey()));
    ui->valueDisplay->setText(QString::fromStdString(pairToShow.getValue()));
    ui->expiryTimeDisplay->setText(QString::fromStdString(boost::posix_time::to_simple_string(pairToShow.getExpiryTime())));
    ui->writeTimeDisplay->setText(QString::fromStdString(boost::posix_time::to_simple_string(pairToShow.getWriteTime())));
}

void PairViewMainWindow::treeWidget_AddChild(QTreeWidgetItem* parent, const QString& Key, const QString& Value)
{
    QTreeWidgetItem *childItem = new QTreeWidgetItem();
    childItem->setText(0, Key);
    childItem->setText(1, Value);
    parent->addChild(childItem);
}

QTreeWidgetItem* PairViewMainWindow::treeWidget_GetItemWithKey (const QString& itemKey)
{
    QList <QTreeWidgetItem*> our_list = ui->treeWidget->findItems(itemKey, Qt::MatchFixedString, 0);

    if(our_list.size() == 0)
        return NULL;
    else if(our_list.size() > 1)
    {
        printf("Something is wrong. Got multiple similar entries.");
    }

    return our_list[0];
}

void PairViewMainWindow::treeWidget_UpdateItem(QTreeWidgetItem* itemToUpdate, const QString& Value)
{
    itemToUpdate->setText(1, Value);
}

PairViewMainWindow::~PairViewMainWindow()
{
    delete ui;
    delete srnp_core;
}

/**
 * @brief SrnpCore::all_pairs holds a mapped copy of all pairs.
 * TODO: Must fix. Redundant data. The pairspace can already give this.
 */
QMap <QString, srnp::Pair> SrnpCore::all_pairs;

/**
 * @brief Constructor.
 * @param window is a pointer to the main window.
 * @param argn: number of arguments - unchanged from command-line.
 * @param args: the command-line arguments.
 * @param env: the environment variables.
 */
SrnpCore::SrnpCore (PairViewMainWindow *window, int argn, char* args[], char* env[])
{
    this->pvmw = window;
    srnp::srnp_print_setup("DEBUG");
    srnp::initialize(argn, args, env);

    srnp::registerSubscription("*");
    srnp::registerCallback(-1, "*", boost::bind(&SrnpCore::callback, this, _1));
}

SrnpCore::~SrnpCore()
{
    srnp::shutdown();
}


void SrnpCore::callback (const srnp::Pair::ConstPtr& pair)
{
    pvmw->signalPairReceived();

    //map_mutex.lock();
    all_pairs[QString::fromStdString(pair->getKey())] = *pair;
    //map_mutex.unlock();
    QTreeWidgetItem* item2Update = pvmw->treeWidget_GetItemWithKey(QString::fromStdString(pair->getKey()));
    if(item2Update != NULL)
        pvmw->treeWidget_UpdateItem(item2Update, QString::fromStdString(pair->getValue())) ;
    else
        pvmw->treeWidget_AddRoot(QString::fromStdString(pair->getKey()), QString::fromStdString(pair->getValue()));
}
