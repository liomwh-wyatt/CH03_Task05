# 천복랜드 사운드 작업내역서

작성일: 2026-06-28  
프로젝트: `C:\Users\liomw\Desktop\Unreal\CH03_Task05`  
기준 문서: `CheonbokLand_Sound_Plan.md`, `과제5_천복랜드_상세기획서.md`, `과제5_천복랜드_에셋제작명세서.md`, `과제5_천복랜드_무성컷신_연출명세_스토리보드.md`

## 1. 작업 목표

천복랜드의 P0 효과음만 넣고 끝내지 않고, 기존 사운드 계획서의 P1, BGM, 컷신 폴리까지 프로젝트에 임포트 가능한 상태로 채웠다. 외부 무료 에셋 다운로드는 이번 작업에서 사용하지 않았다. 모든 새 오디오는 절차적 합성으로 생성했으며, 매니페스트에는 `Project-owned generated asset`으로 기록했다.

## 2. 생성한 오디오 범위

총 36개 WAV를 생성하고 Unreal `SoundWave`로 임포트했다.

| 구분 | 개수 | 내용 |
|---|---:|---|
| P0 효과음 | 12 | UI, 웨이브 시작, 기본 획득, 회복, 스태미나, 피해, 폭탄, 감속, 반전, 게임오버 |
| P1 효과음 | 12 | 상태 종료, 실드, 이동 잠금, 콤보, 황금 날개 사료, 로봇청소기, 위험 경고, 게임 클리어 |
| BGM | 5 | 메인 메뉴, 공통 플레이, 거실, 주방, 꿈속 천복랜드 |
| 컷신 폴리/연출음 | 7 | 룸톤, 반짝임, 황금 생선 루프, 꿈 전환, 발바닥 전구, 간판 점등, 탈진 |

원본 WAV 위치:

```text
SourceAudio/Procedural/
```

원본 WAV와 검증용 JSON은 Unreal 자동 임포트 감지 대상이 되지 않도록 `Content` 폴더 밖에 둔다. 게임에서 참조하는 `SoundWave` 에셋만 `Content/Audio` 아래에 유지한다.

Unreal 임포트 위치:

```text
Content/Audio/UI/
Content/Audio/SFX/Items/
Content/Audio/SFX/Character/
Content/Audio/SFX/Environment/
Content/Audio/BGM/
Content/Audio/Cinematics/
```

## 3. 주요 에셋 목록

| 이름 | 용도 | Unreal 경로 |
|---|---|---|
| `S_UI_Hover` | 버튼 호버 | `/Game/Audio/UI` |
| `S_UI_Click` | 버튼 클릭 | `/Game/Audio/UI` |
| `S_WaveStart` | 웨이브 시작 | `/Game/Audio/UI` |
| `S_Pickup_SmallFeed` | 작은 사료 획득 | `/Game/Audio/SFX/Items` |
| `S_Pickup_LargeFeed` | 큰 사료 획득 | `/Game/Audio/SFX/Items` |
| `S_Heal` | 체력 회복 | `/Game/Audio/SFX/Character` |
| `S_StaminaRecover` | 스태미나 회복 | `/Game/Audio/SFX/Character` |
| `S_PlayerHit` | 피해 | `/Game/Audio/SFX/Character` |
| `S_ToyBomb` | 장난감 폭탄 | `/Game/Audio/SFX/Items` |
| `S_Debuff_Slow` | 감속 디버프 | `/Game/Audio/SFX/Character` |
| `S_Debuff_Reverse` | 조작 반전 | `/Game/Audio/SFX/Character` |
| `S_GameOver` | 게임 오버 | `/Game/Audio/UI` |
| `S_Debuff_End` | 상태효과 종료 | `/Game/Audio/SFX/Character` |
| `S_ShieldApply` | 보호 방울 획득 | `/Game/Audio/SFX/Character` |
| `S_ShieldBlock` | 실드 방어 | `/Game/Audio/SFX/Character` |
| `S_Debuff_Lock` | 엉킨 털실 이동 잠금 | `/Game/Audio/SFX/Character` |
| `S_ComboReward` | 우다다 보상 | `/Game/Audio/UI` |
| `S_ComboBreak` | 콤보 끊김 | `/Game/Audio/UI` |
| `S_GoldenWingSnack_Appear` | 황금 날개 사료 등장 | `/Game/Audio/SFX/Items` |
| `S_GoldenWingSnack_Pickup` | 황금 날개 사료 획득 | `/Game/Audio/SFX/Items` |
| `S_RobotVacuum_Warning` | 로봇청소기 활성화 | `/Game/Audio/SFX/Environment` |
| `S_RobotVacuum_Hit` | 로봇청소기 충돌 | `/Game/Audio/SFX/Environment` |
| `S_HazardWarning` | 위험 경고 | `/Game/Audio/SFX/Environment` |
| `S_GameClear` | 레벨/게임 클리어 | `/Game/Audio/UI` |
| `BGM_MainMenu` | 메인 메뉴 BGM | `/Game/Audio/BGM` |
| `BGM_Play_Common` | 공통 플레이 BGM | `/Game/Audio/BGM` |
| `BGM_LivingRoom` | 거실 BGM | `/Game/Audio/BGM` |
| `BGM_Kitchen` | 주방 BGM | `/Game/Audio/BGM` |
| `BGM_CheonbokLand` | 천복랜드 BGM | `/Game/Audio/BGM` |
| `S_CS_RoomTone_Night` | 오프닝 룸톤 | `/Game/Audio/Cinematics` |
| `S_CS_EarTwitch_Sparkle` | 귀 반응 반짝임 | `/Game/Audio/Cinematics` |
| `S_CS_GoldenFish_Loop` | 황금 생선 루프 | `/Game/Audio/Cinematics` |
| `S_CS_DreamTransition` | 꿈 전환 | `/Game/Audio/Cinematics` |
| `S_CS_PawBulb_On` | 발바닥 전구 점등 | `/Game/Audio/Cinematics` |
| `S_CS_Sign_LightUp` | 간판 점등 | `/Game/Audio/Cinematics` |
| `S_CS_Cheonbok_SoftCollapse` | 게임오버 탈진 | `/Game/Audio/Cinematics` |

