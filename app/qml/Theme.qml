pragma Singleton
import QtQuick

/// Design token singleton matching the textos project theme.
/// All colors and sizes are defined here for consistency.
QtObject {
    id: theme

    // Bound to AppController.isDarkTheme
    property bool dark: false

    // --- Font ---
    readonly property string fontFamily: "Pretendard Variable, -apple-system, BlinkMacSystemFont, Segoe UI, sans-serif"
    readonly property string fontMono: "Consolas, Courier New, monospace"

    // --- Colors: Background ---
    readonly property color bgPrimary:   dark ? "#17171c" : "#ffffff"
    readonly property color bgSecondary: dark ? "#1e1e24" : "#f5f6f8"
    readonly property color bgTertiary:  dark ? "#2c2c35" : "#eceef1"
    readonly property color bgElevated:  dark ? "#25252d" : "#ffffff"

    // --- Colors: Text ---
    readonly property color textPrimary:   dark ? "#ececf1" : "#191f28"
    readonly property color textSecondary: dark ? "#8b8fa3" : "#6b7684"
    readonly property color textTertiary:  dark ? "#6b6e82" : "#8b95a1"

    // --- Colors: Accent ---
    readonly property color accent:      dark ? "#8B7CF6" : "#6C5CE7"
    readonly property color accentHover: dark ? "#A294FF" : "#5A4BD1"
    readonly property color accentLight: dark ? Qt.rgba(0.545, 0.486, 0.965, 0.1)
                                              : Qt.rgba(0.424, 0.361, 0.906, 0.08)

    // --- Colors: Border ---
    readonly property color borderColor: dark ? "#2e2e38" : "#e5e8eb"

    // --- Colors: Danger ---
    readonly property color danger:      dark ? "#f04452" : "#f04452"
    readonly property color dangerHover: dark ? "#ff6370" : "#d92130"

    // --- Colors: Canvas ---
    readonly property color canvasBg:    dark ? "#111115" : "#e8e8e8"

    // --- Colors: Overlay ---
    readonly property color overlay: dark ? Qt.rgba(0, 0, 0, 0.6) : Qt.rgba(0, 0, 0, 0.4)

    // --- Shadows ---
    readonly property color shadowColor: dark ? Qt.rgba(0, 0, 0, 0.3) : Qt.rgba(0, 0, 0, 0.08)

    // --- Sizes ---
    readonly property int toolbarHeight: 44
    readonly property int statusBarHeight: 28
    readonly property int layerPanelWidth: 240

    readonly property int radiusSmall: 4
    readonly property int radiusMedium: 8
    readonly property int radiusLarge: 12
    readonly property int radiusXLarge: 16

    readonly property int spacingTiny: 2
    readonly property int spacingSmall: 4
    readonly property int spacingMedium: 8
    readonly property int spacingLarge: 16
    readonly property int spacingXLarge: 24

    // --- Font Sizes ---
    readonly property int fontSizeSmall: 12
    readonly property int fontSizeBody: 14
    readonly property int fontSizeLabel: 13
    readonly property int fontSizeTitle: 18
    readonly property int fontSizePageTitle: 22

    // --- Animation ---
    readonly property int animFast: 150
    readonly property int animNormal: 200
}
