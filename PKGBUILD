pkgname=luvdot
pkgver=1.0.0
pkgrel=1
pkgdesc=""
arch=('x86_64')
license=('GPL-3.0-only')
depends=('opencv' 'lua')
makedepends=('cmake' 'make' 'gcc')
source=("git+https://github.com/arlirad/luvdot")
sha256sums=('SKIP')

build() {
    cmake -B luvdot/build -S luvdot/. -DCMAKE_BUILD_TYPE=Release
    cmake --build luvdot/build
}

package() {
    install -Dm755 luvdot/build/luvdot "$pkgdir/usr/bin/luvdot"
}
