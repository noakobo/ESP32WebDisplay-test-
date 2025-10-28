#include "noaFrameBuffer.h"

#include <Arduino.h>

//********************************************************************************
/// @brief RGB to RGB565
/// @param r
/// @param g
/// @param b
/// @return RGB565
uint16_t noaFrameBuffer::rgb565(uint8_t r, uint8_t g, uint8_t b) {
  return ((r >> 3) << 11 | (g >> 2) << 5 | (b >> 3));
}

//********************************************************************************
/// @brief pset
/// @param x
/// @param y
/// @param c
void noaFrameBuffer::pset(int x, int y, int colorIndex) {
  if (x < 0 || y < 0 || x >= mWidth || y >= mHeight) return;
  mFrameBuffer[y * mWidth + x] = mColor[colorIndex];
}

//********************************************************************************
/// @brief 矩形描画
/// @param x
/// @param y
/// @param w
/// @param h
/// @param c
void noaFrameBuffer::boxFill(int x, int y, int w, int h, int colorIndex) {
  for (int j = 0; j < h; j++) {
    for (int i = 0; i < w; i++) {
      pset(x + i, y + j, colorIndex);
    }
  }
}

//********************************************************************************
/// @brief 枠を描く
/// @param x
/// @param y
/// @param w
/// @param h
/// @param c
void noaFrameBuffer::box(int x, int y, int w, int h, int colorIndex) {
  // 上辺
  for (int i = 0; i < w; i++) {
    int px = x + i;
    int py = y;
    pset(px, py, colorIndex);
  }

  // 下辺
  for (int i = 0; i < w; i++) {
    int px = x + i;
    int py = y + h - 1;
    pset(px, py, colorIndex);
  }

  // 左辺
  for (int j = 0; j < h; j++) {
    int px = x;
    int py = y + j;
    pset(px, py, colorIndex);
  }

  // 右辺
  for (int j = 0; j < h; j++) {
    int px = x + w - 1;
    int py = y + j;
    pset(px, py, colorIndex);
  }
}

//********************************************************************************
/// @brief 線を引く
/// @param x1 開始x
/// @param y1 開始x
/// @param x2 終了x
/// @param y 2終了x
/// @param c 色
void noaFrameBuffer::line(int x1, int y1, int x2, int y2, int colorIndex) {
  int dx = abs(x2 - x1);
  int dy = abs(y2 - y1);
  int sx = (x1 < x2) ? 1 : -1;
  int sy = (y1 < y2) ? 1 : -1;
  int err = dx - dy;

  while (true) {
    pset(x1, y1, colorIndex);

    if (x1 == x2 && y1 == y2) break;

    int e2 = 2 * err;
    if (e2 > -dy) {
      err -= dy;
      x1 += sx;
    }
    if (e2 < dx) {
      err += dx;
      y1 += sy;
    }
  }
}

//********************************************************************************
/// @brief 円を描く
/// @param x0
/// @param y0
/// @param radius
/// @param colorIndex
void noaFrameBuffer::circle(int x0, int y0, int radius, int colorIndex) {
  int x = radius;
  int y = 0;
  int err = 0;

  while (x >= y) {
    pset(x0 + x, y0 + y, colorIndex);
    pset(x0 + y, y0 + x, colorIndex);
    pset(x0 - y, y0 + x, colorIndex);
    pset(x0 - x, y0 + y, colorIndex);
    pset(x0 - x, y0 - y, colorIndex);
    pset(x0 - y, y0 - x, colorIndex);
    pset(x0 + y, y0 - x, colorIndex);
    pset(x0 + x, y0 - y, colorIndex);

    y += 1;
    if (err <= 0) {
      err += 2 * y + 1;
    }
    if (err > 0) {
      x -= 1;
      err -= 2 * x + 1;
    }
  }
}

//********************************************************************************
/// @brief 塗りつぶした円を描く
/// @param x0
/// @param y0
/// @param radius
/// @param colorIndex
void noaFrameBuffer::circleFill(int x0, int y0, int radius, int colorIndex) {
  int x = radius;
  int y = 0;
  int err = 0;

  while (x >= y) {
    // 対称性を利用して8方向を水平線で埋める
    for (int i = x0 - x; i <= x0 + x; i++) {
      pset(i, y0 + y, colorIndex);
      pset(i, y0 - y, colorIndex);
    }
    for (int i = x0 - y; i <= x0 + y; i++) {
      pset(i, y0 + x, colorIndex);
      pset(i, y0 - x, colorIndex);
    }

    y += 1;
    if (err <= 0) {
      err += 2 * y + 1;
    } else {
      x -= 1;
      err -= 2 * x + 1;
    }
  }
}

//********************************************************************************
/// @brief
void noaFrameBuffer::cls() { memset(mFrameBuffer, 0, sizeof(mFrameBuffer)); }

//********************************************************************************
/// @brief 解像度設定
/// @param w
/// @param h
void noaFrameBuffer::setResolution(int w, int h) {
  mWidth = w;
  mHeight = h;
  // フレームバッファ確保
  mFrameBuffer = (uint16_t*)ps_malloc(sizeof(uint16_t) * (mWidth * mHeight));
  if (!mFrameBuffer) {
    log_e("Failed to allocate FrameBuffer");
  }

  // 背景は黒
  this->cls();
}

//********************************************************************************
/// @brief コンストラクタ
noaFrameBuffer::noaFrameBuffer() {};

//********************************************************************************
/// @brief デストラクタ
noaFrameBuffer::~noaFrameBuffer() { free(mFrameBuffer); }

noaFrameBuffer frameBuffer;  // プロジェクト全体で使えるようにする
