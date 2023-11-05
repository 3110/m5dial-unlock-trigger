# ダイヤル解錠トリガー

[![ダイヤル解錠トリガー動作の様子](http://img.youtube.com/vi/76DNG9YMoTE/0.jpg)](https://www.youtube.com/watch?v=76DNG9YMoTE)

M5Dialを使ってダイヤル錠を解錠するように決まった回し方をすることで，何かアクションを起こすきっかけにできる仕組みです。

## 1. 事前準備

### 1.1 コンパイル方法

[PlatformIO IDE](https://platformio.org/platformio-ide)上でコンパイル・インストールします。Windows 11上で動くVSCodeで動作確認をしています。

### 1.2 アイコンファイルの転送

`data`フォルダに置いてあるアイコンファイルをSPIFFS（SPI Flash File System）に転送します。

PlatformIO メニューから「Upload Filesystem Image」を選択するか，コマンドラインから`pio run --target uploadfs`を実行してSPIFFS にアップロードします。

## 2. 実装方法

`M5ComboLock`の`begin(const int8_t dials[], size_t len, std::function<bool(void)> onValid)`にダイヤルの回し方を定義した配列と解錠できた場合のコールバックを指定します。

ダイヤルを回した後，一定時間（デフォルトでは`DIAL_INTERVAL_MS`で指定した1.5秒）待つと値が確定し，画面に表示される値が`0`になり，入力が継続されます。間違えた回し方をした場合は状態がリセットされるため，正しく回し続けないと解錠できません（確定したかリセットしたかは見た目ではわかりません）。ダイヤルを一定時間（デフォルトでは`DIAL_TIMEOUT_MS`で指定した3秒）回さなかった場合は入力状態がリセットされます。

今回，実装例として`examples/BLEKeyboard`を用意しています。M5DialをBLEキーボードとしてPCに接続し，円周率の最初の8つの値（3.1415926）をそれぞれ左右に回す（右に3回，左に1回，右に4回…）と解錠できるようになっています。解錠できると，接続したPCに`CTRL+ALT+DELETE`を送ります。

解錠のための回し方は`examples/BLEKeyboard/main.cpp`の`N_DIALS`で定義しています。右に回すときは正の値，左に回すときは負の値を指定します。

## 3. TODO

* 解錠するための回し方を登録する方法の追加
* ダイヤルを回すときや解錠できたときに音を鳴らす
