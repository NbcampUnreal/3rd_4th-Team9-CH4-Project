# OverClock

> 오버워치 풍의 팀 기반 FPS 게임 프로젝트

---

## 🌌 세계관

### 시대
- 인류가 은하계를 넘어 수많은 행성에 진출한 미래.
- 각 행성마다 독자적인 문명과 종족이 발전하였다.
  - 크런치 : 강철의 기계 종족
  - 트윈 블라스트 : 맹렬한 연사 속사수
  - 더 페이 : 자연의 정령 치유사


### 배경
- 초월적 존재 혹은 초거대 연합체가 **행성 존망을 건 전쟁 의식(챔피언십)**을 개최
- 각 행성 대표 영웅이 **다차원 전장**으로 소환
- 전장은 “중립적인 아레나” 혹은 무너진 세계의 잔해로, 전투에 최적화된 공간

### 상황
- 승리한 영웅의 행성만 존속, 패배한 행성은 소멸 또는 동화
- 참가자들은 **개인 영광이 아닌 문명과 종족의 생존**을 위해 싸움
- 동시에, 전쟁은 은하 전역에서 중계되는 ‘최강의 전쟁 게임’처럼 소비됨 (e스포츠 + 종족 전쟁 느낌)

---

## 🎮 게임 개요

- 팀 기반 FPS (3 vs 3)
- 역할 : 탱커, 딜러, 힐러
- 각 영웅은 고유 스킬을 가짐
- 라운드 기반 빠른 템포의 전투
- 전략적 위치 선정
- 이동, 점프, 구르기, 대시 등 GAS 기반 캐릭터 애니메이션 모션
- 캐릭터별 개성 있는 스킬/궁극기
- 짧고 강렬한 라운드 단위 플레이

---

## 🛠 개발 구조

- **언리얼 엔진 5.5**  
- **GAS(Gameplay Ability System)** 기반 캐릭터 능력 구현
  - AttributeSet: 캐릭터의 체력과 궁극기 게이지
  - GA : 스킬 구현
  - GE : Tag 부여, Modifier로 AttributeSet 변경
  - Tag : 캐릭터 정보&상태, 스킬 정보 등
- 네트워크
  - 데디케이티드 서버
  - 클라이언트-서버 구조, AimOffset 등 상태 Replication
- 애니메이션
  - ABP(AnimBlueprint)
  - GA 기반 Montage 실행
- UI
  - 메인 메뉴, 캐릭터 UI, 캐릭터 소개

---

## 👥 팀 역할

| 이름 | 역할 | 담당 |
|------|------|------|
| 백승수 | 프로그래머 | GAS, Crunch 구현 |
| 성필준 | 네트워크 | Network, 코드리뷰 |
| 임형택 | 프로그래머 | GAS, TwinBlast 구현 |
| 배수민 | UI/UX | 시작 메뉴, UI, HUD |
| 정경주 | 프로그래머 | GAS, The Fey 구현 |
| 박성신 | 아트디렉터 | 레벨 디자인 |

---

## 📝 트러블슈팅 / 회의록

- 에셋을 이동할 때 깨지는 문제
  - 원인 : 버전 충돌, 큰 용량
  - 해결 : 문제가 있는 특정 파일 삭제 & 이동 전 에셋 재검사

- 사용자 정의 InputComponent를 사용할 시 GC의 대상이 되는 문제
  - 원인 : 기존의 InputComponent를 Cast하여 사용
  - 해결 : Cast 대신 생성자에서 CreateDefaultSubobject를 이용하여 인풋 컴포넌트 생성 후 사용
  - ex) NewInputComponent = CreateDefaultSubobject<UOCInputComponent>(TEXT("NewInputComponent"));

- Notify에서 SpawnActor()를 호출할 수 없는 문제
  - 원인 : Notify에서 직접 GetWorld() 호출 불가
  - 해결 : Notify의 주체인 Character에서 GetWorld()를 호출

- Missile이 Owner와 TargetActor를 구별하지 못함
  - 원인 : Pooling으로 인해 Missile의 GetOwner() 시점이 이르기 때문
  - 해결 : OnHit 방식에서 Overlap 방식으로 변경 + Overlap 함수에서 Actor의 NetRole을 확인

- 몽타주가 재생되지 않는 문제
  - 원인 : Slot 블렌드 문제
  - 해결 : BranchFilter 추가

- GA가 두 번 부여되는 문제
  - 원인 : StartUpData와 코드/기타 경로에서 중복 부여
  - 해결 : StartUpData로 GA 방식 통일

- 쿨다운이 두 개 뜨는 문제
  - 원인 : GA Cooldown 슬롯과 커스텀 쿨타운 GE 동시 사용
  - 해결 : 한 경로만 사용하도록 방식 통일

- UE5.5 AbilityTask 파라미터 차이
  - 원인 : bStopWhenAbilityEnds 등 API 변경
  - 해결 : 새 시그니처로 래퍼 구현

- UFUNCTION(FHitResult*) 컴파일 에러
  - 원인 :  UHT 제약
  - 해결 : BP 노출 제거(C++ 전용 포인터) 또는 레퍼런스+AutoCreateRefTerm

- 로켓펀치 데미지가 0이 되는 문제
  - 원인 : DefaultDamageGE 미세팅
  - 해결 : GE_Damage_Instant 연결

- Barrier Cascade 일시정지
  - 원인 : SetPaused 미지원
  - 해결 : EmitterEnable/SpawnRate 0, 타임고정 파라미터로 대체

- 멀티 입력 미동작
  - 원인 : PC/IMC Add/Remove 시점·Possess 경합
  - 해결 : Controller에서 확실한 타이밍에 IMC 주입

---

## 📂 폴더 구조 예시

/OverClock

//미작성

---

## ⚡ 설치 및 실행



1. 깃허브에서 프로젝트 클론  
2. `OverClock.uproject` 열기 (UE5.5)  
3. Build & Run

---

## 🧩 참고

- **GAS 예시**: 캐릭터 AttributeSet에서 Health 관리  
- **Animation**: AimOffset / Animation BluePrint / Montage
- **네트워크 최적화**: GAS 지원 네트워크, 액터 Replicate, 중요 값은 서버 RPC, Replication  

---
