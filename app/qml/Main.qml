import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import Comicos 1.0
import "controls"

ApplicationWindow {
    id: root

    width: 1280
    height: 800
    minimumWidth: 900
    minimumHeight: 600
    visible: true
    title: {
        let name = "Comicos"
        if (AppController.filePath)
            name = AppController.filePath.split("/").pop() + " — Comicos"
        if (AppController.isDirty)
            name = "● " + name
        return name
    }

    color: Theme.bgPrimary

    onClosing: (close) => {
        if (AppController.isDirty) {
            close.accepted = false
            root._pendingAction = "quit"
            unsavedModal.visible = true
        }
    }

    // Bind theme
    Component.onCompleted: {
        Theme.dark = Qt.binding(() => AppController.isDarkTheme)
    }

    // --- Keyboard Shortcuts ---
    Shortcut { sequence: "B"; onActivated: AppController.currentTool = 0 }  // Pen
    Shortcut { sequence: "P"; onActivated: AppController.currentTool = 0 }  // Pen
    Shortcut { sequence: "E"; onActivated: AppController.currentTool = 1 }  // Eraser
    Shortcut { sequence: "G"; onActivated: AppController.currentTool = 2 }  // Fill

    Shortcut {
        sequence: StandardKey.Undo
        onActivated: AppController.undo()
    }
    Shortcut {
        sequence: StandardKey.Redo
        onActivated: AppController.redo()
    }
    Shortcut {
        sequence: "Ctrl+Shift+Z"
        onActivated: AppController.redo()
    }
    Shortcut {
        sequence: "["
        onActivated: AppController.brushSize = Math.max(1, AppController.brushSize - 1)
    }
    Shortcut {
        sequence: "]"
        onActivated: AppController.brushSize = Math.min(500, AppController.brushSize + 1)
    }

    // --- File Shortcuts ---
    Shortcut {
        sequence: StandardKey.Save
        onActivated: root.handleSave()
    }
    Shortcut {
        sequence: "Ctrl+Shift+S"
        onActivated: saveDialog.open()
    }
    Shortcut {
        sequence: StandardKey.Open
        onActivated: root.handleOpen()
    }

    // --- File Dialogs ---
    property string _pendingAction: ""  // "new" or "open"

    FileDialog {
        id: saveDialog
        title: "문서 저장"
        fileMode: FileDialog.SaveFile
        nameFilters: ["Comicos 파일 (*.cmc)"]
        defaultSuffix: "cmc"
        onAccepted: {
            let path = selectedFile.toString().replace("file:///", "")
            if (AppController.saveDocumentTo(path)) {
                root.executePendingAction()
            }
        }
        onRejected: {
            root._pendingAction = ""
        }
    }

    FileDialog {
        id: openDialog
        title: "문서 열기"
        fileMode: FileDialog.OpenFile
        nameFilters: ["Comicos 파일 (*.cmc)"]
        onAccepted: {
            let path = selectedFile.toString().replace("file:///", "")
            AppController.openDocument(path)
        }
    }

    // --- File Actions ---
    function handleSave() {
        if (AppController.filePath) {
            AppController.saveDocument()
        } else {
            saveDialog.open()
        }
    }

    function handleOpen() {
        if (AppController.isDirty) {
            root._pendingAction = "open"
            unsavedModal.visible = true
        } else {
            openDialog.open()
        }
    }

    // --- New Document Flow ---
    function handleNewDocument() {
        if (AppController.isDirty) {
            root._pendingAction = "new"
            unsavedModal.visible = true
        } else {
            AppController.newDocument(2480, 3508)
        }
    }

    // --- Layout ---
    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // Toolbar
        Toolbar {
            Layout.fillWidth: true
            onNewDocumentRequested: root.handleNewDocument()
            onOpenRequested: root.handleOpen()
            onSaveRequested: root.handleSave()
        }

        // Main content area
        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 0

            // Canvas
            CanvasView {
                Layout.fillWidth: true
                Layout.fillHeight: true
            }

            // Separator
            Rectangle {
                Layout.fillHeight: true
                width: 1
                color: Theme.borderColor
            }

            // Layer Panel
            LayerPanel {
                Layout.fillHeight: true
                Layout.preferredWidth: Theme.layerPanelWidth
            }
        }

        // Status Bar
        StatusBar {
            Layout.fillWidth: true
        }
    }

    // --- Unsaved Changes Modal ---
    Modal {
        id: unsavedModal
        title: "저장되지 않은 변경 사항"
        message: "저장되지 않은 변경 사항이 있습니다. 계속하기 전에 저장하시겠습니까?"
        actions: [
            {
                label: "저장",
                variant: "primary",
                action: () => {
                    unsavedModal.visible = false
                    if (AppController.filePath) {
                        AppController.saveDocument()
                        root.executePendingAction()
                    } else {
                        saveDialog.open()
                    }
                }
            },
            {
                label: "저장 안 함",
                variant: "danger",
                action: () => {
                    unsavedModal.visible = false
                    root.executePendingAction()
                }
            },
            {
                label: "취소",
                variant: "secondary",
                action: () => {
                    unsavedModal.visible = false
                    root._pendingAction = ""
                }
            }
        ]
        onClosed: {
            visible = false
            root._pendingAction = ""
        }
    }

    function executePendingAction() {
        let action = root._pendingAction
        root._pendingAction = ""
        if (action === "new") {
            AppController.newDocument(2480, 3508)
        } else if (action === "open") {
            openDialog.open()
        } else if (action === "quit") {
            Qt.quit()
        }
    }
}
