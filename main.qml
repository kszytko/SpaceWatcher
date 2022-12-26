import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Dialogs
import QtQuick.Controls

Window {
  width: 640
  height: 480
  visible: true
  title: qsTr("Hello World")

  Rectangle {
    anchors.margins: 10
    anchors.fill: parent

    //color: "red"
    Column {
      width: parent.width
      spacing: 2

      //color: "green"
      Button {
        id: addFolderButton
        width: parent.width

        text: qsTr("Add new folder")
        onClicked: folderDialog.open()
      }

      FolderDialog {
        id: folderDialog
        onAccepted: {
          watchedPathsModel.append(selectedFolder)
        }
      }

      Text {
        id: listViewHeader
        //anchors.top: addFolderButton.bottom
        topPadding: 10

        text: qsTr("Watched Paths")
      }

      Rectangle {
        //anchors.top: listViewHeader.bottom
        width: parent.width
        height: 150
        color: "green"

        Component {
          id: pathsDelegate
          Rectangle {
            id: wrapper
            //implicitHeight: pathText.implicitHeight
            height: 30
            width: listView.width

            border.width: 1
            border.color: "black"
            radius: 5

            color: index % 2 == 1 ? "lightgray" : "white"

            Text {
              id: pathText
              text: model.display
            }

            Button {
              id: removeButton
              anchors.right: parent.right
              anchors.top: parent.top
              width: 100
              height: parent.height - 10

              anchors.margins: 5
              text: "Remove"
              onClicked: watchedPathsModel.remove(index)
            }
          }
        }

        ListView {
          id: listView
          //anchors.top: listViewHeader.bottom
          anchors.fill: parent

          model: watchedPathsModel
          delegate: pathsDelegate
          focus: true
          clip: true
          smooth: true
        }
      }

      Rectangle {
        width: parent.width
        height: 200
        border.width: 1
        border.color: "black"

        Component {
          id: pathsWatchDelegate
          Rectangle {
            implicitHeight: watcherText.implicitHeight
            width: tableView.width

            border.width: 1
            border.color: "black"
            radius: 5

            Text {
              id: watcherText
              text: "text"
            }
          }
        }
        TableView {
          id: tableView
          anchors.fill: parent
          model: 10
          delegate: pathsWatchDelegate

          clip: true
        }
      }
      Row {
        spacing: 20
        Button {
          text: "start"
        }
        Button {
          text: "start"
        }
        Button {
          text: "start"
        }
      }
    }
  }
}
