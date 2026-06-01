# 4.2" 300×400 reflective SPI module (ST7305) — documentation & samples

**简体中文：** [`README.md`](README.md)

---

> This repository provides an **ESP-IDF sample project**. Datasheets and specifications will be added to `docs/` when available.

## Product overview

| Item | Description |
|:--|:--|
| Module | 4.2-inch **reflective LCD** (monochrome), **300×400** (400×300 landscape) |
| Interface | **SPI** |
| Driver IC | **ST7305** |
| Spec ID | **`4.2-lcd-300x400-spi-st7305`** is the common product designation in documentation |
| Other 4.2″ variant | **Tri-color reflective** is in **`4.2-lcd-300x400-spi-st7306`** (ST7306) — separate repo |

---

## Repository layout

### Top-level

| Path | Contents |
|:--|:--|
| `assets/` | Demo screenshots for sample projects (when available) |
| `docs/` | Datasheets and specifications (**to be added**) |
| `examples/` | **Sample projects** |

### `examples/` layout

| Location | Description |
|:--|:--|
| `examples/` root | ESP32-S3: ST7305 SPI display; optional FT3269 touch drawing demo |

### Sample project paths

| Description | Path |
|:--|:--|
| ST7305 SPI bringup (FT3269 touch demo) | `examples/esp32s3-4.2-tft-300x400-spi-st7305-bringup/` |
