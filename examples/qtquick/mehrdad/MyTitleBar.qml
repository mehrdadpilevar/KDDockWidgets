/*
  This file is part of KDDockWidgets.

  SPDX-FileCopyrightText: 2020-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Sergio Martins <sergio.martins@kdab.com>

  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

import QtQuick 2.6

// Will be moved to a plugin in the future, if there's enough demand
import "qrc:/kddockwidgets/qtquick/views/qml/" as KDDW

KDDW.TitleBarBase {
    id: root
    color:  isdark?"#333333":"#e2e2e2"
    heightWhenVisible: 25

    Text {
        color: !isdark?"#333333":"#e2e2e2"
        text: root.title
        anchors {
            left: parent.left
            leftMargin: 10
            verticalCenter: root.verticalCenter
        }
    }

//    Rectangle {
//        id: closeButton
//        enabled: root.closeButtonEnabled
//        radius: 5
//        color: "red"
//        height: root.height - 20
//        width: height
//        anchors {
//            right: root.right
//            rightMargin: 10
//            verticalCenter: root.verticalCenter
//        }
//        MouseArea {
//            anchors.fill: parent
//            onClicked: {
//                root.closeButtonClicked();
//            }
//        }
//    }
}
