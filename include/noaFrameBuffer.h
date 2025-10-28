/**
 * RGB565のフレームバッファ
 * 320x240で10fpsくらいになった
 */
#pragma once

#include <Arduino.h>

class noaFrameBuffer {
 private:
  uint16_t* mFrameBuffer = nullptr;  // フレームバッファ
  uint16_t mWidth, mHeight;          // 解像度
  uint16_t mColor[8] = {rgb565(0, 0, 0),     rgb565(255, 0, 0),
                        rgb565(0, 255, 0),   rgb565(255, 255, 0),
                        rgb565(0, 0, 255),   rgb565(255, 0, 255),
                        rgb565(0, 255, 255), rgb565(255, 255, 255)};

 public:
  // 描画関数
  void pset(int x, int y, int colorIndex);
  void line(int x1, int y1, int x2, int y2, int colorIndex);
  void box(int x, int y, int w, int h, int colorIndex);
  void boxFill(int x, int y, int w, int h, int colroIndex);
  void circle(int x, int y, int r, int colorIndex);
  void circleFill(int x, int y, int r, int colorIndex);
  void cls();

  // その他
  inline uint16_t* getFrameBuffer() { return mFrameBuffer; };
  inline size_t getSize() { return sizeof(mFrameBuffer); }

  void setResolution(int w, int h);

  uint16_t rgb565(uint8_t r, uint8_t g, uint8_t b);

  noaFrameBuffer();
  ~noaFrameBuffer();
};

extern noaFrameBuffer frameBuffer;  // プロジェクト全体で使えるようにする
