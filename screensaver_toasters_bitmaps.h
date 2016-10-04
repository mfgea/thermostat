#include <Arduino.h>

const uint8_t PROGMEM
 toastermask0[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00,
  0x00, 0x20, 0x00, 0x00, 0x00, 0x88, 0x00, 0x00,
  0x00, 0x10, 0x00, 0x00, 0x00, 0x43, 0xF8, 0x00,
  0x00, 0x1F, 0x80, 0x00, 0x00, 0x7C, 0x3F, 0x80,
  0x01, 0xF1, 0xF8, 0xF0, 0x07, 0xC7, 0xC7, 0x00,
  0x0F, 0x1F, 0x08, 0x2B, 0x1E, 0x7C, 0x11, 0x00,
  0x04, 0xF0, 0x20, 0x56, 0x61, 0xE4, 0x22, 0x00,
  0x78, 0x48, 0x40, 0xA8, 0x4E, 0x10, 0x44, 0x00,
  0x53, 0x90, 0x81, 0xF8, 0x5C, 0xA0, 0x90, 0x58,
  0x7C, 0xA1, 0x07, 0x90, 0x74, 0xA1, 0x21, 0x38,
  0x7F, 0xB5, 0x0E, 0x30, 0x77, 0xB0, 0x90, 0x78,
  0x7F, 0xB4, 0x60, 0xF0, 0x77, 0xBB, 0x03, 0xE0,
  0x7F, 0xBC, 0x0F, 0xC0, 0x77, 0xBF, 0xFF, 0x00,
  0x7F, 0xBF, 0xFC, 0x00, 0x3F, 0xBF, 0xF0, 0x00,
  0x1F, 0xBF, 0xC0, 0x00, 0x07, 0xBE, 0x00, 0x00,
  0x01, 0xB0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
 toastermask1[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x43, 0xF8, 0x00,
  0x00, 0x1F, 0x80, 0x00, 0x00, 0x7C, 0x3F, 0x80,
  0x01, 0xF1, 0xF8, 0x00, 0x07, 0xC7, 0xC1, 0xE0,
  0x0F, 0x1F, 0x06, 0x10, 0x1E, 0x7C, 0x00, 0x00,
  0x04, 0xF0, 0x07, 0xF0, 0x61, 0xE4, 0x18, 0x08,
  0x78, 0x48, 0x20, 0x80, 0x4E, 0x10, 0x40, 0x2B,
  0x53, 0x90, 0x81, 0x00, 0x5C, 0xA0, 0x80, 0x58,
  0x7C, 0xA1, 0x02, 0x00, 0x74, 0xA1, 0x28, 0xB8,
  0x7F, 0xB5, 0x0F, 0xF0, 0x77, 0xB0, 0x90, 0x78,
  0x7F, 0xB4, 0x60, 0xF0, 0x77, 0xBB, 0x03, 0xE0,
  0x7F, 0xBC, 0x0F, 0xC0, 0x77, 0xBF, 0xFF, 0x00,
  0x7F, 0xBF, 0xFC, 0x00, 0x3F, 0xBF, 0xF0, 0x00,
  0x1F, 0xBF, 0xC0, 0x00, 0x07, 0xBE, 0x00, 0x00,
  0x01, 0xB0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
 toastermask2[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xF8, 0x00,
  0x00, 0x1F, 0x80, 0x00, 0x00, 0x7C, 0x3F, 0x80,
  0x01, 0xF1, 0xF8, 0x00, 0x07, 0xC7, 0xC1, 0xE0,
  0x0F, 0x1F, 0x06, 0x10, 0x1E, 0x7C, 0x00, 0x00,
  0x04, 0xF0, 0x00, 0x00, 0x61, 0xE4, 0x00, 0x08,
  0x78, 0x48, 0x00, 0x10, 0x4E, 0x10, 0x00, 0x18,
  0x53, 0x90, 0x60, 0x70, 0x5C, 0xA0, 0x9F, 0xC8,
  0x7C, 0xA1, 0x04, 0x92, 0x74, 0xA1, 0x09, 0x24,
  0x7F, 0xB5, 0x02, 0x48, 0x77, 0xB0, 0x80, 0x10,
  0x7F, 0xB4, 0x41, 0x00, 0x77, 0xBB, 0x20, 0x40,
  0x7F, 0xBC, 0x10, 0x00, 0x77, 0xBF, 0xF8, 0x00,
  0x7F, 0xBF, 0xFC, 0x00, 0x3F, 0xBF, 0xF0, 0x00,
  0x1F, 0xBF, 0xC0, 0x00, 0x07, 0xBE, 0x00, 0x00,
  0x01, 0xB0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
 toastmask[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xC0, 0x00,
  0x00, 0x0C, 0x30, 0x00, 0x00, 0x30, 0x0C, 0x00,
  0x00, 0xC2, 0x23, 0x00, 0x07, 0x00, 0x80, 0xC0,
  0x08, 0x25, 0x50, 0x30, 0x10, 0x0B, 0xA8, 0x08,
  0x21, 0x37, 0xF5, 0x04, 0x30, 0x4A, 0xE8, 0x0C,
  0x3C, 0x15, 0x52, 0x34, 0x2B, 0x00, 0x80, 0xEC,
  0x35, 0xC2, 0x23, 0x54, 0x1A, 0xB0, 0x0E, 0xAC,
  0x0D, 0x5C, 0x15, 0x58, 0x03, 0xAB, 0xEA, 0xE0,
  0x00, 0xD5, 0x55, 0x80, 0x00, 0x3A, 0xAE, 0x00,
  0x00, 0x0D, 0x58, 0x00, 0x00, 0x03, 0xE0, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
 toaster0[] = {
  0x00, 0x30, 0x00, 0x00, 0x00, 0xE8, 0x00, 0x00,
  0x01, 0xD8, 0x00, 0x00, 0x03, 0x74, 0xF0, 0x00,
  0x07, 0xEF, 0xFE, 0x00, 0x07, 0xBC, 0x07, 0x80,
  0x0F, 0xE0, 0x7F, 0xE0, 0x0F, 0x83, 0xC0, 0x70,
  0x1E, 0x0E, 0x07, 0x08, 0x18, 0x38, 0x38, 0xFF,
  0x30, 0xE0, 0xF7, 0xD4, 0x61, 0x83, 0xEE, 0xFF,
  0x7B, 0x0F, 0xDF, 0xA8, 0x9E, 0x1B, 0xDD, 0xFE,
  0x87, 0xB7, 0xBF, 0x50, 0xB1, 0xEF, 0xBB, 0xF8,
  0xAC, 0x6F, 0x7E, 0x00, 0xA3, 0x5F, 0x6F, 0xA0,
  0x83, 0x5E, 0xF8, 0x68, 0x8B, 0x5E, 0xDE, 0xC0,
  0x80, 0x4A, 0xF1, 0xC8, 0x88, 0x4F, 0x6F, 0x80,
  0x80, 0x4B, 0x9F, 0x08, 0x88, 0x44, 0xFC, 0x10,
  0x80, 0x43, 0xF0, 0x20, 0x88, 0x40, 0x00, 0xC0,
  0x80, 0x40, 0x03, 0x00, 0x40, 0x40, 0x0C, 0x00,
  0x20, 0x40, 0x30, 0x00, 0x18, 0x41, 0xC0, 0x00,
  0x06, 0x4E, 0x00, 0x00, 0x01, 0xF0, 0x00, 0x00 },
 toaster1[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0xF0, 0x00,
  0x00, 0xF7, 0xFE, 0x00, 0x01, 0xBC, 0x07, 0x80,
  0x03, 0xE0, 0x7F, 0xE0, 0x07, 0x83, 0xC0, 0x70,
  0x0E, 0x0E, 0x07, 0xF8, 0x18, 0x38, 0x3E, 0x18,
  0x30, 0xE0, 0xF9, 0xE8, 0x61, 0x83, 0xFF, 0xF8,
  0x7B, 0x0F, 0xF8, 0x08, 0x9E, 0x1B, 0xE7, 0xF0,
  0x87, 0xB7, 0xDF, 0x7F, 0xB1, 0xEF, 0xBF, 0xD4,
  0xAC, 0x6F, 0x7E, 0xFF, 0xA3, 0x5F, 0x7F, 0xA6,
  0x83, 0x5E, 0xFD, 0xF8, 0x8B, 0x5E, 0xD7, 0x40,
  0x80, 0x4A, 0xF0, 0x08, 0x88, 0x4F, 0x6F, 0x80,
  0x80, 0x4B, 0x9F, 0x08, 0x88, 0x44, 0xFC, 0x10,
  0x80, 0x43, 0xF0, 0x20, 0x88, 0x40, 0x00, 0xC0,
  0x80, 0x40, 0x03, 0x00, 0x40, 0x40, 0x0C, 0x00,
  0x20, 0x40, 0x30, 0x00, 0x18, 0x41, 0xC0, 0x00,
  0x06, 0x4E, 0x00, 0x00, 0x01, 0xF0, 0x00, 0x00 },
 toaster2[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x00,
  0x00, 0x07, 0xFE, 0x00, 0x00, 0x3C, 0x07, 0x80,
  0x00, 0xE0, 0x7F, 0xE0, 0x03, 0x83, 0xC0, 0x70,
  0x0E, 0x0E, 0x07, 0xF8, 0x18, 0x38, 0x3E, 0x18,
  0x30, 0xE0, 0xF9, 0xE8, 0x61, 0x83, 0xFF, 0xF8,
  0x7B, 0x0F, 0xFF, 0xF8, 0x9E, 0x1B, 0xFF, 0xF0,
  0x87, 0xB7, 0xFF, 0xE8, 0xB1, 0xEF, 0xFF, 0xE0,
  0xAC, 0x6F, 0x9F, 0x88, 0xA3, 0x5F, 0x60, 0x36,
  0x83, 0x5E, 0xFB, 0x6D, 0x8B, 0x5E, 0xF6, 0xDB,
  0x80, 0x4A, 0xFD, 0xB6, 0x88, 0x4F, 0x7F, 0xEE,
  0x80, 0x4B, 0xBE, 0xFC, 0x88, 0x44, 0xDF, 0xBC,
  0x80, 0x43, 0xEF, 0xF8, 0x88, 0x40, 0x07, 0xF0,
  0x80, 0x40, 0x03, 0xE0, 0x40, 0x40, 0x0C, 0xC0,
  0x20, 0x40, 0x30, 0x00, 0x18, 0x41, 0xC0, 0x00,
  0x06, 0x4E, 0x00, 0x00, 0x01, 0xF0, 0x00, 0x00 },
 toast[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x03, 0xC0, 0x00, 0x00, 0x0F, 0xF0, 0x00,
  0x00, 0x3D, 0xDC, 0x00, 0x00, 0xFF, 0x7F, 0x00,
  0x07, 0xDA, 0xAF, 0xC0, 0x0F, 0xF4, 0x57, 0xF0,
  0x1E, 0xC8, 0x0A, 0xF8, 0x0F, 0xB5, 0x17, 0xF0,
  0x03, 0xEA, 0xAD, 0xC8, 0x14, 0xFF, 0x7F, 0x10,
  0x0A, 0x3D, 0xDC, 0xA8, 0x05, 0x4F, 0xF1, 0x50,
  0x02, 0xA3, 0xEA, 0xA0, 0x00, 0x54, 0x15, 0x00,
  0x00, 0x2A, 0xAA, 0x00, 0x00, 0x05, 0x50, 0x00,
  0x00, 0x02, 0xA0, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

const uint8_t * const mask[] PROGMEM = {
  toastermask0, toastermask1, toastermask2, toastermask1, toastmask };
const uint8_t * const img[]  PROGMEM = {
  toaster0    , toaster1    , toaster2    , toaster1    , toast     };


