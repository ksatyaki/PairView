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
    connect(ui->postPairButton, SIGNAL(clicked()), this, SLOT(postPair()));
    connect(ui->treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(displayPair(QTreeWidgetItem*, int)));

}

void PairViewMainWindow::postPair()
{
    //ui->listWidget->addItem(ui->textEdit->toPlainText());
    //treeWidget_AddRoot(ui->textEdit->toPlainText(), "You Clicked Start");

    if(ui->postPairOwner->toPlainText().toInt() == 0) {
        srnp::setPair(ui->postPairKey->toPlainText().toStdString(),
            ui->postPairValue->toPlainText().toStdString());
    }
    else {
        //SRNP_PRINT_DEBUG << "Owner is " << ui->postPairOwner->toPlainText().toInt();
        //SRNP_PRINT_DEBUG << "Owner is " << this->srnp_core->owner();
        if(ui->postPairOwner->toPlainText().toInt() == this->srnp_core->owner()) {
            srnp::setPair(ui->postPairKey->toPlainText().toStdString(),
                ui->postPairValue->toPlainText().toStdString());
        }
        else {
                SRNP_PRINT_DEBUG << "Remote pair : "<< srnp::setRemotePair(ui->postPairOwner->toPlainText().toInt(),
                                ui->postPairKey->toPlainText().toStdString(),
                                ui->postPairValue->toPlainText().toStdString());
        }
    }

}

QTreeWidgetItem* PairViewMainWindow::treeWidget_AddRoot(const QString& Key, const QString& Value, QTreeWidgetItem* ownerGroup)
{
    QTreeWidgetItem* rootItem = new QTreeWidgetItem(ownerGroup);
    rootItem->setText(1, Key);
    rootItem->setText(2, Value);
    ownerGroup->addChild(rootItem);
    connect(this, SIGNAL(pairReceived(QTreeWidgetItem*)), this, SLOT(showPairIfClicked(QTreeWidgetItem*)));
    return rootItem;
}

QTreeWidgetItem* PairViewMainWindow::treeWidget_AddComponentRoot(const QString& Owner, const QString& componentName)
{
    QTreeWidgetItem* rootItem = new QTreeWidgetItem(ui->treeWidget);
    rootItem->setText(0, Owner);
    rootItem->setText(1, componentName);
    return rootItem;
}

QTreeWidgetItem* PairViewMainWindow::treeWidget_GetItemSection(const int& owner) {

    //printf("Matching for %s\n", QString::number(owner).toStdString().c_str());
    QList <QTreeWidgetItem*> our_list = ui->treeWidget->findItems(QString::number(owner), Qt::MatchFixedString, 0);

    if(our_list.size() == 0)
        return NULL;
    else if(our_list.size() > 1)
    {
        //printf("Something is wrong. Got multiple similar entries.");
    }

    return our_list[0];
}

void PairViewMainWindow::displayPair(QTreeWidgetItem* item, int column)
{
    if(item->text(0).toInt() != 0)
        return;
    srnp::Pair pairToShow = SrnpCore::all_pairs[QPair <int, QString> (item->parent()->text(0).toInt(), item->text(1))];
    ui->keyDisplay->setText(QString::fromStdString(pairToShow.getKey()));
    ui->valueDisplay->setText(QString::fromStdString(pairToShow.getValue()));
    ui->expiryTimeDisplay->setText(QString::fromStdString(boost::posix_time::to_simple_string(pairToShow.getExpiryTime())));
    ui->writeTimeDisplay->setText(QString::fromStdString(boost::posix_time::to_simple_string(pairToShow.getWriteTime())));
    ui->ownerDisplay->setText(QString::number(pairToShow.getOwner()));

    ui->postPairOwner->setText(QString::number(pairToShow.getOwner()));
    ui->postPairKey->setText(QString::fromStdString(pairToShow.getKey()));
}

QTreeWidgetItem* PairViewMainWindow::treeWidget_AddChild(QTreeWidgetItem* parent, const QString& Key, const QString& Value)
{
    QTreeWidgetItem *childItem = new QTreeWidgetItem();
    childItem->setText(1, Key);
    childItem->setText(2, Value);
    parent->addChild(childItem);

    return childItem;
}

QTreeWidgetItem* PairViewMainWindow::treeWidget_GetItemWithKey (const QString& itemKey, QTreeWidgetItem* ownerGroup)
{
    for(int i = 0; i < ownerGroup->childCount(); i++) {
        //printf("%s and %s\n", ownerGroup->child(i)->text(1).toStdString().c_str(), itemKey.toStdString().c_str());
        if(ownerGroup->child(i)->text(1).compare(itemKey) == 0) {
            return ownerGroup->child(i);
        }
    }
    return NULL;
}

void PairViewMainWindow::treeWidget_UpdateItem(QTreeWidgetItem* itemToUpdate, const QString& Value)
{
    itemToUpdate->setText(2, Value);
}

void PairViewMainWindow::showPairIfClicked(QTreeWidgetItem *item2Show) {
    if(item2Show->isSelected()) {
        displayPair(item2Show, 0);
    }
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
QMap <QPair<int, QString>, srnp::Pair> SrnpCore::all_pairs;

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

    owner_ = srnp::getOwnerID();

    srnp::registerSubscription("*");
    srnp::registerCallback(-1, "*", boost::bind(&SrnpCore::callback, this, _1));
}

SrnpCore::~SrnpCore()
{
    srnp::shutdown();
}


void SrnpCore::callback (const srnp::Pair::ConstPtr& pair)
{

    all_pairs[QPair <int, QString> (pair->getOwner(), QString::fromStdString(pair->getKey()))] = *pair;

    QTreeWidgetItem* itemGroup = pvmw->treeWidget_GetItemSection(pair->getOwner());
    if(itemGroup == NULL) {
        //printf("Item group was null.\n");
        itemGroup = pvmw->treeWidget_AddComponentRoot(QString::number(pair->getOwner()), "");
    }
    else {
        //printf("Item group was NOT null.\n");
    }

    QTreeWidgetItem* item2Update = pvmw->treeWidget_GetItemWithKey(QString::fromStdString(pair->getKey()), itemGroup);

    if(item2Update != NULL) {
        //printf("Item to update is NULLE\n");
        pvmw->treeWidget_UpdateItem(item2Update, QString::fromStdString(pair->getValue())) ;
    }
    else {
        item2Update = pvmw->treeWidget_AddRoot(QString::fromStdString(pair->getKey()), QString::fromStdString(pair->getValue()), itemGroup);
    }
    // Finally emit this. In case the item is clicked on, we can display it.
    emit pvmw->pairReceived(item2Update);
}
