# easiOS
Az __easiOS__ egy fejlesztés alatt álló, x86-os architektúrára tervezett operációs rendszer.

* Kooperatív szoftveres multitasking
* Billentyűzet orientált ablakkezelő, a Windows 2000 E S Z T É T I K Á I által inspirált dizájnnal
* Lua 5.1-es, bájtkód mint futtatható fájl
* Multiboot2 kompatibilis kernelfájl
* FAT32 fájlrendszer

## Kernel fordítása
*a fordítási rendszer épp átírás alatt áll*

## Rendszer futtatása
### Követelmények:
* Multiboot2 kompatibilis rendszerbetöltő (Syslinux, GRUB2, stb.)
* IBM PC, legalább SSE-t támogató processzorral (Pentium 3 vagy jobb), legalább 256MB memória

### Futattás virtuális gépben
1. Készítsd el a kernelt a fent leírt módon
2. Csatold fel a forráskód gyökerében található ISO fájlt a virtuális gépre
3. Futtasd a gépet

### Futtatás valódi gépen
1. Készítsd el a kernelt a fent leírt módon
2. Másold a forráskód gyökerében található BIN fájlt a rendszerbetöltő mappájába (pl. /boot)
3. Add hozzá bejegyzést a rendszerbetöltőhöz (példa GRUB2-hoz a /iso/boot/grub/grub.cfg fájlban)

## Mi működik
## Támogatott eszközök

### Működik

### Nem működik