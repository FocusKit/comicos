import QtQuick
import QtQuick.Layouts
import Comicos 1.0

/// Segmented control matching the textos design language.
/// model: array of { label: string, value: int }
Rectangle {
    id: control

    property var model: []
    property int currentIndex: 0

    signal selected(int index)

    implicitWidth: row.implicitWidth + 6
    implicitHeight: 30
    radius: 10
    color: Theme.bgTertiary

    Row {
        id: row
        anchors.centerIn: parent
        spacing: 2
        padding: 3

        Repeater {
            model: control.model

            Rectangle {
                required property var modelData
                required property int index

                width: segLabel.implicitWidth + 24
                height: 24
                radius: Theme.radiusMedium
                color: control.currentIndex === index
                       ? Theme.bgElevated
                       : "transparent"

                // Subtle shadow for active segment
                layer.enabled: control.currentIndex === index
                layer.effect: null  // Extension point: DropShadow

                Text {
                    id: segLabel
                    anchors.centerIn: parent
                    text: modelData.label
                    font.pixelSize: Theme.fontSizeLabel
                    font.weight: control.currentIndex === index ? Font.Medium : Font.Normal
                    color: control.currentIndex === index
                           ? Theme.textPrimary
                           : Theme.textSecondary
                }

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: control.selected(index)
                }

                Behavior on color { ColorAnimation { duration: Theme.animFast } }
            }
        }
    }
}
