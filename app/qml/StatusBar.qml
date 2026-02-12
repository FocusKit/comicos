import QtQuick
import QtQuick.Layouts
import Comicos 1.0

Rectangle {
    id: statusBar

    height: Theme.statusBarHeight
    color: Theme.bgSecondary

    // Top border
    Rectangle {
        anchors.top: parent.top
        width: parent.width
        height: 1
        color: Theme.borderColor
    }

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: Theme.spacingLarge
        anchors.rightMargin: Theme.spacingLarge
        spacing: Theme.spacingLarge

        // Current tool
        Text {
            text: {
                switch (AppController.currentTool) {
                case 0: return "펜"
                case 1: return "지우개"
                case 2: return "채우기"
                default: return ""
                }
            }
            font.pixelSize: Theme.fontSizeSmall
            font.weight: Font.Medium
            color: Theme.textSecondary
        }

        // Separator
        Rectangle { width: 1; height: 12; color: Theme.borderColor }

        // Brush size
        Text {
            text: "크기: " + Math.round(AppController.brushSize) + "px"
            font.pixelSize: Theme.fontSizeSmall
            color: Theme.textTertiary
        }

        Rectangle { width: 1; height: 12; color: Theme.borderColor }

        // Canvas size
        Text {
            text: "캔버스: 2480 × 3508"
            font.pixelSize: Theme.fontSizeSmall
            color: Theme.textTertiary
        }

        Item { Layout.fillWidth: true }

        // Version
        Text {
            text: "v" + COMICOS_VERSION
            font.pixelSize: Theme.fontSizeSmall
            color: Theme.textTertiary
            // COMICOS_VERSION is defined as C++ macro, needs bridge
            // Extension point: expose version from AppController
            visible: false
        }
    }
}
