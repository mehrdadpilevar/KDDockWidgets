/*
  This file is part of KDDockWidgets.

  SPDX-FileCopyrightText: 2019-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Sérgio Martins <sergio.martins@kdab.com>

  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

/**
 * @file
 * @brief Implements a QTabWidget derived class with support for docking and undocking
 * KDockWidget::DockWidget as tabs .
 *
 * @author Sérgio Martins \<sergio.martins@kdab.com\>
 */

#include "TabBar.h"
#include "kddockwidgets/core/TabBar.h"
#include "kddockwidgets/core/Stack.h"

#include <QMetaObject>

using namespace KDDockWidgets;
using namespace KDDockWidgets::flutter;

TabBar::TabBar(Core::TabBar *controller, Core::View *parent)
    : View(controller, Core::ViewType::TabBar, parent)
    , TabBarViewInterface(controller)
    , m_controller(controller)
{
}

void TabBar::init()
{
}

int TabBar::tabAt(QPoint) const
{
    qDebug() << Q_FUNC_INFO << "Not implemented";
    return -1;
}

QString TabBar::text(int) const
{
    qDebug() << Q_FUNC_INFO << "Not implemented";
    return {};
}

QRect TabBar::rectForTab(int) const
{
    qDebug() << Q_FUNC_INFO << "Not implemented";
    return {};
}

void TabBar::moveTabTo(int from, int to)
{
    Q_UNUSED(from);
    Q_UNUSED(to);
    qDebug() << Q_FUNC_INFO << "Not implemented";
}

void TabBar::changeTabIcon(int index, const Icon &)
{
    qWarning() << Q_FUNC_INFO << "Not implemented" << index;
}

void TabBar::removeDockWidget(Core::DockWidget *)
{
    onRebuildRequested();

    // Rebuild the group as well, as it might want to remove the tabbar completely in case there's
    // no tabs or a single tab
    static_cast<flutter::View *>(m_controller->group()->view())->onRebuildRequested();
}

void TabBar::insertDockWidget(int, Core::DockWidget *dw, const Icon &,
                              const QString &)
{
    dw->view()->setParent(this);

    onRebuildRequested();

    // Rebuild the group as well, it might need to show the tabbar if it was hidden before
    static_cast<flutter::View *>(m_controller->group()->view())->onRebuildRequested();
}

void TabBar::renameTab(int index, const QString &)
{
    qWarning() << Q_FUNC_INFO << "Not implemented" << index;
}

void TabBar::setCurrentIndex(int)
{
    onRebuildRequested();
}

void TabBar::onMousePress(MouseEvent *me)
{
    m_controller->onMousePress(me->pos());
}
