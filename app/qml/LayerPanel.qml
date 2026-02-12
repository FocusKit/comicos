import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Comicos 1.0
import "controls"

Rectangle {
    id: layerPanel

    color: Theme.bgSecondary

    readonly property var layerModel: AppController.layerModel

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // Header
        Rectangle {
            Layout.fillWidth: true
            height: 36
            color: "transparent"

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: Theme.spacingLarge
                anchors.rightMargin: Theme.spacingMedium
                spacing: Theme.spacingSmall

                Label {
                    text: "\uB808\uC774\uC5B4"
                    font.pixelSize: Theme.fontSizeLabel
                    font.weight: Font.DemiBold
                    color: Theme.textPrimary
                    Layout.fillWidth: true
                }

                IconButton {
                    iconText: "+"
                    tooltip: "\uB808\uC774\uC5B4 \uCD94\uAC00"
                    width: 24
                    height: 24
                    onClicked: layerModel.addLayer()
                }
            }

            // Bottom border
            Rectangle {
                anchors.bottom: parent.bottom
                width: parent.width
                height: 1
                color: Theme.borderColor
            }
        }

        // Layer list
        ListView {
            id: layerList
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: layerModel
            clip: true
            currentIndex: layerModel.activeLayerIndex

            delegate: Rectangle {
                id: layerDelegate
                width: layerList.width
                height: 40
                color: layerList.currentIndex === index
                       ? Theme.accentLight
                       : (layerHover.hovered ? Theme.bgTertiary : "transparent")

                required property int index
                required property string layerName
                required property real layerOpacity
                required property bool layerVisible

                property bool editing: false

                HoverHandler { id: layerHover }

                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: Theme.spacingLarge
                    anchors.rightMargin: Theme.spacingMedium
                    spacing: Theme.spacingMedium

                    // Visibility toggle
                    IconButton {
                        iconText: layerDelegate.layerVisible ? "\uD83D\uDC41" : "\u2014"
                        width: 24
                        height: 24
                        opacity: layerDelegate.layerVisible ? 1.0 : 0.4
                        onClicked: layerModel.setLayerVisible(layerDelegate.index, !layerDelegate.layerVisible)
                    }

                    // Layer name (label or text field)
                    Label {
                        id: nameLabel
                        text: layerDelegate.layerName
                        font.pixelSize: Theme.fontSizeLabel
                        color: Theme.textPrimary
                        elide: Text.ElideRight
                        Layout.fillWidth: true
                        visible: !layerDelegate.editing
                    }

                    TextField {
                        id: nameField
                        text: layerDelegate.layerName
                        font.pixelSize: Theme.fontSizeLabel
                        color: Theme.textPrimary
                        Layout.fillWidth: true
                        visible: layerDelegate.editing
                        background: Rectangle {
                            radius: Theme.radiusSmall
                            color: Theme.bgPrimary
                            border.width: 1
                            border.color: Theme.accent
                        }
                        padding: 2
                        leftPadding: 4
                        rightPadding: 4

                        onAccepted: {
                            layerModel.setLayerName(layerDelegate.index, text)
                            layerDelegate.editing = false
                        }
                        Keys.onEscapePressed: {
                            text = layerDelegate.layerName
                            layerDelegate.editing = false
                        }
                        onActiveFocusChanged: {
                            if (!activeFocus && layerDelegate.editing) {
                                text = layerDelegate.layerName
                                layerDelegate.editing = false
                            }
                        }
                        Component.onCompleted: {
                            if (layerDelegate.editing) {
                                forceActiveFocus()
                                selectAll()
                            }
                        }
                    }

                    // Opacity percentage
                    Label {
                        text: Math.round(layerDelegate.layerOpacity * 100) + "%"
                        font.pixelSize: Theme.fontSizeSmall
                        color: Theme.textTertiary
                        visible: !layerDelegate.editing
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    z: -1
                    onClicked: {
                        layerModel.activeLayerIndex = layerDelegate.index
                    }
                    onDoubleClicked: {
                        layerModel.activeLayerIndex = layerDelegate.index
                        layerDelegate.editing = true
                        nameField.forceActiveFocus()
                        nameField.selectAll()
                    }
                }

                // Bottom border
                Rectangle {
                    anchors.bottom: parent.bottom
                    width: parent.width
                    height: 1
                    color: Theme.borderColor
                    opacity: 0.5
                }
            }
        }

        // Footer separator
        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: Theme.borderColor
        }

        // Footer: opacity slider
        RowLayout {
            Layout.fillWidth: true
            Layout.leftMargin: Theme.spacingLarge
            Layout.rightMargin: Theme.spacingMedium
            Layout.topMargin: Theme.spacingSmall
            spacing: Theme.spacingSmall

            Label {
                text: "\uBD88\uD22C\uBA85\uB3C4"
                font.pixelSize: Theme.fontSizeSmall
                color: Theme.textSecondary
            }

            Slider {
                id: opacitySlider
                Layout.fillWidth: true
                from: 0
                to: 1
                stepSize: 0.01
                value: layerModel.activeLayerOpacity

                onMoved: {
                    layerModel.activeLayerOpacity = value
                }

                background: Rectangle {
                    x: opacitySlider.leftPadding
                    y: opacitySlider.topPadding + opacitySlider.availableHeight / 2 - height / 2
                    width: opacitySlider.availableWidth
                    height: 4
                    radius: 2
                    color: Theme.bgTertiary

                    Rectangle {
                        width: opacitySlider.visualPosition * parent.width
                        height: parent.height
                        radius: 2
                        color: Theme.accent
                    }
                }

                handle: Rectangle {
                    x: opacitySlider.leftPadding + opacitySlider.visualPosition * (opacitySlider.availableWidth - width)
                    y: opacitySlider.topPadding + opacitySlider.availableHeight / 2 - height / 2
                    width: 14
                    height: 14
                    radius: 7
                    color: Theme.bgElevated
                    border.width: 2
                    border.color: Theme.accent
                }
            }

            Label {
                text: Math.round(layerModel.activeLayerOpacity * 100) + "%"
                font.pixelSize: Theme.fontSizeSmall
                color: Theme.textSecondary
                Layout.preferredWidth: 32
                horizontalAlignment: Text.AlignRight
            }
        }

        // Footer: action buttons
        RowLayout {
            Layout.fillWidth: true
            Layout.leftMargin: Theme.spacingMedium
            Layout.rightMargin: Theme.spacingMedium
            Layout.topMargin: Theme.spacingTiny
            Layout.bottomMargin: Theme.spacingSmall
            spacing: Theme.spacingSmall

            IconButton {
                iconText: "\u25B2"
                tooltip: "\uB808\uC774\uC5B4 \uC704\uB85C"
                width: 28
                height: 28
                enabled: layerModel.activeLayerIndex > 0
                onClicked: {
                    let idx = layerModel.activeLayerIndex
                    layerModel.moveLayer(idx, idx - 1)
                }
            }

            IconButton {
                iconText: "\u25BC"
                tooltip: "\uB808\uC774\uC5B4 \uC544\uB798\uB85C"
                width: 28
                height: 28
                enabled: layerModel.activeLayerIndex < layerList.count - 1
                onClicked: {
                    let idx = layerModel.activeLayerIndex
                    layerModel.moveLayer(idx, idx + 1)
                }
            }

            IconButton {
                iconText: "\u29C9"
                tooltip: "\uB808\uC774\uC5B4 \uBCF5\uC81C"
                width: 28
                height: 28
                onClicked: layerModel.duplicateLayer(layerModel.activeLayerIndex)
            }

            Item { Layout.fillWidth: true }

            IconButton {
                iconText: "\uD83D\uDDD1"
                tooltip: "\uB808\uC774\uC5B4 \uC0AD\uC81C"
                width: 28
                height: 28
                enabled: layerList.count > 1
                onClicked: layerModel.removeLayer(layerModel.activeLayerIndex)
            }
        }
    }
}
