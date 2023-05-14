/*
  This file is part of KDDockWidgets.

  SPDX-FileCopyrightText: 2019-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Sérgio Martins <sergio.martins@kdab.com>

  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

// We don't care about performance related checks in the tests
// clazy:excludeall=ctor-missing-parent-argument,missing-qobject-macro,range-loop,missing-typeinfo,detaching-member,function-args-by-ref,non-pod-global-static,reserve-candidates,qstring-allocations

// A test that was extracted out from tst_docks.cpp as it was too slow
// By using a separate executable it can be paralellized by ctest.

#include "utils.h"
#include "simple_test_framework.h"
#include "Config.h"
#include "core/Position_p.h"
#include "core/WindowBeingDragged_p.h"
#include "core/LayoutSaver_p.h"
#include "core/layouting/Item_p.h"
#include "kddockwidgets/core/ViewFactory.h"
#include "kddockwidgets/core/Action.h"
#include "kddockwidgets/core/MDILayout.h"
#include "kddockwidgets/core/DropArea.h"
#include "kddockwidgets/core/MainWindow.h"
#include "kddockwidgets/core/DockWidget.h"
#include "kddockwidgets/core/DockWidget_p.h"
#include "kddockwidgets/core/Separator.h"
#include "kddockwidgets/core/TabBar.h"
#include "kddockwidgets/core/Stack.h"
#include "kddockwidgets/core/SideBar.h"
#include "kddockwidgets/core/Platform.h"

using namespace KDDockWidgets;
using namespace KDDockWidgets::Core;
using namespace KDDockWidgets::Tests;


KDDW_QCORO_TASK tst_invalidLayoutAfterRestore()
{
    EnsureTopLevelsDeleted e;
    auto m = createMainWindow();
    auto dock1 = createDockWidget("dock1", Platform::instance()->tests_createView({ true }));
    auto dock2 = createDockWidget("dock2", Platform::instance()->tests_createView({ true }));
    auto dock3 = createDockWidget("dock3", Platform::instance()->tests_createView({ true }));
    auto dropArea = m->dropArea();
    Core::DropArea *layout = dropArea;
    // Stack 1, 2, 3
    m->addDockWidget(dock1, Location_OnLeft);
    m->addDockWidget(dock2, Location_OnRight);
    m->addDockWidget(dock3, Location_OnRight);

    const int oldContentsWidth = layout->layoutWidth();

    auto f1 = dock1->dptr()->group();
    dock3->close();
    dock2->close();
    dock1->close();
    CHECK(Platform::instance()->tests_waitForDeleted(f1));

    dock3->open();
    dock2->open();
    dock1->open();
    Platform::instance()->tests_waitForEvent(m.get(), Event::LayoutRequest); // So MainWindow min
                                                                             // size is updated

    Item *item1 = layout->itemForFrame(dock1->dptr()->group());
    Item *item3 = layout->itemForFrame(dock3->dptr()->group());
    Item *item4 = dropArea->centralFrame();

    CHECK_EQ(layout->count(), 4);
    CHECK_EQ(layout->placeholderCount(), 0);

    // Detach dock2
    QPointer<Core::Group> f2 = dock2->dptr()->group();
    f2->detachTab(dock2);
    CHECK(!f2.data());
    QTest::qWait(200); // Not sure why. Some event we're waiting for. TODO: Investigate
    auto fw2 = dock2->floatingWindow();
    CHECK_EQ(layout->view()->minSize().width(),
             2 * Item::separatorThickness + item1->minSize().width() + item3->minSize().width()
                 + item4->minSize().width());

    // Drop left of dock3
    layout->addWidget(fw2->dropArea()->view(), Location_OnLeft, dock3->dptr()->group());

    CHECK(Platform::instance()->tests_waitForDeleted(fw2));
    CHECK_EQ(layout->layoutWidth(), oldContentsWidth);
    layout->checkSanity();

    KDDW_CO_RETURN(true);
}

KDDW_QCORO_TASK tst_setFloatingWhenSideBySide()
{
    // Tests DockWidget::setFloating(false|true) when side-by-side (it should put it where it was)
    EnsureTopLevelsDeleted e;

    {
        // 1. Create a MainWindow with two docked dock-widgets, then float the first one.
        auto m = createMainWindow();
        auto dock1 = createDockWidget("dock1", Platform::instance()->tests_createView({ true }));
        auto dock2 = createDockWidget("dock2", Platform::instance()->tests_createView({ true }));
        m->addDockWidget(dock1, KDDockWidgets::Location_OnLeft);
        m->addDockWidget(dock2, KDDockWidgets::Location_OnRight);

        QPointer<Core::Group> group1 = dock1->dptr()->group();
        dock1->setFloating(true);
        CHECK(dock1->isFloating());
        auto fw = dock1->floatingWindow();
        CHECK(fw);

        // 2. Put it back, via setFloating(). It should return to its place.
        dock1->setFloating(false);

        CHECK(!dock1->isFloating());
        CHECK(!dock1->isTabbed());
    }

    {
        // 2. Tests a case where restoring a dock widget wouldn't make it use all its available
        // space
        auto m = createMainWindow(QSize(800, 500), MainWindowOption_None);
        auto dock1 = createDockWidget("dock1", Platform::instance()->tests_createView({ true }));
        auto dock2 = createDockWidget("dock2", Platform::instance()->tests_createView({ true }));
        auto dock3 = createDockWidget("dock3", Platform::instance()->tests_createView({ true }));
        auto dropArea = m->dropArea();
        Core::DropArea *layout = dropArea;
        m->addDockWidget(dock1, KDDockWidgets::Location_OnLeft);
        m->addDockWidget(dock2, KDDockWidgets::Location_OnRight);
        m->addDockWidget(dock3, KDDockWidgets::Location_OnRight);
        auto f2 = dock2->dptr()->group();
        Item *item2 = layout->itemForFrame(f2);
        CHECK(item2);
        dock2->close();
        dock3->close();
        Platform::instance()->tests_waitForDeleted(f2);
        dock2->open();
        Platform::instance()->tests_waitForResize(dock2->view());

        CHECK_EQ(item2->geometry(), dock2->dptr()->group()->view()->geometry());
        layout->checkSanity();
    }

    KDDW_CO_RETURN(true);
}

KDDW_QCORO_TASK tst_dockWindowWithTwoSideBySideFramesIntoCenter()
{
    EnsureTopLevelsDeleted e;
    KDDockWidgets::Config::self().setInternalFlags(KDDockWidgets::Config::InternalFlag_NoAeroSnap);
    KDDockWidgets::Config::self().setFlags({});

    auto m = createMainWindow();
    auto fw = createFloatingWindow();
    auto dock2 = createDockWidget("doc2");
    nestDockWidget(dock2, fw->dropArea(), nullptr, KDDockWidgets::Location_OnLeft);
    CHECK_EQ(fw->groups().size(), 2);
    CHECK(fw->dropArea()->checkSanity());

    auto fw2 = createFloatingWindow();
    fw2->view()->move(fw->x() + fw->width() + 100, fw->y());

    // QtQuick is a bit more async than QWidgets. Wait for the move.
    Platform::instance()->tests_waitForEvent(fw2->view()->window(), Event::Move);

    auto da2 = fw2->dropArea();
    const QPoint dragDestPos = da2->mapToGlobal(da2->rect().center());

    dragFloatingWindowTo(fw, dragDestPos);
    CHECK(fw2->dropArea()->checkSanity());

    CHECK_EQ(fw2->groups().size(), 1);
    auto f2 = fw2->groups().constFirst();
    CHECK_EQ(f2->dockWidgetCount(), 3);

    KDDW_CO_RETURN(true);
}

KDDW_QCORO_TASK tst_dockWindowWithTwoSideBySideFramesIntoRight()
{
    EnsureTopLevelsDeleted e;

    auto fw = createFloatingWindow();
    auto dock2 = createDockWidget("doc2");
    nestDockWidget(dock2, fw->dropArea(), nullptr,
                   KDDockWidgets::Location_OnTop); // No we stack on top, unlike in previous test
    CHECK_EQ(fw->groups().size(), 2);

    auto fw2 = createFloatingWindow();
    fw2->view()->move(fw->x() + fw->width() + 100, fw->y());

    dragFloatingWindowTo(fw, fw2->dropArea(), DropLocation_Right); // Outer right instead of Left
    CHECK_EQ(fw2->groups().size(), 3);
    CHECK(fw2->dropArea()->checkSanity());

    KDDW_CO_RETURN(true);
}

KDDW_QCORO_TASK tst_dockWindowWithTwoSideBySideFramesIntoLeft()
{
    EnsureTopLevelsDeleted e;

    auto fw = createFloatingWindow();
    fw->setObjectName("fw1");

    auto dock2 = createDockWidget("doc2");
    nestDockWidget(dock2, fw->dropArea(), nullptr, KDDockWidgets::Location_OnLeft);
    CHECK_EQ(fw->groups().size(), 2);

    auto fw2 = createFloatingWindow();
    fw2->setObjectName("fw2");
    fw2->view()->move(fw->x() + fw->width() + 100, fw->y());

    CHECK(fw2->dropArea()->checkSanity());
    dragFloatingWindowTo(fw, fw2->dropArea(), DropLocation_Left);
    CHECK_EQ(fw2->groups().size(), 3);

    CHECK(fw2->dropArea()->checkSanity());

    KDDW_CO_RETURN(true);
}

static const auto s_tests = std::vector<std::function<KDDW_QCORO_TASK()>> {
    tst_invalidLayoutAfterRestore,
    tst_setFloatingWhenSideBySide,
    tst_dockWindowWithTwoSideBySideFramesIntoCenter,
    tst_dockWindowWithTwoSideBySideFramesIntoRight,
    tst_dockWindowWithTwoSideBySideFramesIntoLeft
};

#include "tests_main.h"
