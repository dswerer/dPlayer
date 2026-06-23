import QtQuick 6.0
import QtQuick.Window 2.2
import YourEyes 1.0
import QtMultimedia
import QtQuick.Controls
import QtQuick.Layouts

Window{
    id:shaderWin
    width:400;height:300
    visible:true

    MouseArea{
        anchors.fill:parent
        onClicked:(mouse)=>{
            panel.visible=true
        }
    }

    property MediaPlayer mediaPlayer:null

    OpenGLBridge{
        id:bridge
        anchors.fill:parent
        animationValue: mediaPlayer?mediaPlayer.position:0
        mwidth: shaderWin.width * (shaderWin.Screen ? shaderWin.Screen.devicePixelRatio : 1)
        mheight: shaderWin.height * (shaderWin.Screen ? shaderWin.Screen.devicePixelRatio : 1)
        mamp:s_amp.value
        mzoom:s_zoom.value
    }

    // Component.onDestruction: {
    //     panel.destroy()
    // }

    Window{
        title:"shaderPanel"
        id:panel
        visible:true
        width:200;height:90
        color:"#00100A"
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