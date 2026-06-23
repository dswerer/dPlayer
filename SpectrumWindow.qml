import QtQuick 6.0
import QtQuick.Window 2.2
import YourEyes 1.0
import QtMultimedia
import QtQuick.Controls
import QtQuick.Layouts

Window{
    id:spctWin
    width:600;height:120
    visible:true


    property MediaPlayer mediaPlayer:null

    MouseArea{
        id:clicky
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton|Qt.RightButton
        onClicked:(mouse)=>{
            sm.p_target(bridge.getChannelLiteral())
            if(mouse.button===Qt.RightButton){
                sm.p_rangeR=mouse.x/spctWin.width*bridge.mzoom
                // console.log(sm.p_rangeR)
            }
            else if(mouse.button===Qt.LeftButton){
                sm.p_rangeL=mouse.x/spctWin.width*bridge.mzoom
                // console.log(sm.p_rangeL)
            }
            spctWin.update()
        }
    }

    OpenGLBridge{
        id:bridge
        anchors.fill:parent
        animationValue: mediaPlayer?mediaPlayer.position:0
        mwidth: spctWin.width * (spctWin.Screen ? spctWin.Screen.devicePixelRatio : 1)
        mheight: spctWin.height * (spctWin.Screen ? spctWin.Screen.devicePixelRatio : 1)
        mamp:s_amp.value
        mzoom:s_zoom.value
    }


    Component.onDestruction: {
        // panel.visible=false
        panel.destroy()
    }

    Window{
        title:"panel"
        id:panel
        visible:true
        width:200;height:90
        color:"#000000"
        ColumnLayout{
            Layout.fillWidth: true
            //anchors.margins: 15
            spacing:5
            Label{
                anchors.topMargin: 5
                text:"Intensity"
                color:"#AAAAAA"
            }

            Slider{
                id:s_amp
                from:0
                to:1
                value:0.5
                Layout.fillWidth: true
                background: Rectangle{
                    x:s_amp.leftPadding
                    y:s_amp.topPadding+s_amp.availableHeight / 2 - height / 2
                    implicitWidth: 200
                    implicitHeight: 4
                    width:s_amp.availableWidth
                    height:implicitHeight
                    color:"#e0e0e0"
                }
                handle:Rectangle{
                    x:s_amp.leftPadding+s_amp.visualPosition*(s_amp.availableWidth-width)
                    y: s_amp.topPadding + s_amp.availableHeight / 2 - height / 2
                    implicitWidth: 10
                    implicitHeight: 10
                    radius: 2
                    color: s_amp.pressed ? "#FFFFFF" : "#CCCCCC"
                    border.color: "#FFFFFF"
                    border.width: 1
                }
            }
            Label{
                text:"Zoom"
                color:"#AAAAAA"
            }
            Slider{
                id:s_zoom
                from:0.2
                to:2
                value:1
                Layout.fillWidth: true
                background: Rectangle{
                    x:s_zoom.leftPadding
                    y:s_zoom.topPadding+s_zoom.availableHeight / 2 - height / 2
                    implicitWidth: 200
                    implicitHeight: 4
                    width:s_zoom.availableWidth
                    height:implicitHeight
                    color:"#e0e0e0"
                }
                handle:Rectangle{
                    x:s_zoom.leftPadding+s_zoom.visualPosition*(s_zoom.availableWidth-width)
                    y: s_zoom.topPadding + s_zoom.availableHeight / 2 - height / 2
                    implicitWidth: 10
                    implicitHeight: 10
                    radius: 2
                    color: s_zoom.pressed ? "#FFFFFF" : "#CCCCCC"
                    border.color: "#FFFFFF"
                    border.width: 1
                }
            }
        }
    }


}