## 4. 제작 도구와 기록 파일

추가/갱신한 주요 파일:

```text
Tools/GenerateCheonbokAudio.py
Tools/GenerateCheonbokP0Audio.py
Tools/ImportCheonbokAudioToUnreal.py
Tools/ValidateCheonbokAudio.py
Tools/VerifyCheonbokAudioInUnreal.py
Docs/CheonbokLand_Audio_Manifest.csv
Docs/CheonbokLand_Audio_Manifest.json
Docs/ImportedAudioManifest.json
Docs/AudioAssetVerification.json
SourceAudio/Procedural/
```

`GenerateCheonbokP0Audio.py`는 기존 이름을 유지하되, 새 전체 생성 스크립트 `GenerateCheonbokAudio.py`를 호출하도록 정리했다. 이후 P0만 따로 재생성하는 실수를 줄이기 위한 조치다.

## 5. Unreal 코드 연결

블루프린트 바이너리 파일을 직접 열어 저장하는 대신, C++ 기본값과 재생 경로를 추가했다. 기존 BP가 값을 명시적으로 덮어쓴 경우에는 BP 값이 우선될 수 있다.

### 아이템

수정 파일:

```text
Source/CH03_Task05/Private/Items/Ch03_SmallfeedItem.cpp
Source/CH03_Task05/Private/Items/Ch03_LargefeedItem.cpp
Source/CH03_Task05/Private/Items/Ch03_DamageItem.cpp
Source/CH03_Task05/Private/Items/Ch03_GoldenChuruItem.cpp
```

연결 내용:

- `ACh03_SmallfeedItem`: `S_Pickup_SmallFeed`
- `ACh03_LargefeedItem`: `S_Pickup_LargeFeed`
- `ACh03_DamageItem`: `S_ToyBomb`
- `ACh03_GoldenChuruItem`: `S_GoldenWingSnack_Pickup`

회복, 스태미나, 디버프 계열은 캐릭터 FeedbackCue에서 재생되므로 같은 이벤트에서 비슷한 소리가 중복되지 않게 아이템 `PickupSound` 기본값은 넣지 않았다.

### 캐릭터 FeedbackCue

수정 파일:

```text
Source/CH03_Task05/Private/Character/Ch03_CheonbokCharacter.cpp
```

연결 내용:

- `DamageTakenFeedback`: `S_PlayerHit`
- `DamageBlockedFeedback`: `S_ShieldBlock`
- `HealthRecoveredFeedback`: `S_Heal`
- `StaminaRecoveredFeedback`: `S_StaminaRecover`
- `SlowAppliedFeedback`: `S_Debuff_Slow`
- `ReverseControlAppliedFeedback`: `S_Debuff_Reverse`
- `MovementLockAppliedFeedback`: `S_Debuff_Lock`
- `DamageShieldAppliedFeedback`: `S_ShieldApply`
- `StatusEffectEndedFeedback`: `S_Debuff_End`

