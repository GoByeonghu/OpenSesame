# OpenSesame(열려라 참깨)

경희대학교 국제캠퍼스 22' 2학기 캡스톤 프로젝트

2022 한국소프트웨어종합학술대회 발표자 선정작

## 요약
최근 부상 중인 IoT 기술 발전의 일환으로, 키패드를 누르지 않아도 동작하거나 스마트폰과 연동하여 잠금을 해제하는 등의 기능을 하는 스마트 도어락이 출시되어 많은 곳에서 상용화되고 있습니다. 하지만 이러한 스마트 도어락은 민감한 개인정보를 다수 포함하고, 해킹의 결과가 치명적인 위험으로 이어질 수 있어 높은 수준의 보안성을 요구합니다. 따라서 본 연구는 보안 성능을 높인 스마트 도어락의 구현을 위해 ARM TrustZone의 사용을 제안합니다. 또한 하드웨어적 기반이 다른 모바일 기기와 IoT 기기 두 단말의 TrustZone 간의 보안성을 확보한 통신 방식을 설계합니다.

본 프로젝트는 이러한 설계를 분리된 프로세스로 시뮬레이션해봅니다.

## 사용 기술

### ARM TrustZone
ARM에서 지원하는 보안 기술로, 하드웨어적으로 일반 실행 환경인 REE와 신뢰 실행 환경인 TEE로 시스템을 독립시킵니다. 보안이 필요한 부분과 그렇지 않은 부분을 독립적으로 구성하여 보안성을 확보합니다. 동작 중 normal world에서 secure world로 접근을 요청할 때는 Monitor mode가 사용됩니다. 본 구성은 GlobalPlatform의 아키텍처를 따른 것입니다. TrustZone이 적용된 ARM 사의 프로세서는 크게 ARM Cortex-A 시리즈와 Cortex-M 시리즈로 구분되며, 전자는 모바일 환경에 후자는 마이크로컨트롤러가 사용되는 저사양 환경(주로 임베디드)에 사용됩니다.

### PGP
인증, 기밀성, 무결성의 보장을 위한 일련의 프로세스입니다. Hash, 대칭키 암복호화, 비대칭키 암복호화가 사용됩니다. 두 단말이 비대칭키 쌍인 개인키와 공개키를 각각 공유합니다. 이후 대칭키를 사용한 세션키를 공유합니다. 송신자가 이후 전송 대상이 되는 데이터를 Hash하고 개인키로 암호화해 전자서명을 만듭니다. 전자서명과 원본 데이터를 세션키로 암호화하여 전송합니다. 수신자는 이를 복호화하고 전자서명을 복호화하여 인증을 완료합니다. 그리고 복호화된 원본 데이터를 Hash하여 전자서명과 비교해 무결성을 검증합니다.

## 동작 시나리오

1. **모바일 단 사용자 등록 시나리오**
    - 모바일 단에서 사용자 등록을 위해 ID, PW를 입력 받습니다. 동시에 RSA 기반 비대칭키 쌍이 생성되어 유저 정보, 비대칭키가 key, value 쌍으로 TEE에 저장됩니다. 이후 사용자는 본인 인증을 위해 ID, PW를 입력하여 TEE에서 인증을 완료한 뒤 허가 메시지가 REE로 전달되면 애플리케이션을 사용합니다.

2. **사용자, 도어락 상호 등록 시나리오**
    - 도어락 단과 모바일 단이 상호 간에 정보를 등록하고 암호키를 교환합니다. 사용자, 도어락 등록 모드가 실행되고 Ad hoc 모드로 도어락 단과 모바일 단이 통신을 준비합니다. 이러한 통신을 위한 애플리케이션은 REE에 위치합니다. 이후 두 가지 전송, 확인 과정을 거칩니다.
        1. 공개키를 도어락으로 전송합니다. 도어락은 수신 상태 메시지와 도어락의 고유 번호를 응답합니다.
        2. 사용자 정보 및 대칭키를 전송합니다. 모바일 단에서 REE의 사용자 애플리케이션은 TEE 영역에 도어락 등록을 위한 사용자 정보와 대칭키를 요청합니다. TEE에서는 AES 대칭키를 생성하고 대칭키와 ID를 개인키로 암호화하여 REE로 반환합니다. 이후 이 암호문은 도어락 단으로 전송됩니다. 이후 도어락 단에서는 받은 암호문을 TEE로 넘겨주고 복호화, 인증, 무결성 검사가 끝나면 REE로 완료됨을 반환합니다. 위의 과정이 끝나면 도어락 등록이 완료됩니다. 성공 메시지가 안드로이드 단으로 도착하여 등록이 완료되면 등록 모드는 종료됩니다. 등록 모드 종료 후에 모바일 단에는 도어락 고유 번호와 세션키가 매칭되어 저장되고 도어락 단에는 사용자 정보(ID)와 세션키, 공개키가 매칭되어 저장됩니다.

