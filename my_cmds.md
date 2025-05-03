
# ESP32 OpenCV Useful Commands

**Qemu with trace exec_tb (so I get guest PC addresses), singlestep (one-insn-per-tb=on):**

```console
U:/my_xtensa_qemu/qemu_tracesimple/qemu-system-xtensa.exe \
  -nographic \
  -machine esp32s3,memory-backend=ram0 \
  -object memory-backend-ram,id=ram0,size=8M,prealloc=on,share=on \
  -drive file=flash_image.bin,if=mtd,format=raw \
  -global driver=timer.esp32c3.timg,property=wdt_disable,value=true \
  -smp 2 \
  -accel tcg,one-insn-per-tb=on \
  -trace exec_tb
```

**Qemu no trace:**

```console
U:/my_xtensa_qemu/qemu_tracesimple/qemu-system-xtensa.exe \
  -nographic \
  -machine esp32s3,memory-backend=ram0 \
  -object memory-backend-ram,id=ram0,size=8M,prealloc=on,share=on \
  -drive file=flash_image.bin,if=mtd,format=raw \
  -global driver=timer.esp32c3.timg,property=wdt_disable,value=true \
  -smp 2
```

**Create flash image for qemu (checkout build/flash_args):**

```console
 esptool.py --chip esp32s3 merge_bin --fill-flash-size 16MB -o flash_image.bin --flash_mode dio --flash_freq 80m --flash_size 16MB 0x0 bootloader/bootloader.bin 0x10000 esp32_opencv_calib3d.bin 0x8000 partition_table/partition-table.bin 0x394000 storage.bin
```

**simpletrace.py:**

```console
Oleksii@win_pc_oleksii MINGW64 /u/cpp_proj/vscode_espidf/15_fast_tests_2
$ /u/qemu_build/qemu/scripts/simpletrace.py /u/my_xtensa_qemu/qemu_tracesimple/share/trace-events-all trace-32492
```

**simpletrace.py PC\n to file:**

```console
/u/qemu_build/qemu/scripts/simpletrace.py /u/my_xtensa_qemu/qemu_tracesimple/share/trace-events-all trace-32492 \
  | sed -n 's/.*pc=\(0x[0-9A-Fa-f]\+\).*/\1/p' \
  > all_pcs_2.txt
```

**Logs Qemu with trace exec_tb, singlestep:**

