import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Dialogs
import QtQuick.Controls 2.15
import QtMultimedia 6.0
// import Qt.labs.platform 1.1
// import QtQuick.Dialogs
import YourEyes 1.0

ApplicationWindow {
    id: mainWindow
    width: 500
    height: 100
    visible: true
    title: "Main"
    property string _Cbackground:"#001B1C"
    property string _CSelectable:"#001516"
    property int _loadMode:0
    color:_Cbackground

    // 将毫秒转换为 mm:ss 格式
    function formatTime(msecs) {
        if (msecs <= 0) return "00:00"
        var totalSeconds = Math.floor(msecs / 1000)
        var minutes = Math.floor(totalSeconds / 60)
        var seconds = totalSeconds % 60
        return (minutes < 10 ? "0" + minutes : minutes) + ":" +
                (seconds < 10 ? "0" + seconds : seconds)
    }



    // SCholder{id:sc}
    // ShaderManager{id:sm}

    AudioOutput {
        id: audioOutput
        volume: 0.8
        muted: false
    }


    MediaPlayer {
        id: mediaPlayer
        audioOutput: audioOutput
        loops: -1

        onDurationChanged: {
            if (duration > 0) {
                totalTimeText.text = formatTime(duration)
                seekSlider.to = duration
            }
        }
        onPositionChanged: {
            if (!seekSlider.pressed) {
                seekSlider.value = position
                currentTimeText.text = formatTime(position)
            }
        }

        // 错误处理
        onErrorOccurred: {
            console.log("播放错误:", errorString)
            mainWindow.title = "错误: " + errorString
        }
    }


    FileDialog {
        id: fileDialog
        title: "选择音频文件"
        fileMode:FileDialog.OpenFile
        nameFilters: ["音频文件 (*.mp3 *.wav *.flac *.m4a *.ogg)", "所有文件 (*)"]
        onAccepted: {
            mediaPlayer.source=fileDialog.selectedFile
            console.log(fileDialog.selectedFile)
            sc.loadscFromURL(fileDialog.selectedFile,_loadMode)
        }
    }
    FileDialog{
        id:shaderSrcDialog
        title:"选择片段着色器文件(glsl)"
        fileMode:FileDialog.OpenFile
        nameFilters:[
            "片段着色器 (*.frag *.fsh *.fs)",
            "所有文件 (*)"
        ]
        onAccepted:{
            shaderManager.registerShaderFromUrl(shaderSrcDialog.selectedFile)
        }
    }


    // 用户界面↓
    ColumnLayout {
        id:userInterface
        anchors.fill: parent
        anchors.margins: 10
        spacing: 15


        // 文件信息行
        RowLayout {
            Layout.fillWidth: true
            spacing: 5

            // Label {
            //     id: statusText
            //     text: "音频未选取"
            //     elide: Text.ElideLeft
            // }

            Button {
                id: playButton
                icon.name:"media-playback-start"
                onClicked: {
                    var src = mediaPlayer.source
                    if (!src || src.toString() === "") {
                        fileDialog.open()
                        return
                    }

                    if (mediaPlayer.playbackState === MediaPlayer.PlayingState) {
                        mediaPlayer.pause()
                        icon.name="media-playback-start"
                        // mainWindow.title = "已暂停"
                    } else {
                        mediaPlayer.play()
                        icon.name="media-playback-pause"
                        // mainWindow.title = "播放中..."
                    }
                }
                Rectangle{
                    anchors.fill:parent
                    color:_Cbackground
                }
            }

            Button {
                flat:true
                id:audioPicking
                // text: "选择文件"
                icon.name:"document-open"
                onClicked: fileDialog.open()
                background:Rectangle{
                    anchors.fill:parent
                    color:_Cbackground
                    radius:2
                }
            }

            Button{
                id:spectrumPicking
                flat:true
                width: 20
                height: 20
                background: Rectangle {
                    color: _Cbackground
                }
                contentItem: Text {
                    font.family: "iconfont"
                    text: "\ue68f"
                    font.pixelSize: 30
                    color: "#FFFFFF"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                onClicked:channelPopup.visible=true
            }

            Button{
                id:samplerPicking
                flat:true
                width: 20
                height: 20
                background: Rectangle {
                    color: _Cbackground
                }
                contentItem: Text {
                    font.family: "iconfont"
                    text: "\uebcc"
                    font.pixelSize: 30
                    font.bold: true
                    color: "#FFFFFF"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter

                }
                onClicked:samplerPopup.visible=true

            }
            Button{
                id:shaderPicking
                // flat:true
                flat:true
                width: 20
                height: 20
                background: Rectangle {
                    color: _Cbackground
                }
                contentItem: Text {
                    font.family: "iconfont"
                    text: "\ue695"
                    font.pixelSize: 30
                    font.bold: true
                    color: "#FFFFFF"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                onClicked:shaderPopup.visible=true
            }
        }

        // 进度时间轴区域
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 5

            // 滑块（时间轴）
            Slider {
                id: seekSlider
                from: 0
                to: 1000
                value: 0
                Layout.fillWidth: true
                enabled: mediaPlayer.duration > 0

                onMoved: {
                    mediaPlayer.position=value
                    // statusText=value
                }

                background: Rectangle {
                    x: seekSlider.leftPadding
                    y: seekSlider.topPadding + seekSlider.availableHeight / 2 - height / 2
                    implicitWidth: 200
                    implicitHeight: 4
                    width: seekSlider.availableWidth
                    height: implicitHeight
                    radius: 2
                    color: "#e0e0e0"

                    Rectangle {
                        width: seekSlider.visualPosition * parent.width
                        height: parent.height
                        color: "#777777"
                        radius: 2
                    }
                }

                handle: Rectangle {
                    x: seekSlider.leftPadding + seekSlider.visualPosition * (seekSlider.availableWidth - width)
                    y: seekSlider.topPadding + seekSlider.availableHeight / 2 - height / 2
                    implicitWidth: 14
                    implicitHeight: 14
                    radius: 7
                    color: seekSlider.pressed ? "#FFFFFF" : "#11BB77"
                    border.color: "#FFFFFF"
                    border.width: 2
                }
            }

            // 时间标签行
            RowLayout {
                Layout.fillWidth: true

                Label {
                    id: currentTimeText
                    text: "00:00"
                    font.pixelSize: 12
                    color: "#CCCCCC"
                }

                Item { Layout.fillWidth: true }

                Label {
                    id: totalTimeText
                    text: "00:00"
                    font.pixelSize: 12
                    color: "#CCCCCC"
                }
            }
        }

    }

    Window{
        id:channelPopup
        width:200;height:200
        // modal:true
        visible:false
        color:_Cbackground
        // title:"ChannelView"
        ListView{
            boundsBehavior: Flickable.StopAtBounds
            anchors.fill:parent
            model:sc
            delegate:Button{

                width:parent.width
                height:30
                // text:{model.name
                //     color:"#FFFFFF"
                // }
                Text {
                    text: model.name
                    color:"#FFFFFF"
                    anchors.centerIn: parent
                }

                background: Rectangle{
                    color:_CSelectable
                    anchors.fill:parent
                }

                onClicked:{
                    windowManager.openSpectrumWindow(index,sc.ptr,shaderManager.ptr,mediaPlayer,sm.ptr)
                    channelPopup.close()
                }
            }
        }
    }

    Window{
        id:samplerPopup
        width:200;height:300
        // modal:true
        visible:false
        color:_Cbackground
        property int state:0

        ColumnLayout {
            anchors.fill: parent
            spacing: 10

            RowLayout {
                Layout.fillWidth: true
                // spacing: 25
                Button {
                    flat:true
                    width: 20
                    height: 20
                    background: Rectangle {
                        color: _Cbackground
                    }
                    contentItem: Text {
                        font.family: "iconfont"
                        text: "\ue794"
                        font.pixelSize: 30
                        color: "#FFFFFF"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    onClicked: samplerTextPopup.visible = true
                }
                Button {
                    flat:true
                    width: 20
                    height: 20
                    background: Rectangle {
                        color: _Cbackground
                    }
                    contentItem: Text {
                        font.family: "iconfont"
                        text: "\ue620"
                        font.pixelSize: 30
                        color: "#FFFFFF"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    onClicked: {
                        if (samplerPopup.state === 0) {
                            samplerPopup.state = 1;
                            samplerListView.childrenColor = "#100000";
                        } else {
                            samplerPopup.state = 0;
                            samplerListView.childrenColor = _CSelectable;
                        }
                    }
                }
            }

            RowLayout {
                id:sV
                Layout.fillWidth: true
                Layout.topMargin: 5
                property real maxEnergy:100

                Rectangle {
                    id: energyBar
                    Layout.fillWidth: true
                    height: 18
                    color: "#000000"
                    property real normalized: sm.currentValue
                    Rectangle {
                        width: parent.width * Math.min(parent.normalized / sV.maxEnergy,1.)
                        height: parent.height
                        radius: parent.radius
                        color: "#00AAFF"
                    }
                }

                Label {

                    // text: energyBar.normalized
                    text: sV.maxEnergy.toFixed(1)
                    color: "#CCCCCC"
                    font.pixelSize: 10
                }
            }

            // 可选的：最大值调节滑块（若不需可删除）
            RowLayout {
                Layout.fillWidth: true
                Label {
                    text: "max:"
                    color: "#FFFFFF"
                    font.pixelSize: 12
                }
                Slider {
                    id: maxEnergySlider
                    from: 10.
                    to: 500.0
                    stepSize: 0.1
                    value: sV.maxEnergy
                    onValueChanged: sV.maxEnergy = value
                    Layout.fillWidth: true
                }
                Label {
                    text: maxEnergySlider.value.toFixed(1)
                    color: "#FFFFFF"
                    font.pixelSize: 12
                    Layout.preferredWidth: 30
                }
            }

            RowLayout {
                Layout.fillWidth: true
                Label {
                    text: "Ratio:"
                    color: "#FFFFFF"
                    font.pixelSize: 12
                }
                Slider {
                    id: ratioSlider
                    from: 0.0
                    to: 5.0
                    stepSize: 0.01
                    value: sm.p_ratio
                    onValueChanged: sm.p_ratio = value
                    Layout.fillWidth: true
                    enabled: sm.picked !== null   // 当有选中采样点时启用
                }
                Label {
                    text: ratioSlider.value.toFixed(2)
                    color: "#FFFFFF"
                    font.pixelSize: 12
                    Layout.preferredWidth: 40
                }
            }

            ListView {
                id: samplerListView
                Layout.fillWidth: true
                Layout.fillHeight: true
                property string childrenColor:_CSelectable
                model: sm
                delegate: Button {
                    width: parent.width
                    height:30
                    // text: model.name
                    Text{
                        text:model.name;
                        color:"#FFFFFF"
                        anchors.centerIn: parent
                    }

                    onClicked:{
                        if(samplerPopup.state===0){
                            sm.setSamplerPicked(model.name)
                        }else{
                            sm.deregisterSampler()
                            samplerPopup.state = 0;
                            samplerListView.childrenColor = _CSelectable;
                        }
                    }
                    background: Rectangle{
                        id:samplerButtonBackground
                        color:samplerListView.childrenColor;
                    }
                }
            }
        }
        Popup {
            id: samplerTextPopup
            width: samplerPopup.width
            height: samplerPopup.height
            closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
            background: Rectangle{
                anchors.fill:parent
                color:_CSelectable
            }

            // 对外暴露当前输入文本的属性（可选）
            property alias inputText: textField.text

            // 信号：点击确定时发射，携带用户输入的文字
            signal textConfirmed(string text)

            // 弹窗内容布局
            Column {
                anchors.centerIn: parent
                spacing: 12
                width: parent.width - 40

                TextField {
                    id: textField
                    width: parent.width
                    placeholderText: "请输入采样点名称"
                    focus: true      // 弹窗打开时自动获得焦点
                }

                Button {
                    text: "确定"
                    width: parent.width
                    onClicked: {

                        // textConfirmed(textField.text)

                        sm.registerSampler(textField.text);


                        // 关闭弹窗
                        samplerTextPopup.close()
                    }
                }
            }
        }
    }


    Window{
        id:shaderPopup
        width:200;height:200
        visible:false
        color:_Cbackground
        property int state:0;

        ColumnLayout {
            anchors.fill: parent
            spacing: 10

            RowLayout {
                Layout.fillWidth: true
                Button {
                    flat:true
                    width: 20
                    height: 20
                    background: Rectangle {
                        color: _Cbackground
                    }
                    contentItem: Text {
                        font.family: "iconfont"
                        text: "\ue794"
                        font.pixelSize: 30
                        color: "#FFFFFF"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    // onClicked: samplerTextPopup.visible = true
                    onClicked: shaderSrcDialog.open()
                }
                Button {
                    flat:true
                    width: 20
                    height: 20
                    background: Rectangle {
                        color: _Cbackground
                    }
                    contentItem: Text {
                        font.family: "iconfont"
                        text: "\ue620"
                        font.pixelSize: 30
                        color: "#FFFFFF"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    onClicked:{
                        if(shaderPopup.state===0){
                            shaderPopup.state=1;
                            shaderListView.childrenColor="#100000"
                        }
                        else{
                            shaderPopup.state=0;
                            shaderListView.childrenColor=_CSelectable
                        }
                    }
                }
            }

            ListView {
                id: shaderListView
                Layout.fillWidth: true
                Layout.fillHeight: true
                model: shaderManager
                property string childrenColor:_CSelectable
                delegate: Button {
                    width: parent.width
                    height:30
                    // text: model.name
                    Text{
                        text:model.display;
                        color:"#FFFFFF"
                        anchors.centerIn: parent
                    }

                    onClicked:{
                        if(shaderPopup.state===0){
                            windowManager.openShaderWindow(1,sc.ptr,shaderManager.ptr,mediaPlayer,sm.ptr,index)
                        } else {
                            // shaderManager
                        }
                    }
                    background: Rectangle{
                        id:shaderButtonBackground
                        color:shaderListView.childrenColor
                    }
                }
            }
        }
    }

    // 监听媒体加载状态
    Connections {
        target: mediaPlayer
        function onMediaStatusChanged() {
            if (mediaPlayer.mediaStatus === MediaPlayer.LoadedMedia) {
                mainWindow.title = "就绪"
                mediaPlayer.pause()
                playButton.icon.name="media-playback-start"
                if (mediaPlayer.metaData.title) {
                    mainWindow.title = mediaPlayer.metaData.title
                } else {
                    var path = mediaPlayer.source.toString()
                    var fileName = path.substring(path.lastIndexOf("/") + 1)
                    fileName = decodeURIComponent(fileName)
                    mainWindow.title = fileName
                }
            } else if (mediaPlayer.mediaStatus === MediaPlayer.LoadingMedia) {
                mainWindow.title = "Loading..."
            }
        }
    }

    // 应用启动时显示提示
    Component.onCompleted: {
        mainWindow.title = "就绪"
    }
    onClosing: {
        Qt.quit()
    }
}