### 콤보

수정 파일:

```text
Source/CH03_Task05/Public/Core/Ch03_GameStateBase.h
Source/CH03_Task05/Private/Core/Ch03_GameStateBase.cpp
```

연결 내용:

- 콤보 보상 발생 시 `S_ComboReward`
- 콤보가 2 이상에서 끊길 때 `S_ComboBreak`

### 게임 흐름과 BGM

수정 파일:

```text
Source/CH03_Task05/Public/Core/Ch03_GameModeBase.h
Source/CH03_Task05/Private/Core/Ch03_GameModeBase.cpp
Source/CH03_Task05/Public/Core/Ch03_MainMenuGameMode.h
Source/CH03_Task05/Private/Core/Ch03_MainMenuGameMode.cpp
```

연결 내용:

- 웨이브 시작: `S_WaveStart`
- 게임오버: `S_GameOver`
- 레벨 완료: `S_GameClear`
- 황금 날개 사료 등장: `S_GoldenWingSnack_Appear`
- 메인 메뉴 BGM: `BGM_MainMenu`
- 플레이 레벨 BGM:
  - `L_LivingRoom`: `BGM_LivingRoom`
  - `L_Kitchen`: `BGM_Kitchen`
  - `L_CheonbokLand`: `BGM_CheonbokLand`
  - 그 외: `BGM_Play_Common`

추가로, 현재 프로젝트에 `BP_Item_GoldenChuru.uasset`은 삭제되어 있고 `BP_Item_GoldenFeed.uasset`가 존재하므로, `GoldenComboItemClass` 기본 참조를 `/Game/Blueprints/Items/BP_Item_GoldenFeed`로 맞췄다. 기존 사용자 변경분을 되돌리지 않고 현재 에셋명에 맞춘 조치다.

### 환경 장애물

수정 파일:

```text
Source/CH03_Task05/Private/Environment/Ch03_WaveEnvironmentActor.cpp
```

연결 내용:

- 활성화 피드백: `S_RobotVacuum_Warning`
- 충돌 피드백: `S_RobotVacuum_Hit`

### UI 버튼

수정 파일:

```text
Source/CH03_Task05/Public/UI/Ch03_MainMenuWidget.h
Source/CH03_Task05/Private/UI/Ch03_MainMenuWidget.cpp
Source/CH03_Task05/Public/UI/Ch03_PauseMenuWidget.h
Source/CH03_Task05/Private/UI/Ch03_PauseMenuWidget.cpp
Source/CH03_Task05/Public/UI/Ch03_GameResultWidget.h
Source/CH03_Task05/Private/UI/Ch03_GameResultWidget.cpp
```

연결 내용:

- 버튼 hover: `S_UI_Hover`
- 버튼 click: `S_UI_Click`

대상 위젯:

- `UCh03_MainMenuWidget`
- `UCh03_PauseMenuWidget`
- `UCh03_GameResultWidget`

## 6. 검증 결과

### WAV 포맷 검증

실행:

```text
Tools/ValidateCheonbokAudio.py
```

결과:

```text
checked=36 errors=0
```

확인 내용:

- 효과음/컷신 폴리: 48kHz, 16bit, mono
- BGM: 48kHz, 16bit, stereo
- 매니페스트의 채널 수, 샘플레이트, 비트 깊이와 실제 WAV 헤더 일치

### Unreal 임포트 검증

실행:

```text
Tools/ImportCheonbokAudioToUnreal.py
```

결과:

```text
Docs/ImportedAudioManifest.json 기준 36개 SoundWave 임포트 완료
UnrealEditor-Cmd 종료 코드 0
```

### Unreal 에셋 로딩 검증

실행:

```text
Tools/VerifyCheonbokAudioInUnreal.py
```

결과:

```text
checked_count=36
missing_count=0
UnrealEditor-Cmd: Success - 0 error(s), 0 warning(s)
```

### C++ 빌드 검증

실행:

```text
Build.bat CH03_Task05Editor Win64 Development -Project=CH03_Task05.uproject -WaitMutex -NoHotReload
```

결과:

```text
Result: Succeeded
```

처음 실행은 UnrealBuildTool 기본 로그 파일 잠금으로 시작 전에 실패했다. 이후 프로젝트 내부 별도 로그 파일을 지정해 재실행했고 컴파일과 링크가 성공했다.

## 7. 주의 사항

