import QtQuick
import QtQuick.Window
import QtQuick.Dialogs
import QtQuick.Controls

Window {
  width: 640
  height: 480
  visible: true
  title: qsTr("File watcher")

  Rectangle {
    anchors.margins: 10
    anchors.fill: parent

    Column {
      width: parent.width
      spacing: 2

      Button {
        id: addFolderButton
        width: parent.width

        text: qsTr("Add new folder")
        onClicked: folderDialog.open()
      }

      FolderDialog {
        id: folderDialog
        title: qsTr("Please select folder to watch")

        onAccepted: {
          pathsModel.append(folderDialog.selectedFolder)
        }
      }

      Text {
        id: listViewHeader
        topPadding: 10

        text: qsTr("Watched Paths")
      }

      Rectangle {
        width: parent.width
        height: 150
        border.width: 1
        border.color: "black"
        radius: 10

        Component {
          id: pathsDelegate
          Rectangle {
            id: wrapper
            height: 30
            width: listView.width

            border.width: 1
            border.color: "black"

            // radius: 5
            color: index % 2 == 1 ? "lightgray" : "white"

            Text {
              id: pathText
              padding: 5
              text: model.display
            }

            Button {
              id: removeButton
              anchors.right: parent.right
              anchors.top: parent.top
              width: 100
              height: parent.height - 10

              anchors.margins: 5
              text: qsTr("Remove")

              onClicked: pathsModel.remove(index)
            }
          }
        }

        ListView {
          id: listView
          anchors.fill: parent

          model: pathsModel
          delegate: pathsDelegate
          focus: true
          clip: true
          smooth: true
        }
      }

      Rectangle {
        width: parent.width
        height: 200
        border.color: "black"
        border.width: 1
        radius: 10

        HorizontalHeaderView {
          id: tableHeader
          syncView: tableView
          anchors.top: parent.top
          height: 30
          width: parent.width
          z: 1
        }

        TableView {
          id: tableView
          anchors.fill: parent
          topMargin: tableHeader.implicitHeight
          clip: true
          z: 0

          function getColumnWeight(column) {
            switch (column) {
            case 0:
              return 0.1
            case 1:
              return 0.5
            case 2:
              return 0.1
            case 3:
              return 0.3
            default:
              break
            }
          }

          delegate: Rectangle {
            implicitWidth: tableView.width * tableView.getColumnWeight(
                             model.column)
            implicitHeight: 30
            border.color: "Black"
            border.width: 1

            Text {
              id: tableViewText
              text: model.display
              padding: 5
            }
          }

          model: fileEventsModel
        }
      }

      Row {
        spacing: 20
        topPadding: 20

        Button {
          id: clearButton
          text: "Clear"
          onClicked: controller.clearTable()
        }

        Button {
          id: startButton
          text: qsTr("Start")

          onClicked: {
            controller.startWatching()
          }
        }

        Button {
          id: stopButton
          text: qsTr("Stop")
          onClicked: controller.stopWatching()
        }

        Text {
          id: watchState
          leftPadding: 30
          text: controller.watcherState === true ? "Watching started" : "Stopped"
          color: controller.watcherState === true ? "green" : "red"
        }
      }
    }
  }
}
