import QtQuick
import QtQuick.Layouts
import Comicos 1.0

/// Reusable modal dialog matching the textos design language.
/// Usage:
///   Modal {
///       title: "Title"
///       message: "Message"
///       actions: [
///           { label: "OK", variant: "primary", action: () => { ... } },
///           { label: "Cancel", variant: "secondary", action: () => { ... } }
///       ]
///   }
/// Variants: "primary" (accent), "danger" (red text), "secondary" (gray)
Item {
    id: modal

    property string title: ""
    property string message: ""
    property var actions: []

    signal closed()

    anchors.fill: parent
    visible: false
    z: 1000

    // Escape to close
    focus: visible
    Keys.onEscapePressed: modal.closed()

    // Backdrop
    Rectangle {
        anchors.fill: parent
        color: Theme.overlay
        opacity: modal.visible ? 1 : 0

        Behavior on opacity {
            NumberAnimation { duration: Theme.animFast }
        }

        MouseArea {
            anchors.fill: parent
            onClicked: modal.closed()
        }
    }

    // Content card
    Rectangle {
        id: card
        anchors.centerIn: parent
        width: 380
        height: contentColumn.implicitHeight + 48
        radius: Theme.radiusLarge
        color: Theme.bgElevated
        border.width: 1
        border.color: Theme.borderColor

        // Shadow
        layer.enabled: true
        layer.effect: null

        // Prevent clicks from reaching backdrop
        MouseArea { anchors.fill: parent }

        // Scale/fade entrance animation
        scale: modal.visible ? 1.0 : 0.95
        opacity: modal.visible ? 1.0 : 0.0

        Behavior on scale {
            NumberAnimation { duration: Theme.animNormal; easing.type: Easing.OutCubic }
        }
        Behavior on opacity {
            NumberAnimation { duration: Theme.animFast }
        }

        ColumnLayout {
            id: contentColumn
            anchors.fill: parent
            anchors.margins: 24
            spacing: Theme.spacingMedium

            // Title
            Text {
                text: modal.title
                font.pixelSize: Theme.fontSizeTitle
                font.weight: Font.Bold
                color: Theme.textPrimary
                Layout.fillWidth: true
            }

            // Message
            Text {
                text: modal.message
                font.pixelSize: Theme.fontSizeBody
                color: Theme.textSecondary
                wrapMode: Text.Wrap
                lineHeight: 1.4
                Layout.fillWidth: true
            }

            Item { height: Theme.spacingSmall }

            // Action buttons
            ColumnLayout {
                Layout.fillWidth: true
                spacing: Theme.spacingMedium

                Repeater {
                    model: modal.actions

                    Rectangle {
                        required property var modelData
                        required property int index

                        Layout.fillWidth: true
                        height: 40
                        radius: Theme.radiusMedium
                        color: {
                            let v = modelData.variant
                            if (v === "primary") {
                                if (btnArea.pressed) return Theme.accentHover
                                if (btnArea.containsMouse) return Qt.darker(Theme.accent, 1.1)
                                return Theme.accent
                            }
                            if (v === "danger") {
                                if (btnArea.pressed) return Theme.dark ? Qt.rgba(1,1,1,0.08) : Qt.rgba(0,0,0,0.06)
                                if (btnArea.containsMouse) return Theme.dark ? Qt.rgba(1,1,1,0.04) : Qt.rgba(0,0,0,0.03)
                                return "transparent"
                            }
                            // secondary
                            if (btnArea.pressed) return Theme.bgTertiary
                            if (btnArea.containsMouse) return Qt.darker(Theme.bgSecondary, 1.05)
                            return Theme.bgSecondary
                        }

                        Text {
                            anchors.centerIn: parent
                            text: modelData.label
                            font.pixelSize: Theme.fontSizeBody
                            font.weight: Font.Medium
                            color: {
                                let v = modelData.variant
                                if (v === "primary") return "#ffffff"
                                if (v === "danger") {
                                    return btnArea.containsMouse ? Theme.dangerHover : Theme.danger
                                }
                                return Theme.textPrimary
                            }
                        }

                        MouseArea {
                            id: btnArea
                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor
                            onClicked: {
                                if (modelData.action) modelData.action()
                            }
                        }

                        Behavior on color {
                            ColorAnimation { duration: Theme.animFast }
                        }
                    }
                }
            }
        }
    }
}
