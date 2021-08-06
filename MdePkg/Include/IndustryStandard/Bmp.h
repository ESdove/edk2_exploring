/** @file
  This file defines BMP file header data structures.

Copyright (c) 2006 - 2011, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/
//此文件定义了BMP文件头的数据结构
#ifndef _BMP_H_
#define _BMP_H_

#pragma pack(1)

// 4 Byte 表示一个像素点的颜色
typedef struct {
  UINT8   Blue;
  UINT8   Green;
  UINT8   Red;
  UINT8   Reserved;
} BMP_COLOR_MAP;

//描述BMP图片Header的结构
typedef struct {
  CHAR8         CharB;
  CHAR8         CharM;
  UINT32        Size;         
  UINT16        Reserved[2];
  UINT32        ImageOffset; //图片的偏移
  UINT32        HeaderSize; //sizeof(BMP_IMAGE_HEADER)
  UINT32        PixelWidth; //像素点宽度
  UINT32        PixelHeight; //像素点高度
  UINT16        Planes;          ///< Must be 1
  UINT16        BitPerPixel;     ///< 1, 4, 8, or 24
  UINT32        CompressionType;
  UINT32        ImageSize;       ///< Compressed image size in bytes
  UINT32        XPixelsPerMeter;
  UINT32        YPixelsPerMeter;
  UINT32        NumberOfColors;
  UINT32        ImportantColors;
} BMP_IMAGE_HEADER;

#pragma pack()

#endif
