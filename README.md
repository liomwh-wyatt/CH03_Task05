# 천복랜드 (Cheonbok Land)

스파르타 내일배움캠프 Unreal CH3 개인 과제 5 프로젝트입니다.

반려묘 천복이를 모델로 한 2등신 SD 고양이가 간식을 모으고 위험 요소를 피하며 3개 레벨의 멀티 웨이브를 통과하는 3D 수집형 서바이벌 아케이드 게임을 제작합니다.

## 개발 환경

- Unreal Engine 5.8
- C++
- JetBrains Rider
- Blueprint / UMG
- Git / Git LFS

## 과제 핵심 목표

- 3개 플레이 레벨
- 각 레벨에 3개 웨이브
- 웨이브별 제한 시간과 아이템 스폰 변화
- 점수, 남은 시간, 체력을 표시하는 HUD
- 메인 메뉴와 게임 오버 메뉴
- 슬로우 및 조작 반전 디버프
- 웨이브별 환경 변화와 UI 알림

## 현재 진행 상태

- [x] Unreal Engine 5.8 C++ 프로젝트 생성
- [x] Git 저장소 및 원격 저장소 연결
- [x] Git ignore와 Git LFS 기본 설정
- [x] 천복이 Skeletal Mesh 및 애니메이션 임포트
- [x] 플레이어 캐릭터와 Enhanced Input
- [x] 아이템 인터페이스와 Spawn Volume
- [x] 멀티 웨이브 시스템
- [x] HUD, 메인 메뉴, 결과 메뉴
- [x] 최고 점수 SaveGame
- [x] 일시정지 메뉴
- [x] 슬로우 및 조작 반전 디버프 수직 슬라이스
- [x] 웨이브별 환경 변화 Actor 연동 구조
- [ ] 웨이브별 장애물/비주얼 폴리싱
- [x] 웨이브 배너 UI 애니메이션
- [ ] 3D 위젯
- [ ] 최종 테스트 및 시연 영상

## 조작법

- `WASD`: 이동
- `Mouse`: 시점 조작
- `Space`: 점프
- `Shift`: 지상 스프린트
- `Esc` / `P`: 일시정지 메뉴 열기/닫기
- `Shift + Esc`: 에디터 PIE 프리뷰 종료

## 주요 구현 포인트

- `GameMode` 기반 3개 웨이브 진행 및 레벨 전환
- `GameState` 델리게이트 기반 HUD 실시간 갱신
- 웨이브 시작/클리어/레벨 완료 안내 배너 표시
- `GameInstance` 기반 레벨 진행도 및 최고 점수 저장
- `UUserWidget` C++ 부모 클래스를 통한 메인 메뉴, 결과 메뉴, 일시정지 메뉴 제어
- 웨이브 안내 배너의 Fade/Scale UI 애니메이션
- 슬로우/조작 반전 디버프의 지속 시간, 중첩 수, HUD 표시
- `ACh03_WaveEnvironmentActor` 기반 웨이브별 장애물 활성/비활성, 왕복 이동, 별도 안내 배너 구조

## 프로젝트 구조

```text
CH03_Task05/
├─ Config/                 Unreal 프로젝트 설정
├─ Content/                맵, 블루프린트, UI 및 게임 에셋
├─ Source/CH03_Task05/     C++ 소스 코드
├─ CH03_Task05.uproject
├─ .gitattributes          Git LFS 대상 파일
└─ .gitignore              생성 파일 및 IDE 파일 제외
```

## 저장소 사용 방법

Git LFS가 설치되어 있어야 Unreal 바이너리 에셋을 정상적으로 받을 수 있습니다.

```powershell
git lfs install
git clone https://github.com/liomwh-wyatt/CH03_Task05.git
cd CH03_Task05
git lfs pull
```

프로젝트는 `CH03_Task05.uproject`를 Rider 또는 Unreal Editor에서 직접 엽니다. 에디터 실행 전 Rider의 빌드 구성이 `Development Editor | Win64`인지 확인합니다.

## Git 관리 원칙

- `Source`, `Config`, `Content`, `.uproject`는 저장소에 포함합니다.
- `Binaries`, `Intermediate`, `Saved`, `DerivedDataCache`는 저장소에 포함하지 않습니다.
- `.uasset`, `.umap`, `.fbx`와 대용량 원본 에셋은 Git LFS로 관리합니다.
- 리플렉션 타입이나 C++ 클래스 구조를 변경한 경우 Live Coding에만 의존하지 않고 에디터를 종료한 뒤 전체 빌드합니다.

## 기획 문서

상세 기획서는 노트 저장소의 `CH3/개인과제/과제5_천복랜드_상세기획서.md`에서 관리합니다.
