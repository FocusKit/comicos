import QtQuick
import QtQuick.Controls
import Comicos 1.0

/// Minimal icon button matching the textos design language.
AbstractButton {
    id: control

    property string iconText: ""
    property string tooltip: ""

    implicitWidth: 32
    implicitHeight: 32

    ToolTip.visible: tooltip && hovered
    ToolTip.text: tooltip
    ToolTip.delay: 500

    background: Rectangle {
        radius: Theme.radiusMedium
        color: {
            if (!control.enabled) return "transparent"
            if (control.pressed) return Theme.accentLight
            if (control.hovered) return Theme.bgTertiary
            return "transparent"
        }

        Behavior on color { ColorAnimation { duration: Theme.animFast } }
    }

    contentItem: Text {
        text: control.iconText
        font.pixelSize: 16
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        color: control.enabled ? Theme.textPrimary : Theme.textTertiary
    }

    opacity: enabled ? 1.0 : 0.5
}
