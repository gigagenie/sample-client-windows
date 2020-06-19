# sample-client-windows

Windows Device SDK 샘플 클라이언트

본 프로젝트는 Window OS기반으로 동작하는 GiGA Genie Inside(이하, G-INSIDE) SDK sample app을 포함합니다. 

## GiGA Genie Inside
GiGA Genie Inside(이하, G-INSIDE)는 3rd party 개발자가 자신들의 제품(단말 장치, 서비스, 앱 등)에 KT의 AI Platform인 
'기가지니'를 올려서 음성인식과 자연어로 제어하고 기가지니가 제공하는 서비스(생활비서, 뮤직, 라디오 등)를 사용할 수 있도록 해줍니다.
G-INSIDE는 기가지니가 탑재된 제품을 개발자들이 쉽게 만들 수 있도록 개발 도구와 문서, 샘플 소스 등 개발에 필요한 리소스를 제공합니다.

## Prerequisites
* OS: Windows (7 > 권장)
* Build Tool: Visual Studio 2019
* [G-INSIDE Windows Device SDK](https://github.com/gigagenie/ginside-sdk/tree/master/g-sdk-windows)
* [Gom Player](https://www.gomlab.com/download/) : 샘플 클라이언트의 미디어 플레이를 위해 설치 필요

## 인사이드 디바이스 키 발급

1. [API Link](https://apilink.kt.co.kr) 에서 회원가입 
2. 사업 제휴 신청 및 디바이스 등록 (Console > GiGA Genie > 인사이드 디바이스 등록)
3. 디바이스 등록 완료 후 My Device에서 등록한 디바이스 정보 및 개발키 발급 확인 (Console > GiGA Genie > My Device)

## Window용 Sample 빌드
- sample-client-windows를 다운로드 받고 디렉토리로 이동하여 솔루션 파일인 ginsidemfc-test.sln을 오픈한다.
- **Visual Studio에 외부 라이브러리 추가 방법**을 참고하여 운영체제에 맞는 플랫폼의 라이브러리를 추가한다.
- 테스트를 위해 x86/x64 Release 모드로 변경 후 Build > Build Solution을 클릭하여 빌드를 시작한다.
- Build가 성공적으로 끝나면 x64/Release(64bit) 혹은 Release(32bit) 디렉토리에 ginsidemfc-test.exe 파일이 생성된다.
- Application 실행을 위해 .exe 실행파일이 있는 폴더에 플랫폼에 맞는 [dll](https://github.com/gigagenie/ginside-sdk/tree/master/g-sdk-windows/dll) 파일들과 KWS 모델 데이터 및 roots.pem [데이터](https://github.com/gigagenie/ginside-sdk/tree/master/g-sdk-windows/data)를 위치시킨다.
- Application 실행 전 인사이드 디바이스 키를 설정한다. key.txt 파일을 생성하고 아래 내용을 실제 발급받은 키값으로 입력한다.
    ```
    YOUR-CLIENT-ID
    YOUR-CLIENT-KEY
    YOUR-CLIENT-SECRET
    ```

- ginsidemfc-test.exe 파일을 실행한다.

## Visual Studio에 외부 라이브러리 추가 방법

Visual Studio에서 외부 라이브러리를 링크하는 방법에 대해서 아래에 설명한다.

- ginsidemfc-test.sln 파일을 오픈하고 오른쪽 상단의 솔루션 탐색기에서 프로젝트 명을 오른쪽 마우스 클릭하여 속성 페이지에 들어간다.
- 속성창 왼쪽 메뉴에서 **C/C++ > 일반**탭을 선택하고 **추가 포함 디렉터리리**의 편집 화면에 진입한다.
- Window SDK와 같이 배포되는 [include/](https://github.com/gigagenie/ginside-sdk/tree/master/g-sdk-windows/include) 내 헤더 파일의 경로를 추가한다. 이때 빌드하는 플랫폼(x64/x86)과 동일한 플랫폼으로 제공되는 헤더 파일의 경로를 아래와 같이 추가한다.
  - 64bit 플랫폼
    ```
    ..\include\cJSON\include
    ..\include
    ```
  - 32bit 플랫폼
    ```
    ..\include\cJSON-x86\include
    ..\include
    ```
- 그 후 속성창 왼쪽 메뉴에서 **링커 > 일반**탭을 선택하고 **추가 라이브러리 디렉터리**의 편집 화면에 진입한다.
- Window SDK와 같이 배포되는 [lib/](https://github.com/gigagenie/ginside-sdk/tree/master/g-sdk-windows/lib) 내 라이브러리의 경로를 추가한다. 앞선 설명에서와 같이 빌드하는 플랫폼과 동일한 플랫폼으로 제공되는 라이브러리 경로를 아래와 같이 추가한다.
  - 64bit 플랫폼
    ```
    ..\lib\cjson
    ..\lib\ginsidewin
    ```
  - 32bit 플랫폼
    ```
    ..\lib\cjson-x86
    ..\lib\ginsidewin-x86
    ```

- 마지막으로 **링커 > 입력**탭을 선택하고 **추가 종속성**의 편집 화면에 진입한다.
- 빌드에 필요한 아래 라이브러리를 추가하고 저장한다.
  - cjson.lib, ginsidewin.lib
  

## 라이선스

**Sample Client Windows** is licensed under the [Apache License 2.0](http://www.apache.org/licenses/LICENSE-2.0).

sample-client-windows는 아래 라이브러리를 사용합니다.
* cJSON : MIT license(https://github.com/DaveGamble/cJSON/blob/master/LICENSE)
