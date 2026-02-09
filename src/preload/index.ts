import { contextBridge, ipcRenderer } from 'electron'

const api = {
  saveProject: (data: string, filePath?: string) =>
    ipcRenderer.invoke('file:save-project', data, filePath),
  saveProjectAs: (data: string) => ipcRenderer.invoke('file:save-project-as', data),
  openProject: () => ipcRenderer.invoke('file:open-project'),
  exportImage: (dataUrl: string) => ipcRenderer.invoke('file:export-image', dataUrl),
  getZoomFactor: () => ipcRenderer.invoke('zoom:get'),
  setZoomFactor: (factor: number) => ipcRenderer.invoke('zoom:set', factor),
  onMenuAction: (callback: (action: string) => void) => {
    const handler = (_event: Electron.IpcRendererEvent, action: string) => callback(action)
    ipcRenderer.on('menu:action', handler)
    return () => ipcRenderer.removeListener('menu:action', handler)
  }
}

contextBridge.exposeInMainWorld('api', api)

export type ElectronAPI = typeof api
