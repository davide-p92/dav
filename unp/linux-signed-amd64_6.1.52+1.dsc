-----BEGIN PGP SIGNED MESSAGE-----
Hash: SHA512

Format: 3.0 (native)
Source: linux-signed-amd64
Binary: kernel-image-6.1.0-12-amd64-di, nic-modules-6.1.0-12-amd64-di, nic-wireless-modules-6.1.0-12-amd64-di, nic-shared-modules-6.1.0-12-amd64-di, serial-modules-6.1.0-12-amd64-di, usb-serial-modules-6.1.0-12-amd64-di, ppp-modules-6.1.0-12-amd64-di, pata-modules-6.1.0-12-amd64-di, cdrom-core-modules-6.1.0-12-amd64-di, firewire-core-modules-6.1.0-12-amd64-di, scsi-core-modules-6.1.0-12-amd64-di, scsi-modules-6.1.0-12-amd64-di, scsi-nic-modules-6.1.0-12-amd64-di, loop-modules-6.1.0-12-amd64-di, btrfs-modules-6.1.0-12-amd64-di, ext4-modules-6.1.0-12-amd64-di, isofs-modules-6.1.0-12-amd64-di, jfs-modules-6.1.0-12-amd64-di, xfs-modules-6.1.0-12-amd64-di, fat-modules-6.1.0-12-amd64-di, squashfs-modules-6.1.0-12-amd64-di, udf-modules-6.1.0-12-amd64-di, fuse-modules-6.1.0-12-amd64-di, f2fs-modules-6.1.0-12-amd64-di, md-modules-6.1.0-12-amd64-di, multipath-modules-6.1.0-12-amd64-di, usb-modules-6.1.0-12-amd64-di, usb-storage-modules-6.1.0-12-amd64-di,
 pcmcia-storage-modules-6.1.0-12-amd64-di, fb-modules-6.1.0-12-amd64-di, input-modules-6.1.0-12-amd64-di, event-modules-6.1.0-12-amd64-di, mouse-modules-6.1.0-12-amd64-di, nic-pcmcia-modules-6.1.0-12-amd64-di, pcmcia-modules-6.1.0-12-amd64-di, nic-usb-modules-6.1.0-12-amd64-di, sata-modules-6.1.0-12-amd64-di, acpi-modules-6.1.0-12-amd64-di, i2c-modules-6.1.0-12-amd64-di, crc-modules-6.1.0-12-amd64-di, crypto-modules-6.1.0-12-amd64-di, crypto-dm-modules-6.1.0-12-amd64-di, efi-modules-6.1.0-12-amd64-di, ata-modules-6.1.0-12-amd64-di, mmc-core-modules-6.1.0-12-amd64-di, mmc-modules-6.1.0-12-amd64-di, nbd-modules-6.1.0-12-amd64-di, speakup-modules-6.1.0-12-amd64-di, uinput-modules-6.1.0-12-amd64-di, sound-modules-6.1.0-12-amd64-di, mtd-core-modules-6.1.0-12-amd64-di, rfkill-modules-6.1.0-12-amd64-di, linux-image-6.1.0-12-amd64, linux-image-amd64, linux-headers-amd64, linux-image-6.1.0-12-cloud-amd64, linux-image-cloud-amd64, linux-headers-cloud-amd64,
 linux-image-6.1.0-12-rt-amd64, linux-image-rt-amd64,
 linux-headers-rt-amd64
