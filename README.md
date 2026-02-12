# Comicos

고성능 만화/드로잉 앱. C++ / Qt 6 기반.

## 아키텍처

```
comicos/
├── app/                    # 앱 진입점 + QML UI
│   ├── main.cpp            # Qt 앱 초기화, QML 엔진 설정
│   └── qml/
│       ├── Main.qml        # 메인 윈도우 레이아웃
│       ├── Theme.qml       # 디자인 토큰 (컬러, 폰트, 사이즈)
│       ├── Toolbar.qml     # 상단 툴바 (도구, 브러시, 색상)
│       ├── CanvasView.qml  # 캔버스 영역 (C++ CanvasItem 래핑)
│       ├── LayerPanel.qml  # 레이어 패널 (목록, 추가/삭제)
│       ├── StatusBar.qml   # 하단 상태 바
│       └── controls/       # 재사용 QML 컨트롤
│           ├── IconButton.qml
│           └── SegmentedControl.qml
│
├── core/                   # 핵심 데이터 구조 (순수 C++, Qt 종속성 최소)
│   ├── Types.h             # Pixel, TileCoord, BlendMode, ToolType
│   ├── Tile.h/cpp          # 256×256 RGBA8 타일 (지연 할당)
│   ├── TileManager.h/cpp   # 희소 타일 그리드 (레이어별 하나)
│   ├── Layer.h/cpp         # 단일 레이어 (TileManager 소유)
│   ├── LayerStack.h/cpp    # 레이어 스택 (추가/삭제/이동/복제)
│   ├── Stroke.h/cpp        # 브러시 스트로크 입력 데이터
│   ├── History.h/cpp       # 실행 취소/다시 실행 (커맨드 패턴, 메모리 제한)
│   └── Document.h/cpp      # 최상위 문서 모델 (레이어 + 히스토리 + 메타)
│
├── engine/                 # 브러시 엔진 + 합성 파이프라인
│   ├── BrushDab.h/cpp      # 단일 브러시 dab + dab 배치 알고리즘
│   ├── BrushEngine.h/cpp   # 스트로크→타일 렌더링 (핵심 성능 경로)
│   ├── TileCache.h/cpp     # GPU 타일 텍스처 캐시 (LRU)
│   └── Compositor.h/cpp    # 레이어 합성 (블렌드 모드, 알파 합성)
│
├── render/                 # Qt RHI 기반 렌더링 추상화
│   ├── RenderBackend.h/cpp     # GPU 백엔드 추상화 (D3D12/Metal/Vulkan)
│   ├── CanvasRenderer.h/cpp    # 뷰 변환 (팬/줌/회전) + 가시 타일 결정
│   ├── TileRenderer.h/cpp      # SceneGraph 노드 관리
│   └── CanvasItem.h/cpp        # QQuickItem (QML↔렌더링 브릿지, 입력 처리)
│
├── bridge/                 # QML ↔ C++ 바인딩
│   ├── AppController.h/cpp     # 앱 전역 상태 (도구, 색상, 테마, 액션)
│   └── DocumentModel.h/cpp     # 레이어 리스트 모델 (QAbstractListModel)
│
├── shaders/                # GPU 셰이더 (GLSL 440 → Qt Shader Tools)
│   ├── canvas.vert         # 타일 쿼드 변환
│   ├── canvas.frag         # 타일 텍스처 샘플링
│   ├── brush.frag          # 브러시 dab 렌더링 (하드니스 폴오프)
│   └── composite.frag      # 레이어 합성 (Normal/Multiply/Screen/Overlay)
│
└── CMakeLists.txt          # 루트 CMake (모든 서브모듈 통합)
```

## 모듈 역할

| 모듈 | 역할 | 의존성 |
|------|------|--------|
| **core** | 순수 데이터 구조. UI/렌더링 무관. | Qt Core, Gui |
| **engine** | 픽셀 연산. 브러시→타일 렌더링, 합성. | core |
| **render** | GPU 추상화. SceneGraph/RHI로 화면 출력. | core, engine, Qt Quick |
| **bridge** | QML 바인딩. QObject로 UI에 C++ 노출. | core, engine, render |
| **shaders** | GPU 셰이더. 크로스 플랫폼 단일 소스. | Qt Shader Tools |
| **app** | 진입점 + QML UI. | 전체 |

## 핵심 설계 결정

### 타일 기반 캔버스
- **256×256 RGBA8** 타일 단위로 픽셀 관리
- **희소 저장**: 빈 영역은 메모리 미할당
- **지연 할당**: 실제 그리기 시에만 타일 생성
- 대형 캔버스(10000×10000+)에서도 메모리 효율적

### 렌더링 파이프라인
```
입력 → BrushEngine → Tile 수정 → TileCache(GPU 업로드)
                                        ↓
                                  Compositor → TileRenderer → SceneGraph → 화면
```

### 히스토리 (실행 취소)
- **커맨드 패턴**: 각 액션이 undo/redo 방법을 알고 있음
- **메모리 제한**: 기본 256MB, 초과 시 오래된 항목 삭제
- **타일 스냅샷**: 변경된 타일만 복사 (전체 캔버스 X)

### 플랫폼 분기
- **Windows**: D3D12 (RHI), WinTab/WM_POINTER 태블릿
- **macOS**: Metal (RHI), NSEvent 태블릿
- **iOS/iPad**: Metal (RHI), Apple Pencil (UITouch)

## 빌드

### 요구사항
- CMake 3.21+
- Qt 6.5+ (Quick, QuickControls2, ShaderTools, Svg)
- C++20 컴파일러

### Windows (MSVC)
```bash
# Qt 경로를 CMAKE_PREFIX_PATH에 설정
cmake -B build -G "Visual Studio 17 2022" -DCMAKE_PREFIX_PATH="C:/Qt/6.7.0/msvc2022_64"
cmake --build build --config Release
```

### macOS
```bash
cmake -B build -DCMAKE_PREFIX_PATH="~/Qt/6.7.0/macos"
cmake --build build --config Release
```

### iOS (Xcode)
```bash
cmake -B build-ios -G Xcode \
  -DCMAKE_SYSTEM_NAME=iOS \
  -DCMAKE_PREFIX_PATH="~/Qt/6.7.0/ios"
cmake --build build-ios --config Release
```

## 확장 포인트

코드 전체에 `Extension point:` 주석으로 향후 기능 삽입 지점이 표시되어 있습니다:

- **브러시 텍스처**: `BrushEngine::renderDab` / `brush.frag`
- **블렌드 모드**: `Compositor::blendPixels` / `composite.frag`
- **GPU 렌더링**: `RenderBackend` / `TileRenderer::updateSceneGraph`
- **파일 저장**: `Document::save/load`
- **레이어 그룹**: `LayerStack::createGroup`
- **선택 도구**: `ToolType::Select`
- **태블릿 입력**: `CanvasItem::tabletEvent`

## 테마

textos 프로젝트와 동일한 디자인 시스템:
- **라이트**: 흰색 배경, 보라 액센트 `#6C5CE7`
- **다크**: `#17171c` 배경, 보라 액센트 `#8B7CF6`
- **폰트**: Pretendard Variable
- 시스템 테마 자동 감지 지원
