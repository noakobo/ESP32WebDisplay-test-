## 概要
ESP32 内部にフレームバッファを持たせて、ブラウザへ送信する。\
外部接続の OLED ではなく、ブラウザを使うことによって、ハードウェアコストが下がればいいと思い制作。\
フレームバッファはAdafruit-GFX-Libraryなどの方が便利だと思うが、あらゆる技術に対して知識が足りないので、勉強のため作ってみた。\
やっていることは、カメラのリアルタイム転送と同じことらしい。

## 注意事項
- ESP32はPSRAMを搭載していること。
- links2004/WebSocketsを使わせていただきましたが、`WEBSOCKETS_MAX_DATA_SIZE`を考えず送信しています。
- `websocket.broadcastBIN(jpegImage, imageLen);`内でプチフリーズが発生します。
- setup()内の`delay(1000);`は、直後のSerial.printが表示されないため入れています。不要な場合は削除しても問題ありません。

## 設定が必要な部分
無線LAN内に接続する場合は、`WiFi.begin("ssid", "pass");`を使います。\
ESP32単体で使う場合は`WiFi.softAP("esp32WebDisplay", "");`を使います。\
SSID及びパスワードを設定してください。\

## 使い方
PlatformIOで[Upload Filesystem Image]を行って、ESP32内にindex.htmlを保存してください。続いて[Upload and Monitor]を行います。\
起動時に本体のIPアドレスが表示されるので、ブラウザからそのIPアドレスにアクセスしてください。
例`http://192.168.4.1/`\
ブラウザの画面に320x240でグラフィックが表示されるので、ソースコードを適宜書き換えてお使いください。

## 感想
PSRAMを有効にする方法がわからなくて調べたところ`4d_systems_esp32s3_gen4_r8n16`というボードがあったので、そちらを使わせていただきました。

## 開発環境
- Windows11
- vscode
- PlatformIO(ArduinoFramework)
- ESP32R8N16
- links2004/WebSockets
- littleFS