- `Content/Audio` 안에는 `.uasset`만 남겨야 한다. 원본 WAV, CSV, JSON을 넣으면 Unreal이 DataTable 또는 신규 임포트 대상으로 감지할 수 있다.
- 이번 작업은 실제 청취 테스트가 아니다. `UnrealEditor-Cmd`는 오디오 디바이스를 초기화하지 않으므로, PIE 또는 Standalone에서 체감 음량과 반복 피로도를 들어봐야 한다.
- 절차 생성 BGM은 루프 플래그를 켰지만, 청감상 루프 경계가 자연스러운지는 에디터에서 확인이 필요하다.
- 기존 작업 트리에는 BP, 맵, VFX 관련 변경분이 이미 있었다. 이번 작업에서는 그 변경분을 되돌리지 않았다.
- `BP_Item_GoldenChuru` 삭제와 `BP_Item_GoldenFeed` 추가는 기존 사용자 변경 상태로 보인다. 현재 코드는 `BP_Item_GoldenFeed` 기준으로 맞췄다.
- 외부 무료 에셋은 사용하지 않았다. 추후 Freesound, OpenGameArt, Pixabay 등에서 교체 소스를 받을 경우 개별 에셋 라이선스를 다시 확인해야 한다.

## 8. 다음 청취 체크리스트

- 작은 사료와 큰 사료가 연속 획득 시 구분되는지 확인한다.
- 회복, 스태미나, 실드가 모두 긍정 피드백으로 들리는지 확인한다.
- 폭탄, 피해, 로봇청소기 충돌이 과하게 무겁거나 공격적으로 들리지 않는지 확인한다.
- 감속, 반전, 이동 잠금이 소리만으로도 구분되는지 확인한다.
- BGM 볼륨이 HUD, 획득음, 위험 경고를 가리지 않는지 확인한다.
- UI hover/click이 메뉴 이동 중 과하게 반복되지 않는지 확인한다.
- 컷신 폴리는 Sequencer Audio Track에서 프레임 타이밍에 맞춰 배치한다.

## 9. 추가 작업: BGM 재제작

요청에 따라 기존 BGM이 주는 어두운 인상을 줄이고, 피아노 중심의 밝은 질감으로 5개 BGM을 다시 생성했다.

| 에셋 | 재제작 방향 |
|---|---|
| `BGM_MainMenu` | 밝은 아침 피아노 루프 |
| `BGM_Play_Common` | 플레이 공통용 경쾌한 햇살 느낌 피아노 |
| `BGM_LivingRoom` | 드뷔시 `달빛`에서 연상되는 부드러운 화성과 롤링 코드를 아침 햇살 쪽으로 조정한 피아노 |
| `BGM_Kitchen` | 기존보다 재미와 약한 긴장감을 넣은 주방용 피아노 |
| `BGM_CheonbokLand` | 몽환적이면서 신나는 꿈속 테마파크용 피아노와 반짝임 |

적용 내용:

- `Tools/GenerateCheonbokAudio.py`의 BGM 생성 로직을 피아노 음색 합성, 롤링 코드, 짧은 딜레이 기반으로 교체했다.
- `SourceAudio/Procedural/BGM_*.wav` 5개를 새로 생성했다.
- `/Game/Audio/BGM`의 5개 `SoundWave`를 재임포트했다.
- `Docs/CheonbokLand_Audio_Manifest.csv`, `Docs/CheonbokLand_Audio_Manifest.json`, `Docs/ImportedAudioManifest.json`, `Docs/AudioAssetVerification.json`을 갱신했다.

검증:

```text
Tools/ValidateCheonbokAudio.py
checked=36 errors=0

Tools/ImportCheonbokAudioToUnreal.py
UnrealEditor-Cmd: Success - 0 error(s), 0 warning(s)

Tools/VerifyCheonbokAudioInUnreal.py
checked_count=36
missing_count=0
UnrealEditor-Cmd: Success - 0 error(s), 0 warning(s)
```

## 10. 추가 작업: BGM별 기본 멜로디 분리

요청에 따라 5개 BGM이 같은 곡의 변형처럼 들리지 않도록, 각 곡의 상성 멜로디 모티프를 분리했다. 피아노 중심 톤은 유지하되 첫 구간에서 다른 노래로 인지되도록 멜로디 진행, 리듬 밀도, 화성 보조음을 다르게 구성했다.

