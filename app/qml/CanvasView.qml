import QtQuick
import QtQuick.Controls
import Comicos 1.0

/// Main canvas area with the CanvasItem and background.
Item {
    id: canvasView
    clip: true

    // Canvas background (gray area outside the paper)
    Rectangle {
        anchors.fill: parent
        color: Theme.canvasBg
    }

    // Canvas rendering item (C++ QQuickItem)
    // Paper background and tile rendering are handled in C++ SceneGraph
    CanvasItem {
        id: canvas
        anchors.fill: parent
        focus: true

        // Connect stroke signals to AppController
        onStrokeStarted: (pos, pressure) => {
            AppController.onStrokeStarted(pos, pressure)
        }
        onStrokeUpdated: (pos, pressure) => {
            AppController.onStrokeUpdated(pos, pressure)
        }
        onStrokeEnded: {
            AppController.onStrokeEnded()
        }

        Component.onCompleted: {
            AppController.setCanvasItem(canvas)
            canvas.forceActiveFocus()
        }
    }

    // Bridge: repaint canvas when tile data changes
    Connections {
        target: AppController
        function onCanvasNeedsUpdate() { canvas.update() }
    }

    // Zoom indicator
    Rectangle {
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: Theme.spacingLarge
        width: zoomLabel.implicitWidth + Theme.spacingLarge * 2
        height: 28
        radius: Theme.radiusMedium
        color: Theme.bgElevated
        border.width: 1
        border.color: Theme.borderColor
        opacity: 0.9

        Label {
            id: zoomLabel
            anchors.centerIn: parent
            text: Math.round(canvas.zoom * 100) + "%"
            font.pixelSize: Theme.fontSizeSmall
            color: Theme.textSecondary
        }
    }

    // Extension point: canvas overlays
    // - Brush cursor preview
    // - Selection marquee
    // - Transform handles
    // - Grid overlay
    // - Ruler
}
