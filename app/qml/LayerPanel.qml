import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Comicos 1.0
import "controls"

Rectangle {
    id: layerPanel

    color: Theme.bgSecondary

    DocumentModel {
        id: layerModel
        // Extension point: bind to AppController.document
    }

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
                    text: "레이어"
                    font.pixelSize: Theme.fontSizeLabel
                    font.weight: Font.DemiBold
                    color: Theme.textPrimary
                    Layout.fillWidth: true
                }

                IconButton {
                    iconText: "+"
                    tooltip: "레이어 추가"
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

                HoverHandler { id: layerHover }

                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: Theme.spacingLarge
                    anchors.rightMargin: Theme.spacingMedium
                    spacing: Theme.spacingMedium

                    // Visibility toggle
                    IconButton {
                        iconText: layerDelegate.layerVisible ? "👁" : "—"
                        width: 24
                        height: 24
                        opacity: layerDelegate.layerVisible ? 1.0 : 0.4
                        onClicked: layerModel.setLayerVisible(layerDelegate.index, !layerDelegate.layerVisible)
                    }

                    // Layer name
                    Label {
                        text: layerDelegate.layerName
                        font.pixelSize: Theme.fontSizeLabel
                        color: Theme.textPrimary
                        elide: Text.ElideRight
                        Layout.fillWidth: true
                    }

                    // Opacity
                    Label {
                        text: Math.round(layerDelegate.layerOpacity * 100) + "%"
                        font.pixelSize: Theme.fontSizeSmall
                        color: Theme.textTertiary
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    z: -1
                    onClicked: {
                        layerModel.activeLayerIndex = layerDelegate.index
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

        // Extension point: layer panel footer
        // - Blend mode selector
        // - Opacity slider for selected layer
        // - Layer actions (merge, flatten, etc.)
    }
}
