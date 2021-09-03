import QtQuick 2.12
import QtQuick.Window 2.12
import QtReadSerial 1.12
import QtQuick.Controls 2.12

ApplicationWindow {
    width: 240
    height: 360
    visible: true
    title: qsTr("Demo M502")

    Rectangle {
        anchors.fill: parent
        ReadSerial {
            id: read
            onErrRead: console.log("onErrRead")
            onSuccessRead: console.log(tag)
            onGetTagChanged: console.log(tag)
            onSuccessLoginChanged: console.log("onSuccessLoginChanged")
            onDataChanged: console.log("onDataChanged", data)
        }

        Button {
            id: get
            anchors.top: parent.top
            anchors.topMargin: 10
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Get Tag"
            onClicked: read.getTag()
        }

        Button {
            id: pTag
            anchors.top: get.bottom
            anchors.topMargin: 10
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Persistent Tag"
            onClicked: read.persistentTag()
        }

        Button {
            id: autoLogin
            anchors.top: pTag.bottom
            anchors.topMargin: 10
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Persistent Login"
            onClicked: read.persistentLogin(0,1,"A","FFFFFFFFFFFF")
        }

        Button {
            id: login
            anchors.top: autoLogin.bottom
            anchors.topMargin: 10
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Login"
            onClicked: read.login(0,"A","FFFFFFFFFFFF")
        }

        Button {
            id:readBt
            anchors.top: login.bottom
            anchors.topMargin: 10
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Read"
            onClicked: read.read(1)
        }

        Button {
            anchors.top: readBt.bottom
            anchors.topMargin: 10
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Write"
            onClicked: read.write(1, "11000000000000000000000000000022")
        }
    }
}
