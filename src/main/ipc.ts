import { ipcMain, dialog, BrowserWindow } from 'electron'
import { readFile, writeFile } from 'fs/promises'

export function registerIpcHandlers(): void {
  ipcMain.handle(
    'file:save-project',
    async (_event, data: string, filePath?: string) => {
      if (filePath) {
        await writeFile(filePath, data, 'utf-8')
        return { success: true, filePath }
      }

      const win = BrowserWindow.getFocusedWindow()
      if (!win) return { success: false }

      const { canceled, filePath: chosen } = await dialog.showSaveDialog(win, {
        title: '프로젝트 저장',
        defaultPath: '제목 없음.cmc',
        filters: [{ name: 'Comicos 프로젝트', extensions: ['cmc'] }]
      })

      if (canceled || !chosen) return { success: false }

      await writeFile(chosen, data, 'utf-8')
      return { success: true, filePath: chosen }
    }
  )

  ipcMain.handle('file:save-project-as', async (_event, data: string) => {
    const win = BrowserWindow.getFocusedWindow()
    if (!win) return { success: false }

    const { canceled, filePath } = await dialog.showSaveDialog(win, {
      title: '다른 이름으로 저장',
      defaultPath: '제목 없음.cmc',
      filters: [{ name: 'Comicos 프로젝트', extensions: ['cmc'] }]
    })

    if (canceled || !filePath) return { success: false }

    await writeFile(filePath, data, 'utf-8')
    return { success: true, filePath }
  })

  ipcMain.handle('file:open-project', async () => {
    const win = BrowserWindow.getFocusedWindow()
    if (!win) return { success: false }

    const { canceled, filePaths } = await dialog.showOpenDialog(win, {
      title: '프로젝트 열기',
      filters: [{ name: 'Comicos 프로젝트', extensions: ['cmc'] }],
      properties: ['openFile']
    })

    if (canceled || filePaths.length === 0) return { success: false }

    const data = await readFile(filePaths[0], 'utf-8')
    return { success: true, data, filePath: filePaths[0] }
  })

  ipcMain.handle('file:export-image', async (_event, dataUrl: string) => {
    const win = BrowserWindow.getFocusedWindow()
    if (!win) return { success: false }

    const { canceled, filePath } = await dialog.showSaveDialog(win, {
      title: '이미지 내보내기',
      defaultPath: 'untitled.png',
      filters: [
        { name: 'PNG 이미지', extensions: ['png'] },
        { name: 'JPEG 이미지', extensions: ['jpg', 'jpeg'] }
      ]
    })

    if (canceled || !filePath) return { success: false }

    const base64Data = dataUrl.replace(/^data:image\/\w+;base64,/, '')
    const buffer = Buffer.from(base64Data, 'base64')
    await writeFile(filePath, buffer)
    return { success: true, filePath }
  })

  ipcMain.handle('zoom:get', (event) => {
    return event.sender.getZoomFactor()
  })

  ipcMain.handle('zoom:set', (event, factor: number) => {
    event.sender.setZoomFactor(factor)
  })
}