```console
Oleksii@win_pc_oleksii MINGW64 /u/cpp_proj/vscode_espidf/15_fast_tests_2
$ U:/my_xtensa_qemu/qemu_tracesimple/qemu-system-xtensa.exe \
  -nographic \
  -machine esp32s3,memory-backend=ram0 \
  -object memory-backend-ram,id=ram0,size=8M,prealloc=on,share=on \
  -drive file=flash_image.bin,if=mtd,format=raw \
  -global driver=timer.esp32c3.timg,property=wdt_disable,value=true \
  -smp 2 \
  -accel tcg,one-insn-per-tb=on \
  -trace exec_tb
ESP-ROM:esp32s3-20210327
Build:Mar 27 2021
rst:0x1 (POWERON),boot:0x4 (SPI_FLASH_BOOT)
SPIWP:0xee
mode:DIO, clock div:1
load:0x3fce2810,len:0x166c
load:0x403c8700,len:0x4
load:0x403c8704,len:0xd98
load:0x403cb700,len:0x3338
entry 0x403c89a0
I (74) boot: ESP-IDF v5.4.1-268-g6897a7bf40 2nd stage bootloader
I (77) boot: compile time Apr 21 2025 10:11:55
I (78) boot: Multicore bootloader
I (82) boot: chip revision: v0.0
I (83) boot: efuse block revision: v0.0
I (86) boot.esp32s3: Boot SPI Speed : 80MHz
I (87) boot.esp32s3: SPI Mode       : SLOW READ
I (88) boot.esp32s3: SPI Flash Size : 16MB
I (90) boot: Enabling RNG early entropy source...
I (96) boot: Partition Table:
I (96) boot: ## Label            Usage          Type ST Offset   Length
I (98) boot:  0 nvs              WiFi data        01 02 00009000 00006000
I (101) boot:  1 phy_init         RF data          01 01 0000f000 00001000
I (103) boot:  2 factory          factory app      00 00 00010000 00384000
I (105) boot:  3 storage          Unknown data     01 82 00394000 00177000
I (107) boot: End of partition table
I (109) esp_image: segment 0: paddr=00010020 vaddr=3c1f0020 size=90af4h (592628) map
I (577) esp_image: segment 1: paddr=000a0b1c vaddr=3fc92500 size=0e270h ( 57968) load
I (616) esp_image: segment 2: paddr=000aed94 vaddr=40374000 size=01284h (  4740) load
I (620) esp_image: segment 3: paddr=000b0020 vaddr=42000020 size=1e5148h (1986888) map
I (1567) esp_image: segment 4: paddr=00295170 vaddr=40375284 size=0d178h ( 53624) load
I (1605) esp_image: segment 5: paddr=002a22f0 vaddr=600fe100 size=0001ch (    28) load
I (1642) boot: Loaded app from partition at offset 0x10000
I (1644) boot: Disabling RNG early entropy source...
W (3326) rtcinit: o_code calibration fail

I (1669) esp_psram: Found 8MB PSRAM device
I (1670) esp_psram: Speed: 80MHz
I (1672) cpu_start: Multicore app
I (1853) esp_psram: SPI SRAM memory test OK
I (1866) cpu_start: Pro cpu start user code
I (1867) cpu_start: cpu freq: 240000000 Hz
I (1867) app_init: Application information:
I (1868) app_init: Project name:     esp32_opencv_calib3d
I (1869) app_init: App version:      1
I (1869) app_init: Compile time:     Apr 21 2025 10:10:28
I (1870) app_init: ELF file SHA256:  75b9735b8...
I (1871) app_init: ESP-IDF:          v5.4.1-268-g6897a7bf40
I (1871) efuse_init: Min chip rev:     v0.0
I (1872) efuse_init: Max chip rev:     v0.99
I (1873) efuse_init: Chip rev:         v0.0
I (1874) heap_init: Initializing. RAM available for dynamic allocation:
I (1876) heap_init: At 3FCAB350 len 0003E3C0 (248 KiB): RAM
I (1877) heap_init: At 3FCE9710 len 00005724 (21 KiB): RAM
I (1879) heap_init: At 3FCF0000 len 00008000 (32 KiB): DRAM
I (1880) heap_init: At 600FE11C len 00001ECC (7 KiB): RTCRAM
I (1883) esp_psram: Adding pool of 8192K of PSRAM memory to heap allocator
I (1888) spi_flash: detected chip: issi
I (1889) spi_flash: flash io: dio
I (1910) sleep_gpio: Configure to isolate all GPIO pins in sleep state
I (1914) sleep_gpio: Enable automatic switching of GPIO sleep configuration
I (1918) main_task: Started on CPU0
I (1918) esp_psram: Reserving pool of 32K of internal memory for DMA/internal allocations
I (1928) main_task: Calling app_main()
I (1938) opencv_tests: Starting main
I (1938) SYSTEM: task stack watermark: 1996 Bytes
I (1948) SYSTEM: heap left: 8642852 Bytes
I (1948) spiffs_init: Initializing SPIFFS
I (3138) spiffs_init: Partition size: total: 1404596, used: 1400329
I (3148) opencv_tests: End of main
I (3158) main_task: Returned from app_main()
libpng warning: iCCP: known incorrect sRGB profile
I (5648) opencv_tests: Image read of 120x160x1, with 0 pixel depth
I (5648) opencv_tests: next img
libpng warning: iCCP: known incorrect sRGB profile
I (9648) opencv_tests: Image read of 240x320x1, with 0 pixel depth
I (9658) opencv_tests: next img
libpng warning: iCCP: known incorrect sRGB profile
I (19728) opencv_tests: Image read of 480x640x1, with 0 pixel depth
I (19728) opencv_tests: next img
libpng warning: iCCP: known incorrect sRGB profile
I (39928) opencv_tests: Image read of 768x1024x1, with 0 pixel depth
I (39928) opencv_tests: next img
starting benchmark with 4 images, 6 groups
Group contains 4 testcases
I (41438) BENCHMARK: task stack watermark: 10236 Bytes
I (41448) BENCHMARK: heap left: 7411148 Bytes
I (44178) BENCHMARK: task stack watermark: 10236 Bytes
I (44188) BENCHMARK: heap left: 7411128 Bytes
I (47138) BENCHMARK: task stack watermark: 10204 Bytes
I (47148) BENCHMARK: heap left: 7411108 Bytes
I (48368) BENCHMARK: task stack watermark: 10204 Bytes
I (48378) BENCHMARK: heap left: 7411088 Bytes
Group contains 3 testcases
I (56578) BENCHMARK: task stack watermark: 10204 Bytes
I (56578) BENCHMARK: heap left: 7410892 Bytes
QEMU 9.2.2 monitor - type 'help' for more information
(qemu) quit

Oleksii@win_pc_oleksii MINGW64 /u/cpp_proj/vscode_espidf/15_fast_tests_2
$ U:/my_xtensa_qemu/qemu_tracesimple/qemu-system-xtensa.exe   -nographic   -machine esp32s3,memory-backend=ram0   -object memory-backend-ram,id=ram0,size=8M,prealloc=on,share=on   -drive file=flash_image.bin,if=mtd,format=raw   -global driver=timer.esp32c3.timg,property=wdt_disable,value=true   -smp 2   -accel tcg,one-insn-per-tb=on   -trace exec_tb
ESP-ROM:esp32s3-20210327
Build:Mar 27 2021
rst:0x1 (POWERON),boot:0x4 (SPI_FLASH_BOOT)
SPIWP:0xee
mode:DIO, clock div:1
load:0x3fce2810,len:0x166c
load:0x403c8700,len:0x4
load:0x403c8704,len:0xd98
load:0x403cb700,len:0x3338
entry 0x403c89a0
I (37) boot: ESP-IDF v5.4.1-268-g6897a7bf40 2nd stage bootloader
I (40) boot: compile time Apr 21 2025 10:11:55
I (41) boot: Multicore bootloader
I (45) boot: chip revision: v0.0
I (46) boot: efuse block revision: v0.0
I (49) boot.esp32s3: Boot SPI Speed : 80MHz
I (50) boot.esp32s3: SPI Mode       : SLOW READ
I (51) boot.esp32s3: SPI Flash Size : 16MB
I (53) boot: Enabling RNG early entropy source...
I (58) boot: Partition Table:
I (59) boot: ## Label            Usage          Type ST Offset   Length
I (60) boot:  0 nvs              WiFi data        01 02 00009000 00006000
I (63) boot:  1 phy_init         RF data          01 01 0000f000 00001000
I (64) boot:  2 factory          factory app      00 00 00010000 00384000
I (66) boot:  3 storage          Unknown data     01 82 00394000 00177000
I (68) boot: End of partition table
I (70) esp_image: segment 0: paddr=00010020 vaddr=3c1f0020 size=90af4h (592628) map
I (300) esp_image: segment 1: paddr=000a0b1c vaddr=3fc92500 size=0e270h ( 57968) load
I (383) esp_image: segment 2: paddr=000aed94 vaddr=40374000 size=01284h (  4740) load
I (393) esp_image: segment 3: paddr=000b0020 vaddr=42000020 size=1e5148h (1986888) map
I (1270) esp_image: segment 4: paddr=00295170 vaddr=40375284 size=0d178h ( 53624) load
I (1292) esp_image: segment 5: paddr=002a22f0 vaddr=600fe100 size=0001ch (    28) load
I (1310) boot: Loaded app from partition at offset 0x10000
I (1311) boot: Disabling RNG early entropy source...
W (2651) rtcinit: o_code calibration fail

I (1331) esp_psram: Found 8MB PSRAM device
I (1332) esp_psram: Speed: 80MHz
I (1333) cpu_start: Multicore app
I (1516) esp_psram: SPI SRAM memory test OK
I (1530) cpu_start: Pro cpu start user code
I (1531) cpu_start: cpu freq: 240000000 Hz
I (1532) app_init: Application information:
I (1533) app_init: Project name:     esp32_opencv_calib3d
I (1533) app_init: App version:      1
I (1534) app_init: Compile time:     Apr 21 2025 10:10:28
I (1535) app_init: ELF file SHA256:  75b9735b8...
I (1536) app_init: ESP-IDF:          v5.4.1-268-g6897a7bf40
I (1537) efuse_init: Min chip rev:     v0.0
I (1537) efuse_init: Max chip rev:     v0.99
I (1538) efuse_init: Chip rev:         v0.0
I (1540) heap_init: Initializing. RAM available for dynamic allocation:
I (1541) heap_init: At 3FCAB350 len 0003E3C0 (248 KiB): RAM
I (1543) heap_init: At 3FCE9710 len 00005724 (21 KiB): RAM
I (1544) heap_init: At 3FCF0000 len 00008000 (32 KiB): DRAM
I (1545) heap_init: At 600FE11C len 00001ECC (7 KiB): RTCRAM
I (1548) esp_psram: Adding pool of 8192K of PSRAM memory to heap allocator
I (1555) spi_flash: detected chip: issi
I (1556) spi_flash: flash io: dio
I (1573) sleep_gpio: Configure to isolate all GPIO pins in sleep state
I (1576) sleep_gpio: Enable automatic switching of GPIO sleep configuration
I (1581) main_task: Started on CPU0
I (1611) esp_psram: Reserving pool of 32K of internal memory for DMA/internal allocations
I (1611) main_task: Calling app_main()
I (1611) opencv_tests: Starting main
I (1611) SYSTEM: task stack watermark: 1996 Bytes
I (1611) SYSTEM: heap left: 8642852 Bytes
I (1611) spiffs_init: Initializing SPIFFS
I (2621) spiffs_init: Partition size: total: 1404596, used: 1400329
I (2631) opencv_tests: End of main
I (2631) main_task: Returned from app_main()
libpng warning: iCCP: known incorrect sRGB profile
I (4141) opencv_tests: Image read of 120x160x1, with 0 pixel depth
I (4141) opencv_tests: next img
libpng warning: iCCP: known incorrect sRGB profile
I (7641) opencv_tests: Image read of 240x320x1, with 0 pixel depth
I (7651) opencv_tests: next img
libpng warning: iCCP: known incorrect sRGB profile
I (15831) opencv_tests: Image read of 480x640x1, with 0 pixel depth
I (15831) opencv_tests: next img
libpng warning: iCCP: known incorrect sRGB profile
I (35631) opencv_tests: Image read of 768x1024x1, with 0 pixel depth
I (35631) opencv_tests: next img
starting benchmark with 4 images, 6 groups
Group contains 4 testcases
I (37131) BENCHMARK: task stack watermark: 10252 Bytes
I (37141) BENCHMARK: heap left: 7411148 Bytes
I (40091) BENCHMARK: task stack watermark: 10252 Bytes
I (40091) BENCHMARK: heap left: 7411128 Bytes
I (43361) BENCHMARK: task stack watermark: 9052 Bytes
I (43361) BENCHMARK: heap left: 7411108 Bytes
I (44751) BENCHMARK: task stack watermark: 9052 Bytes
I (44761) BENCHMARK: heap left: 7411088 Bytes
Group contains 3 testcases
I (51561) BENCHMARK: task stack watermark: 9052 Bytes
I (51561) BENCHMARK: heap left: 7410892 Bytes
I (151941) BENCHMARK: task stack watermark: 9052 Bytes
I (151961) BENCHMARK: heap left: 7410872 Bytes
I (316001) BENCHMARK: task stack watermark: 9052 Bytes
I (316011) BENCHMARK: heap left: 7410852 Bytes
Group contains 3 testcases
I (324911) BENCHMARK: task stack watermark: 9052 Bytes
I (324921) BENCHMARK: heap left: 7410832 Bytes
I (334251) BENCHMARK: task stack watermark: 9052 Bytes
I (334251) BENCHMARK: heap left: 7410812 Bytes
I (352831) BENCHMARK: task stack watermark: 9052 Bytes
I (352831) BENCHMARK: heap left: 7410792 Bytes
Group contains 2 testcases
I (358821) BENCHMARK: task stack watermark: 9052 Bytes
I (358831) BENCHMARK: heap left: 7410772 Bytes
I (372571) BENCHMARK: task stack watermark: 9052 Bytes
I (372591) BENCHMARK: heap left: 7410752 Bytes
Group contains 1 testcases
I (398651) BENCHMARK: task stack watermark: 9052 Bytes
I (398651) BENCHMARK: heap left: 7410732 Bytes
Group contains 2 testcases
I (511241) BENCHMARK: task stack watermark: 7996 Bytes
I (511251) BENCHMARK: heap left: 7410696 Bytes
I (604231) BENCHMARK: task stack watermark: 7852 Bytes
I (604261) BENCHMARK: heap left: 7410676 Bytes
+------------------------------------------------+-------------+-------------+-------------+-------------+
| Function name and arguments                    | BUILD_TYPE=Release                                    |
+------------------------------------------------+-------------+-------------+-------------+-------------+
|                                                |     160x120 |     320x240 |     640x480 |    1024x768 |
+================================================+=============+=============+=============+=============+
|                                                                                                        |
+------------------------------------------------+-------------+-------------+-------------+-------------+
| **Threshold**                                                                                          |
+------------------------------------------------+-------------+-------------+-------------+-------------+
| binaryThreshold                                |          26 |          53 |         105 |         273 |
+------------------------------------------------+-------------+-------------+-------------+-------------+
| triangleThreshold                              |          34 |          68 |         243 |         601 |
+------------------------------------------------+-------------+-------------+-------------+-------------+
| OTSUThreshold                                  |          59 |         128 |         267 |         599 |
+------------------------------------------------+-------------+-------------+-------------+-------------+
| toZeroThreshold                                |         6.7 |          24 |         129 |         267 |
+------------------------------------------------+-------------+-------------+-------------+-------------+
|                                                                                                        |
+------------------------------------------------+-------------+-------------+-------------+-------------+
| **Blurring**                                                                                           |
+------------------------------------------------+-------------+-------------+-------------+-------------+
| GaussianBlur 3x3 kernel                        |          43 |         147 |         549 |        1493 |
+------------------------------------------------+-------------+-------------+-------------+-------------+
| medianBlur 3x3 kernel                          |         533 |        1926 |        8435 |       22535 |
+------------------------------------------------+-------------+-------------+-------------+-------------+
| bilateralFilter diameter=5                     |         945 |        3433 |       14326 |       35943 |
+------------------------------------------------+-------------+-------------+-------------+-------------+
|                                                                                                        |
+------------------------------------------------+-------------+-------------+-------------+-------------+
| **Morphological transforms**                                                                           |
+------------------------------------------------+-------------+-------------+-------------+-------------+
| erode 5x5 kernel                               |          61 |         179 |         718 |        1975 |
+------------------------------------------------+-------------+-------------+-------------+-------------+
| dilate 5x5 kernel                              |          49 |         177 |         783 |        2066 |
+------------------------------------------------+-------------+-------------+-------------+-------------+
| open 5x5 kernel                                |         133 |         425 |        1568 |        4028 |
+------------------------------------------------+-------------+-------------+-------------+-------------+
|                                                                                                        |
+------------------------------------------------+-------------+-------------+-------------+-------------+
| **Resize image**                                                                                       |
+------------------------------------------------+-------------+-------------+-------------+-------------+
| resize linear interpolation                    |          72 |         138 |         439 |        1310 |
+------------------------------------------------+-------------+-------------+-------------+-------------+
| resize cubic interpolation                     |         102 |         313 |        1199 |        2933 |
+------------------------------------------------+-------------+-------------+-------------+-------------+
|                                                                                                        |
+------------------------------------------------+-------------+-------------+-------------+-------------+
| **Edge detection**                                                                                     |
+------------------------------------------------+-------------+-------------+-------------+-------------+
| Sobel                                          |         193 |         619 |        2235 |        5605 |
+------------------------------------------------+-------------+-------------+-------------+-------------+
|                                                                                                        |
+------------------------------------------------+-------------+-------------+-------------+-------------+
| **Hough transformations**                                                                              |
+------------------------------------------------+-------------+-------------+-------------+-------------+
| HoughLines                                     |        2071 |        4051 |       10620 |       17009 |
+------------------------------------------------+-------------+-------------+-------------+-------------+
| HoughLines probabilistic                       |        1477 |        3196 |        8775 |       14024 |
+------------------------------------------------+-------------+-------------+-------------+-------------+

QEMU 9.2.2 monitor - type 'help' for more information
(qemu) quit
```
