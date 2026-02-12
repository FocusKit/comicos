import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Comicos 1.0
import "controls"

Rectangle {
    id: toolbar

    signal newDocumentRequested()
    signal openRequested()
    signal saveRequested()

    height: Theme.toolbarHeight
    color: Theme.bgSecondary

    // Platform-specific: macOS traffic light offset
    readonly property bool isMacOS: Qt.platform.os === "osx"

    // Bottom border
    Rectangle {
        anchors.bottom: parent.bottom
        width: parent.width
        height: 1
        color: Theme.borderColor
    }

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: isMacOS ? 80 : Theme.spacingLarge
        anchors.rightMargin: Theme.spacingLarge
        spacing: Theme.spacingMedium

        // --- File Actions ---
        RowLayout {
            spacing: Theme.spacingSmall

            IconButton {
                iconText: "+"
                tooltip: "새 문서"
                onClicked: toolbar.newDocumentRequested()
            }
            IconButton {
                iconText: "📂"
                tooltip: "열기 (Ctrl+O)"
                onClicked: toolbar.openRequested()
            }
            IconButton {
                iconText: "💾"
                tooltip: "저장 (Ctrl+S)"
                onClicked: toolbar.saveRequested()
            }
        }

        // Separator
        Rectangle { width: 1; height: 20; color: Theme.borderColor }

        // --- Undo / Redo ---
        RowLayout {
            spacing: Theme.spacingSmall

            IconButton {
                iconText: "↶"
                tooltip: "실행 취소"
                enabled: AppController.canUndo
                onClicked: AppController.undo()
            }
            IconButton {
                iconText: "↷"
                tooltip: "다시 실행"
                enabled: AppController.canRedo
                onClicked: AppController.redo()
            }
        }

        Rectangle { width: 1; height: 20; color: Theme.borderColor }

        // --- Tool Selection ---
        SegmentedControl {
            model: [
                { label: "펜", value: 0 },
                { label: "지우개", value: 1 },
                { label: "채우기", value: 2 }
            ]
            currentIndex: AppController.currentTool
            onSelected: (index) => { AppController.currentTool = index }
        }

        Rectangle { width: 1; height: 20; color: Theme.borderColor }

        // --- Color Picker ---
        Rectangle {
            width: 28
            height: 28
            radius: Theme.radiusMedium
            color: AppController.currentColor
            border.width: 1
            border.color: Theme.borderColor

            // Extension point: proper color picker dialog
            MouseArea {
                anchors.fill: parent
                // onClicked: colorDialog.open()
            }
        }

        // --- Brush Size ---
        RowLayout {
            spacing: Theme.spacingSmall

            Label {
                text: "크기"
                font.pixelSize: Theme.fontSizeLabel
                font.weight: Font.Medium
                color: Theme.textSecondary
            }

            Slider {
                id: brushSizeSlider
                from: 1
                to: 100
                value: AppController.brushSize
                onValueChanged: AppController.brushSize = value
                implicitWidth: 100

                background: Rectangle {
                    x: brushSizeSlider.leftPadding
                    y: brushSizeSlider.topPadding + brushSizeSlider.availableHeight / 2 - height / 2
                    width: brushSizeSlider.availableWidth
                    height: 4
                    radius: 2
                    color: Theme.bgTertiary

                    Rectangle {
                        width: brushSizeSlider.visualPosition * parent.width
                        height: parent.height
                        radius: 2
                        color: Theme.accent
                    }
                }

                handle: Rectangle {
                    x: brushSizeSlider.leftPadding + brushSizeSlider.visualPosition * (brushSizeSlider.availableWidth - width)
                    y: brushSizeSlider.topPadding + brushSizeSlider.availableHeight / 2 - height / 2
                    width: 14
                    height: 14
                    radius: 7
                    color: Theme.bgElevated
                    border.width: 2
                    border.color: Theme.accent
                }
            }

            Label {
                text: Math.round(AppController.brushSize) + "px"
                font.pixelSize: Theme.fontSizeLabel
                color: Theme.textSecondary
                Layout.preferredWidth: 36
            }
        }

        // Spacer
        Item { Layout.fillWidth: true }

        // --- Theme Toggle ---
        IconButton {
            iconText: AppController.isDarkTheme ? "☀" : "☾"
            tooltip: "테마 전환"
            onClicked: {
                AppController.theme = AppController.isDarkTheme ? "light" : "dark"
            }
        }
    }
}
