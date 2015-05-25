/*
  pair_view_main_window.h

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

#ifndef PAIR_VIEW_MAIN_WINDOW_H
#define PAIR_VIEW_MAIN_WINDOW_H

#include <stdio.h>
#include <QMap>
#include <QMainWindow>
#include <QTreeWidget>
#include <srnp/srnp_kernel.h>
#include <boost/thread/mutex.hpp>

namespace Ui {
class MainWindow;
}

class SrnpCore;

class PairViewMainWindow : public QMainWindow
{
    Q_OBJECT

protected slots:
    void postPair();
    void displayPair(QTreeWidgetItem* item, int column);

signals:
    void pairReceived();

public:
    inline void signalPairReceived() { emit pairReceived(); }

    void treeWidget_AddRoot(const QString& Key, const QString& Value);
    void treeWidget_AddChild(QTreeWidgetItem* parent, const QString& Key, const QString& Value);
    void treeWidget_UpdateItem(QTreeWidgetItem* itemToUpdate, const QString& value);
    QTreeWidgetItem* treeWidget_GetItemWithKey (const QString& itemKey);

    explicit PairViewMainWindow(QWidget *parent, int argn, char *args[], char* env[]);
    ~PairViewMainWindow();

private:
    SrnpCore* srnp_core;
    Ui::MainWindow *ui;
};

/**
 * @brief This class takes care of all srnp things.
 */
class SrnpCore
{
protected:
    void callback (const srnp::Pair::ConstPtr& pair);
    boost::mutex map_mutex;
public:
    virtual ~SrnpCore();
    SrnpCore(PairViewMainWindow *window, int argn, char* args[], char* env[]);
    static QMap <QString, srnp::Pair> all_pairs;
private:
    PairViewMainWindow* pvmw;
};

#endif // PAIR_VIEW_MAIN_WINDOW_H