| 에셋 | 새 기본 멜로디 방향 |
|---|---|
| `BGM_MainMenu` | 올라갔다 내려오는 안정적인 아침 인사형 8음 모티프 |
| `BGM_Play_Common` | 같은 음 반복과 도약이 있는 경쾌한 플레이 모티프 |
| `BGM_LivingRoom` | 느린 하행/상행이 섞인 부드러운 거실 모티프 |
| `BGM_Kitchen` | 반음 진행과 짧은 쉼표가 있는 주방 장난감 같은 모티프 |
| `BGM_CheonbokLand` | 3박 계열 상승 도약으로 꿈속 테마파크 느낌을 주는 모티프 |

적용 내용:

- `Tools/GenerateCheonbokAudio.py`에 `add_piano_melody` 헬퍼를 추가했다.
- `BGM_MainMenu`, `BGM_Play_Common`, `BGM_LivingRoom`은 같은 피아노 코드 질감 위에 각각 다른 모티프를 받도록 수정했다.
- `BGM_Kitchen`, `BGM_CheonbokLand`는 기존 패턴 위에 별도 멜로디 라인을 추가했다.
- `SourceAudio/Procedural/BGM_*.wav` 5개를 다시 생성했다.
- `/Game/Audio/BGM`의 5개 `SoundWave`를 재임포트했다.

검증:

```text
Tools/ValidateCheonbokAudio.py
checked=36 errors=0

Tools/ImportCheonbokAudioToUnreal.py
UnrealEditor-Cmd: Success - 0 error(s), 0 warning(s)

Tools/VerifyCheonbokAudioInUnreal.py
checked_count=36
missing_count=0
UnrealEditor-Cmd: Success - 0 error(s), 0 warning(s)
```

## 11. 추가 작업: BGM별 작곡 구조 재분리

이전 버전은 멜로디 모티프는 달랐지만 피아노 질감, 반주 방식, 코드 운용이 비슷해 같은 곡의 변형처럼 들릴 가능성이 있었다. 그래서 5개 BGM을 공통 함수 변형에서 분리하고, 각 곡을 별도 작곡 함수로 다시 구성했다.

| 에셋 | 새 구성 |
|---|---|
| `BGM_MainMenu` | 느린 4/4, 넓게 굴러가는 코드, 아침 인사형 긴 멜로디 |
| `BGM_Play_Common` | 빠른 4/4, 짧은 베이스 펄스와 오프비트 코드, 게임 플레이용 반복 리프 |
| `BGM_LivingRoom` | 느린 6박 계열, 넓은 롤링 코드, 하행 중심의 부드러운 선율 |
| `BGM_Kitchen` | 빠른 스타카토, 반음 진행, 짧은 쉼표와 장난스러운 리듬 |
| `BGM_CheonbokLand` | 3박 테마파크 왈츠, 움파파 반주, 상승 도약 테마와 반짝임 |

코드 변경:

- `add_main_menu_bgm`
- `add_common_play_bgm`
- `add_living_room_bgm`
- `add_kitchen_character_bgm`
- `add_cheonbokland_character_bgm`

검증:

```text
Tools/ValidateCheonbokAudio.py
checked=36 errors=0

Tools/ImportCheonbokAudioToUnreal.py
UnrealEditor-Cmd: Success - 0 error(s), 0 warning(s)

Tools/VerifyCheonbokAudioInUnreal.py
checked_count=36
missing_count=0
UnrealEditor-Cmd: Success - 0 error(s), 0 warning(s)
```

주의:

- Commandlet 검증은 파일, 포맷, 에셋 로딩 기준이다. 실제로 밝고 편안하게 들리는지는 PIE 또는 에디터 미리듣기에서 확인해야 한다.

## 12. 추가 작업: BGM 재생 볼륨 상향

배경음이 작게 들린다는 피드백에 따라 WAV 파일의 음압과 SFX/UI 효과음 밸런스는 유지하고, BGM 재생 계수만 상향했다. 메뉴와 인게임 배경음 모두 같은 계수를 사용하도록 맞췄다.

변경:

- `ACh03_GameModeBase::MusicVolumeMultiplier`: `0.34` -> `0.48`
- `ACh03_MainMenuGameMode::MusicVolumeMultiplier`: `0.34` -> `0.48`

검증:

```text
Build.bat CH03_Task05Editor Win64 Development -Project=CH03_Task05.uproject -WaitMutex -NoHotReload
Result: Succeeded
```

주의:

- 실제 체감 볼륨은 플레이어의 OS/에디터 출력 장치, Unreal 에디터 볼륨, PIE 환경에 따라 다르게 들릴 수 있으므로 최종 판단은 에디터 플레이에서 다시 확인해야 한다.
