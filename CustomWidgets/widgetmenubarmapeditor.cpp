/*
    RPG Paper Maker Copyright (C) 2017 Marie Laporte

    This file is part of RPG Paper Maker.

    RPG Paper Maker is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    RPG Paper Maker is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QPainter>
#include "widgetmenubarmapeditor.h"
#include "ui_widgetmenubarmapeditor.h"

QColor WidgetMenuBarMapEditor::colorBackgroundSelected(95, 158, 160);
QColor WidgetMenuBarMapEditor::colorBackgroundRightSelected(120, 163, 131);

// -------------------------------------------------------
//
//  CONSTRUCTOR / DESTRUCTOR / GET / SET
//
// -------------------------------------------------------

WidgetMenuBarMapEditor::WidgetMenuBarMapEditor(QWidget *parent,
                                               bool selection) :
    QMenuBar(parent),
    ui(new Ui::WidgetMenuBarMapEditor),
    m_selectionKind(MapEditorSelectionKind::Land),
    m_selection(selection),
    m_actionPencil(nullptr),
    m_actionRectangle(nullptr),
    m_actionPin(nullptr),
    m_actionLayerNone(nullptr),
    m_actionLayerOn(nullptr)
{
    ui->setupUi(this);

    if (m_selection){
        for (int i = 0; i < this->actions().size(); i++)
            actions().at(i)->setProperty("selection", false);
        actions().at((int) m_selectionKind)->setProperty("selection",true);
    }
}

WidgetMenuBarMapEditor::~WidgetMenuBarMapEditor()
{
    if (this->cornerWidget() != nullptr)
        delete this->cornerWidget();

    if (m_actionPencil != nullptr) {
        delete m_actionPencil;
        delete m_actionRectangle;
        delete m_actionPin;
        delete m_actionLayerNone;
        delete m_actionLayerOn;
    }

    delete ui;
}

MapEditorSelectionKind WidgetMenuBarMapEditor::selectionKind() const {
    return m_selectionKind;
}

MapEditorSubSelectionKind WidgetMenuBarMapEditor::subSelectionKind() const {
    QString text = this->actions().at((int) m_selectionKind)->text();

    if (text == ui->actionFloors->text())
        return MapEditorSubSelectionKind::Floors;
    else if (text == ui->actionFace_Sprite->text())
        return MapEditorSubSelectionKind::SpritesFace;
    else if (text == ui->actionFix_Sprite->text())
        return MapEditorSubSelectionKind::SpritesFix;
    else if (text == ui->actionDouble_Sprite->text())
        return MapEditorSubSelectionKind::SpritesDouble;
    else if (text == ui->actionQuadra_Sprite->text())
        return MapEditorSubSelectionKind::SpritesQuadra;
    else if (text == ui->actionWall_Sprite->text())
        return MapEditorSubSelectionKind::SpritesWall;
    else if (text == ui->actionEvents->text())
        return MapEditorSubSelectionKind::Object;

    return MapEditorSubSelectionKind::None;
}

DrawKind WidgetMenuBarMapEditor::drawKind() const{
    WidgetMenuBarMapEditor* bar =
            (WidgetMenuBarMapEditor*) this->cornerWidget();
    int index = (int) MapEditorModesKind::DrawPencil;

    if (bar->actions().at(index++)->property("selection") == true)
        return DrawKind::Pencil;
    else if (bar->actions().at(index++)->property("selection") == true)
        return DrawKind::Rectangle;
    else if (bar->actions().at(index++)->property("selection") == true)
        return DrawKind::Pin;

    return DrawKind::Pencil;
}

bool WidgetMenuBarMapEditor::layerOn() const {
    WidgetMenuBarMapEditor* bar =
            (WidgetMenuBarMapEditor*) this->cornerWidget();
    int index = (int) MapEditorModesKind::LayerNone;

    if (bar->actions().at(index++)->property("selection") == true)
        return false;
    else if (bar->actions().at(index++)->property("selection") == true)
        return true;

    return false;
}

// -------------------------------------------------------
//
//  INTERMEDIARY FUNCTIONS
//
// -------------------------------------------------------

bool WidgetMenuBarMapEditor::containsMenu() const{
    QAction* action = this->activeAction();
    if (action != nullptr && action->menu() != nullptr) {
        return action->menu()->rect().contains(
                    action->menu()->mapFromGlobal(QCursor::pos()));
    }

    return false;
}

// -------------------------------------------------------

void WidgetMenuBarMapEditor::initializeRightMenu(){
    WidgetMenuBarMapEditor *bar = new WidgetMenuBarMapEditor(this, false);
    bar->clear();

    // Draw mode
    m_actionPencil = new QAction(QIcon(":/icons/Ressources/pencil.png"),
                                 "Pencil");
    m_actionPencil->setProperty("selection", true);
    m_actionRectangle = new QAction(QIcon(":/icons/Ressources/rectangle.png"),
                         "Rectangle");
    m_actionRectangle->setEnabled(false);
    m_actionRectangle->setProperty("selection", false);
    m_actionPin = new QAction(QIcon(":/icons/Ressources/pin.png"),
                              "Pin of paint");
    m_actionPin->setProperty("selection", false);
    bar->addAction(m_actionPencil);
    bar->addAction(m_actionRectangle);
    bar->addAction(m_actionPin);
    bar->addAction("|");

    // Layer
    m_actionLayerNone = new QAction(QIcon(":/icons/Ressources/layer_none.png"),
                                    "Normal");
    m_actionLayerNone->setProperty("selection", true);
    m_actionLayerOn = new QAction(QIcon(":/icons/Ressources/layer_on.png"),
                         "On top");
    m_actionLayerOn->setProperty("selection", false);
    bar->addAction(m_actionLayerNone);
    bar->addAction(m_actionLayerOn);

    this->setCornerWidget(bar);
}

// -------------------------------------------------------

void WidgetMenuBarMapEditor::updateSelection(QAction* action){

    // Deselect previous selected action
    actions().at((int) m_selectionKind)->setProperty("selection", false);

    // Select the pressed action
    m_selectionKind =
         static_cast<MapEditorSelectionKind>(actions().indexOf(action));

    action->setProperty("selection", true);

    // Repaint
    this->repaint();
}

// -------------------------------------------------------

void WidgetMenuBarMapEditor::updateMenutext(QMenu* menu, QAction *action){
    menu->setTitle(action->text());
}

// -------------------------------------------------------

void WidgetMenuBarMapEditor::updateSubSelection(QMenu *menu,
                                                QAction* menuAction,
                                                QAction* action)
{
    updateMenutext(menu, action);
    updateSelection(menuAction);
}

// -------------------------------------------------------

void WidgetMenuBarMapEditor::updateRight(QAction* action)
{
    int index = this->actions().indexOf(action);
    QList<int> list;
    list << (int) MapEditorModesKind::DrawPencil
         << (int) MapEditorModesKind::DrawPin
         << (int) MapEditorModesKind::LayerNone
         << (int) MapEditorModesKind::LayerOn;

    // Deselect previous selected action
    for (int i = 0; i < list.size() / 2; i++) {
        int l = list.at(i * 2);
        int r = list.at(i * 2 + 1);
        if (index >= l && index <= r) {
            for (int i = l; i <= r; i++)
                actions().at(i)->setProperty("selection", false);
        }
    }

    // Select the pressed action
    action->setProperty("selection", true);

    // Repaint
    this->repaint();
}

// -------------------------------------------------------
//
//  EVENTS
//
// -------------------------------------------------------

void WidgetMenuBarMapEditor::mouseMoveEvent(QMouseEvent* event){
    this->setActiveAction(this->actionAt(event->pos()));
}

// -------------------------------------------------------

void WidgetMenuBarMapEditor::mousePressEvent(QMouseEvent* event){
    QAction* action = this->actionAt(event->pos());
    if (m_selection) {
        if (action != nullptr) {
            updateSelection(action);
            emit selectionChanged();
        }
    }
    else {
        if (action != nullptr && action->text() != "|" && action->isEnabled())
            updateRight(action);
    }

    QMenuBar::mousePressEvent(event);
}

// -------------------------------------------------------

void WidgetMenuBarMapEditor::paintEvent(QPaintEvent *e){
    QPainter p(this);

    // Draw the items
    for (int i = 0; i < actions().size(); ++i) {
        QAction *action = actions().at(i);
        QRect adjustedActionRect = this->actionGeometry(action);

        // Fill by the magic color the selected item
        if (action->property("selection") == true) {
            if (m_selection)
                p.fillRect(adjustedActionRect, colorBackgroundSelected);
            else
                p.fillRect(adjustedActionRect, colorBackgroundRightSelected);
        }

        // Draw all the other stuff (text, special background..)
        if (adjustedActionRect.isEmpty() || !action->isVisible())
            continue;
        if(!e->rect().intersects(adjustedActionRect))
            continue;

        QStyleOptionMenuItem opt;
        initStyleOption(&opt, action);
        opt.rect = adjustedActionRect;

        // Drawing separator with darker color
        if (action->text() == "|") {
            p.setPen(QColor(100, 100, 100));
            p.drawText(adjustedActionRect, "|", QTextOption(Qt::AlignCenter));
            opt.text = "";
        }
        style()->drawControl(QStyle::CE_MenuBarItem, &opt, &p, this);
    }
}

// -------------------------------------------------------
//
//  SLOTS
//
// -------------------------------------------------------

void WidgetMenuBarMapEditor::on_menuFloors_triggered(QAction* action){
    updateSubSelection(ui->menuFloors,
                       this->actions().at((int)MapEditorSelectionKind::Land),
                       action);
}

// -------------------------------------------------------

void WidgetMenuBarMapEditor::on_menuFace_Sprite_triggered(QAction* action){
    updateSubSelection(ui->menuFace_Sprite,
                       this->actions().at((int)MapEditorSelectionKind::Sprites),
                       action);
}
