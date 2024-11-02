# SPISD library for Sony SPRESENSE mainboard

This SD library is made for SPI interface for Spresense. The examples are using SPI5 that is on SPRESENSE mainboard.

## Hardware
SPISD library is designed for a self-made SD card Add-on board for SPRESENSE mainboard. The outlook of the board is below.

![SD card add-on board](https://makers-with-myson.c.blog.ss-blog.jp/_images/blog/_786/makers-with-myson/DSC01995.JPG)

日本語はこちら

[一月の電子工作「SPRESENSEでタイムラプスカメラを作ろう」ーSDカードの検討ー](https://makers-with-myson.blog.ss-blog.jp/2021-01-17)


## How to make the add-on board
### Preparation of the parts
#### (1) microSD card breakout board

<img src = "https://akizukidenshi.com/img/goods/L/105488.jpg" width="300"/>

You can get it at [Akizuki-denshi](https://akizukidenshi.com/catalog/g/g105488/)

#### (2) Level shifter breakout board

<img src = "https://akizukidenshi.com/img/goods/L/104522.jpg" width = "300"/>

You can get it at [Akizuki-denshi](https://akizukidenshi.com/catalog/g/g104522/)

#### (3) Universal board

<img src = "https://akizukidenshi.com/img/goods/L/109608.jpg" width="300"/>

This universal board can be cutted by Scissors. You can get it at [Akizuki-denshi](https://akizukidenshi.com/catalog/g/g109608/)

#### (4) Pin headers

<img src = "https://d2air1d4eqhwg2.cloudfront.net/images/3940/500x500/4b2f0c8c-6cac-4c2a-9eaf-9a871386b28f.jpg" width="300"/>

You can get it at [Switch Science](https://www.switch-science.com/catalog/3940/)

### How to make it
#### (Step1) cut the universal board and make the base board.

![cut universal board](https://makers-with-myson.c.blog.ss-blog.jp/_images/blog/_786/makers-with-myson/DSC01994-7feeb.JPG)
![add pin headers](https://makers-with-myson.c.blog.ss-blog.jp/_images/blog/_786/makers-with-myson/DSC01986-2f6c7.JPG)

#### (step2) build the circuit

SPI5 pins on Spresense Mainboard are here.

![SPI5 pins on Spresense mainboard](https://makers-with-myson.c.blog.ss-blog.jp/_images/blog/_786/makers-with-myson/SPISD-52dc8.png)

The connections between Spresense and SD card break out board

![connection](https://makers-with-myson.c.blog.ss-blog.jp/_images/blog/_786/makers-with-myson/SPISD2.png)

pin connections are below

|SPRESENSE|SD Card|
|----|----|
| SPI_CS | CD/DAT3 |
| SPI_MOSI | CMD |
| 3.3V | VDD |
| SPI_CLK | CLK |
| GND | VSS |
| SPI_MISO | DAT0 |

Wiring the parts

![wiring](https://makers-with-myson.c.blog.ss-blog.jp/_images/blog/_786/makers-with-myson/DSC01988-1e058.JPG)

#### (step3) Packing the parts

packing the parts by glue.

![packing the parts](https://makers-with-myson.c.blog.ss-blog.jp/_images/blog/_786/makers-with-myson/DSC01989.JPG)

The result of the work

![SD card add-on board](https://makers-with-myson.c.blog.ss-blog.jp/_images/blog/_786/makers-with-myson/DSC01995.JPG)