3. **도어락 개폐 시나리오**
    - 사용자는 OP-TEE 영역에서 ID, PW 기반의 사용자 인증을 진행한 후 도어락 개폐를 시도합니다. 그 후 사용자의 키 유효 기간을 확인합니다. 키가 유효하다면 개폐 명령 정보를 SHA-256 해시 알고리즘을 적용한 뒤 개인키로 암호화하여 인증서를 만들고, 개폐 명령과 인증서를 세션키로 암호화하여 암호문을 REE로 반환하고 REE에서 도어락 단으로 전송합니다. 도어락은 암호문을 수신합니다. 전달받은 암호문을 KHU-TEE 영역으로 전달해 인증과 무결성 검사를 수행한 후 문 개폐를 수행합니다.

## 아키텍처

### Mobile system architecture overview

![image](https://github.com/capstone1-OpenSesame/OpenSesame/assets/92240138/c0d440b8-88e7-4c40-9127-6b06b9cab693)

### Mobile system architecture overview

![image](https://github.com/capstone1-OpenSesame/OpenSesame/assets/92240138/01ad2ffa-46fb-438e-b474-fe6be7cc2992)


## 설치 및 실행 방법

### 유저 단 라즈베리 파이 ssh 접속

![image](https://github.com/capstone1-OpenSesame/OpenSesame/assets/92240138/b9fbda6f-b330-4232-bcd6-f8bbfff29fdf)

### 도어락 단 라즈베리 파이 ssh 접속

![image](https://github.com/capstone1-OpenSesame/OpenSesame/assets/92240138/35e6e405-97c0-4fe6-934d-c5079ec2cf3a)

### 컴파일

- 유저

```Bash
gcc aes.c rsa.c sha256.c ./socket/tcp.c ./tee/user_REE.c ./tee/user_TEE.c user.c -lcrypto
```
  
- 도어락

```Bash
gcc aes.c rsa.c sha256.c ./socket/tcp.c ./tee/doorlock_REE.c ./tee/doorlock_TEE.c doorlock.c -lcrypto
```




소켓 통신 open을 위해서 실행 순서는 도어락 -> 유저 순으로 실행한다

### 실행(데모)
- 유저 아이디 패스워드 입력

![image](https://github.com/capstone1-OpenSesame/OpenSesame/assets/92240138/a4d07c33-55da-414f-b818-aa12b854b87c)

- 암호키생성 및 공유

![image](https://github.com/capstone1-OpenSesame/OpenSesame/assets/92240138/ac8311cb-8530-44d0-939a-178fa3a83506)

![image](https://github.com/capstone1-OpenSesame/OpenSesame/assets/92240138/b78d4afe-40ed-404a-8d70-7e90d7ef641c)

- 사용자인증

![image](https://github.com/capstone1-OpenSesame/OpenSesame/assets/92240138/770e24ed-48ae-4961-9de1-6840067eb755)

![image](https://github.com/capstone1-OpenSesame/OpenSesame/assets/92240138/f50bb05a-a5ab-4564-8d19-fbabca06b757)

- 개폐명령 생성, 암호화, 전송

![image](https://github.com/capstone1-OpenSesame/OpenSesame/assets/92240138/35351f30-4c5b-4f9d-b1d7-d4bbb1830722)

- 개폐명령 수신, 복호화, 인증

![image](https://github.com/capstone1-OpenSesame/OpenSesame/assets/92240138/a6eeda89-0f16-4e45-9fed-cffcac241c66)

![image](https://github.com/capstone1-OpenSesame/OpenSesame/assets/92240138/29a558a6-3099-4332-812e-b2c9e0de9e19)

