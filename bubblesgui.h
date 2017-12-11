/***************************************************************************
 *   Copyright (C) 2003 by Tim Sutton                                      *
 *   tim@linfiniti.com                                                     *
 *                                                                         *
 *   This is a plugin generated from the QGIS plugin template              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef bubblesGUI_H
#define bubblesGUI_H

#include <QDialog> 
#include <ui_bubblesguibase.h>

/**
@author Tim Sutton
*/
class bubblesGui : public QDialog, private Ui::bubblesGuiBase
{
    Q_OBJECT
  public:
    bubblesGui( QWidget* parent = 0, Qt::WindowFlags fl = 0 );
    ~bubblesGui();

  private:
    static const int context_id = 0;

  private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
    void on_buttonBox_helpRequested();

};

#endif
