/*
  This file is part of KDDockWidgets.

  SPDX-FileCopyrightText: 2020-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Sergio Martins <sergio.martins@kdab.com>

  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

import QtQuick 2.6
import QtQuick.Controls 2.12
import com.kdab.dockwidgets 2.0 as KDDW
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15

ApplicationWindow {
    id:pp
    visible: true
    width: 1000
    height: 800
    property var obj:[]


    ColumnLayout{
        anchors.fill: parent
        Button{
            property int indexx: 0
            Layout.fillWidth: true
            Layout.fillHeight: false
            text: "make tab"
            onClicked: {
                var dd=componen.createObject(sta,{indss:indexx})
                //                sta.push(dd)

                //                pp.obj.push(dd);
                //                dd.show();
                barb.createObject(bar,{text:indexx})
                indexx++

            }
        }

        TabBar{
            id:bar
            Layout.fillHeight: false
            Layout.fillWidth: true
            visible: true

        }

        StackLayout{
            id:sta
            Layout.fillHeight: true
            Layout.fillWidth: true
                        currentIndex: bar.currentIndex

            //            onCurrentIndexChanged:{
            //                console.log("size"+pp.obj.length)

            //                console.log("=====================================")
            //                for(var e=0;e<pp.obj.length;e++){
            //                    console.log(e+"  "+pp.obj[e].visible +"   "+pp.obj[e].indss)
            //                }
            //                console.log("=====================================")
            //            }


        }

    }

    Component{
        id:barb
        TabButton{

        }
    }
    Component{
        id:componen
        Rectangle{
            id:dockWidgetArea
            property int indss
            //            title: indss
            //            width: 200
            visible: true
            //            height: 500

            KDDW.DockingArea {
                anchors.fill: parent
                //                property int indss
                width: 200
                height: 500
                //            Layout.fillHeight: true
                //            Layout.fillWidth: true


                uniqueName: "MyMainLayout"+dockWidgetArea.indss

                KDDW.DockWidget {
                    id: dock4
                    uniqueName: "dock4"+dockWidgetArea.indss
                    source: "qrc:/Guest4.qml"

                }
                KDDW.DockWidget {
                    id: dock1
                    uniqueName: "dock1"+dockWidgetArea.indss
                    source: "qrc:/Guest4.qml"

                }
                KDDW.DockWidget {
                    id: dock2
                    uniqueName: "dock2"+dockWidgetArea.indss
                    source: "qrc:/Guest4.qml"

                }
                KDDW.DockWidget {
                    id: dock3
                    uniqueName: "dock3"+dockWidgetArea.indss
                    source: "qrc:/Guest4.qml"

                }

                Component.onCompleted: {
                    // The other 3 dock widgets are created via C++ in main.cpp
                    // For illustration purposes, here's a .qml version. Maybe it's te preferred form even.
                    addDockWidget(dock1, KDDW.KDDockWidgets.Location_OnBottom);
                    addDockWidget(dock2, KDDW.KDDockWidgets.Location_OnBottom);
                    addDockWidget(dock3, KDDW.KDDockWidgets.Location_OnBottom);
                    addDockWidget(dock4, KDDW.KDDockWidgets.Location_OnBottom);

                }
            }
        }
    }



}