Architecture: amd64
Version: 6.1.52+1
Maintainer: Debian Kernel Team <debian-kernel@lists.debian.org>
Uploaders: Bastian Blank <waldi@debian.org>, maximilian attems <maks@debian.org>, Ben Hutchings <benh@debian.org>, Salvatore Bonaccorso <carnil@debian.org>
Homepage: https://www.kernel.org/
Standards-Version: 4.2.0
Vcs-Browser: https://salsa.debian.org/kernel-team/linux
Vcs-Git: https://salsa.debian.org/kernel-team/linux.git
Build-Depends: debhelper-compat (= 12), dh-exec, python3:any, rsync, sbsigntool [amd64 arm64 i386], kernel-wedge (>= 2.102~), linux-support-6.1.0-12 (= 6.1.52-1), linux-image-6.1.0-12-amd64-unsigned (= 6.1.52-1) [amd64], linux-image-6.1.0-12-cloud-amd64-unsigned (= 6.1.52-1) [amd64], linux-image-6.1.0-12-rt-amd64-unsigned (= 6.1.52-1) [amd64]
Package-List:
 acpi-modules-6.1.0-12-amd64-di udeb debian-installer optional arch=amd64
 ata-modules-6.1.0-12-amd64-di udeb debian-installer optional arch=amd64
 btrfs-modules-6.1.0-12-amd64-di udeb debian-installer optional arch=amd64
 cdrom-core-modules-6.1.0-12-amd64-di udeb debian-installer standard arch=amd64
 crc-modules-6.1.0-12-amd64-di udeb debian-installer optional arch=amd64
 crypto-dm-modules-6.1.0-12-amd64-di udeb debian-installer optional arch=amd64
 crypto-modules-6.1.0-12-amd64-di udeb debian-installer optional arch=amd64
 efi-modules-6.1.0-12-amd64-di udeb debian-installer optional arch=amd64
 event-modules-6.1.0-12-amd64-di udeb debian-installer optional arch=amd64
 ext4-modules-6.1.0-12-amd64-di udeb debian-installer standard arch=amd64
 f2fs-modules-6.1.0-12-amd64-di udeb debian-installer optional arch=amd64
 fat-modules-6.1.0-12-amd64-di udeb debian-installer standard arch=amd64
 fb-modules-6.1.0-12-amd64-di udeb debian-installer optional arch=amd64
 firewire-core-modules-6.1.0-12-amd64-di udeb debian-installer standard arch=amd64
 fuse-modules-6.1.0-12-amd64-di udeb debian-installer optional arch=amd64
 i2c-modules-6.1.0-12-amd64-di udeb debian-installer optional arch=amd64
 input-modules-6.1.0-12-amd64-di udeb debian-installer optional arch=amd64
 isofs-modules-6.1.0-12-amd64-di udeb debian-installer standard arch=amd64
 jfs-modules-6.1.0-12-amd64-di udeb debian-installer standard arch=amd64
 kernel-image-6.1.0-12-amd64-di udeb debian-installer standard arch=amd64
 linux-headers-amd64 deb kernel optional arch=amd64
 linux-headers-cloud-amd64 deb kernel optional arch=amd64
 linux-headers-rt-amd64 deb kernel optional arch=amd64
 linux-image-6.1.0-12-amd64 deb kernel optional arch=amd64
 linux-image-6.1.0-12-cloud-amd64 deb kernel optional arch=amd64
 linux-image-6.1.0-12-rt-amd64 deb kernel optional arch=amd64
 linux-image-amd64 deb kernel optional arch=amd64
 linux-image-cloud-amd64 deb kernel optional arch=amd64
 linux-image-rt-amd64 deb kernel optional arch=amd64
 loop-modules-6.1.0-12-amd64-di udeb debian-installer standard arch=amd64
 md-modules-6.1.0-12-amd64-di udeb debian-installer optional arch=amd64
 mmc-core-modules-6.1.0-12-amd64-di udeb debian-installer optional arch=amd64
 mmc-modules-6.1.0-12-amd64-di udeb debian-installer optional arch=amd64
 mouse-modules-6.1.0-12-amd64-di udeb debian-installer optional arch=amd64
 mtd-core-modules-6.1.0-12-amd64-di udeb debian-installer optional arch=amd64
 multipath-modules-6.1.0-12-amd64-di udeb debian-installer optional arch=amd64
 nbd-modules-6.1.0-12-amd64-di udeb debian-installer optional arch=amd64
 nic-modules-6.1.0-12-amd64-di udeb debian-installer standard arch=amd64
 nic-pcmcia-modules-6.1.0-12-amd64-di udeb debian-installer standard arch=amd64
 nic-shared-modules-6.1.0-12-amd64-di udeb debian-installer standard arch=amd64
 nic-usb-modules-6.1.0-12-amd64-di udeb debian-installer standard arch=amd64
 nic-wireless-modules-6.1.0-12-amd64-di udeb debian-installer standard arch=amd64
 pata-modules-6.1.0-12-amd64-di udeb debian-installer standard arch=amd64
 pcmcia-modules-6.1.0-12-amd64-di udeb debian-installer standard arch=amd64
 pcmcia-storage-modules-6.1.0-12-amd64-di udeb debian-installer standard arch=amd64
 ppp-modules-6.1.0-12-amd64-di udeb debian-installer optional arch=amd64
 rfkill-modules-6.1.0-12-amd64-di udeb debian-installer optional arch=amd64
 sata-modules-6.1.0-12-amd64-di udeb debian-installer standard arch=amd64
 scsi-core-modules-6.1.0-12-amd64-di udeb debian-installer standard arch=amd64
 scsi-modules-6.1.0-12-amd64-di udeb debian-installer standard arch=amd64
 scsi-nic-modules-6.1.0-12-amd64-di udeb debian-installer optional arch=amd64
 serial-modules-6.1.0-12-amd64-di udeb debian-installer optional arch=amd64
 sound-modules-6.1.0-12-amd64-di udeb debian-installer optional arch=amd64
 speakup-modules-6.1.0-12-amd64-di udeb debian-installer optional arch=amd64
 squashfs-modules-6.1.0-12-amd64-di udeb debian-installer optional arch=amd64
 udf-modules-6.1.0-12-amd64-di udeb debian-installer optional arch=amd64
 uinput-modules-6.1.0-12-amd64-di udeb debian-installer optional arch=amd64
 usb-modules-6.1.0-12-amd64-di udeb debian-installer optional arch=amd64
 usb-serial-modules-6.1.0-12-amd64-di udeb debian-installer optional arch=amd64
 usb-storage-modules-6.1.0-12-amd64-di udeb debian-installer standard arch=amd64
 xfs-modules-6.1.0-12-amd64-di udeb debian-installer standard arch=amd64
