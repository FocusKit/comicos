import React, { useState, useMemo, useCallback } from 'react'

interface CanvasSizeModalProps {
  onConfirm: (width: number, height: number) => void
}

interface PaperPreset {
  label: string
  widthMm: number
  heightMm: number
}

const PRESETS: PaperPreset[] = [
  { label: 'B4', widthMm: 257, heightMm: 364 },
  { label: 'B5', widthMm: 182, heightMm: 257 },
  { label: 'B6', widthMm: 128, heightMm: 182 },
  { label: 'A4', widthMm: 210, heightMm: 297 },
  { label: 'A5', widthMm: 148, heightMm: 210 },
  { label: 'A6', widthMm: 105, heightMm: 148 }
]

const DPI_OPTIONS = [72, 144, 300, 350, 600]

const MIN_PX = 1
const MAX_PX_W = 10000
const MAX_PX_H = 10000

function mmToPx(mm: number, dpi: number): number {
  return Math.round((mm / 25.4) * dpi)
}

function clamp(value: number, min: number, max: number): number {
  return Math.max(min, Math.min(max, value))
}

const CanvasSizeModal: React.FC<CanvasSizeModalProps> = ({ onConfirm }) => {
  const [selectedPreset, setSelectedPreset] = useState<string>('B5')
  const [dpi, setDpi] = useState(300)
  const [customWidth, setCustomWidth] = useState(1200)
  const [customHeight, setCustomHeight] = useState(800)

  const isCustom = selectedPreset === 'custom'

  const pixelSize = useMemo(() => {
    if (isCustom) {
      return {
        width: clamp(customWidth, MIN_PX, MAX_PX_W),
        height: clamp(customHeight, MIN_PX, MAX_PX_H)
      }
    }
    const preset = PRESETS.find((p) => p.label === selectedPreset)
    if (!preset) return { width: 1200, height: 800 }
    return {
      width: clamp(mmToPx(preset.widthMm, dpi), MIN_PX, MAX_PX_W),
      height: clamp(mmToPx(preset.heightMm, dpi), MIN_PX, MAX_PX_H)
    }
  }, [selectedPreset, dpi, customWidth, customHeight, isCustom])

  const handlePresetChange = useCallback((e: React.ChangeEvent<HTMLSelectElement>) => {
    setSelectedPreset(e.target.value)
  }, [])

  const handleDpiChange = useCallback((e: React.ChangeEvent<HTMLSelectElement>) => {
    setDpi(Number(e.target.value))
  }, [])

  const handleSubmit = useCallback(
    (e: React.FormEvent) => {
      e.preventDefault()
      onConfirm(pixelSize.width, pixelSize.height)
    },
    [onConfirm, pixelSize]
  )

  return (
    <div className="modal-overlay">
      <div className="modal-card">
        <h2 className="modal-title">새 캔버스</h2>
        <form onSubmit={handleSubmit}>
          <div className="modal-form-group">
            <label className="modal-form-label">용지 크기</label>
            <select
              className="modal-form-select"
              value={selectedPreset}
              onChange={handlePresetChange}
            >
              {PRESETS.map((p) => (
                <option key={p.label} value={p.label}>
                  {p.label} ({p.widthMm}×{p.heightMm}mm)
                </option>
              ))}
              <option value="custom">사용자 지정</option>
            </select>
          </div>

          {!isCustom && (
            <div className="modal-form-group">
              <label className="modal-form-label">DPI</label>
              <select
                className="modal-form-select"
                value={dpi}
                onChange={handleDpiChange}
              >
                {DPI_OPTIONS.map((d) => (
                  <option key={d} value={d}>
                    {d} DPI
                  </option>
                ))}
              </select>
            </div>
          )}

          {isCustom && (
            <div className="modal-form-row">
              <div className="modal-form-group">
                <label className="modal-form-label">너비 (px)</label>
                <input
                  type="number"
                  className="modal-form-input"
                  value={customWidth}
                  min={MIN_PX}
                  max={MAX_PX_W}
                  onChange={(e) => setCustomWidth(Number(e.target.value) || MIN_PX)}
                />
              </div>
              <span className="modal-form-x">×</span>
              <div className="modal-form-group">
                <label className="modal-form-label">높이 (px)</label>
                <input
                  type="number"
                  className="modal-form-input"
                  value={customHeight}
                  min={MIN_PX}
                  max={MAX_PX_H}
                  onChange={(e) => setCustomHeight(Number(e.target.value) || MIN_PX)}
                />
              </div>
            </div>
          )}

          <div className="modal-form-info">
            {pixelSize.width} × {pixelSize.height} px
          </div>

          <button type="submit" className="modal-btn-primary">
            만들기
          </button>
        </form>
      </div>
    </div>
  )
}

export default CanvasSizeModal