Checksums-Sha1:
 4b984d9e6ff485a9207dfdf2a5e203434379eec1 3061624 linux-signed-amd64_6.1.52+1.tar.xz
Checksums-Sha256:
 4f9d073b3dce57b80d86697de36688af6385c91042829b62ad786069c50a070d 3061624 linux-signed-amd64_6.1.52+1.tar.xz
Files:
 89b5cc86ee00deb5f24acd87c3d76ee5 3061624 linux-signed-amd64_6.1.52+1.tar.xz

-----BEGIN PGP SIGNATURE-----

iQIzBAEBCgAdFiEEfKFfvHEI+gkU+E+di0FRiLdONzYFAmT5+WUACgkQi0FRiLdO
Nzax7g/8CLo8d1hWeAA1gObFAf/Siw5QBP6jynKV02QPq2j7FsD2hfpVDgesz94E
bTvQoh3DDKWW0yqmVUePO6KHtAGPFepYnBumPBdEBsK3VxzCa5N8lseiScNgW9BW
wtuQQeKFLckik5cbLbJMjhLCc4wH1eX0XukfjbpEs/1/gQO7iZ8/PtUzep2V3l/V
9AZafAZFZiYiBL57bAtjNKkX44s0DUEEUImr6P6fr4xcFtjz156iAw+JUgOj0fNN
Dpp0v71lByxXL5oJJEI//SyRRwIKkMCicAopknZrADeRtapfINRc1q1B7QSLyKtL
BaadG5yjwueRfeLqi5kqEH7ZU3zI1clJfRRXh9kvxjTYqQTNyhfx9biZnRCMnZLB
kUP+/9MRdVaTF3IOMVu1iFMXOIz7kLg2wWkXCMoid4OIJ7eHCJ1QuVq+Up611e7m
n6zpBvqG+KcxvvYYHEHSVOX3YJemfRNg33y1TgzlfkcCqpkZikJUB0dSEEXYFvoC
KdNF8EZz+JhKHSLoOVdfOjH64fwE78r6E8VnBILMU4wVRumtWCKxy594ir33Elot
g24dlu4ZyuLw+3+V066vvTrZxhSRQqyh3ylgQWKacLe25CZXqIE3pFTjaxtFjlGt
tpruX8oFMQGhpk8uHJ9RsUXwl23wwSdgQhk8zNe+wQALRkgUKoI=
=bexE
-----END PGP SIGNATURE-----
