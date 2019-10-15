pkgname=waysig-git
_pkgname=waysig
pkgver=77.6fda9ad
pkgrel=1
pkgdesc="C++ signal/slot implementation based on wl_signal/listener"
arch=('any')
url="https://github.com/dreyri/waysig"
license=('MIT')
makedepends=('cmake' 'ninja' 'git' 'catch2' 'wayland')
provides=("$_pkgname")
conflicts=("$_pkgname")
_branch="waysig"
source=('git+https://github.com/dreyri/waysig.git')
md5sums=('SKIP')

pkgver() {
    cd "$_branch"
    echo $(git rev-list --count HEAD).$(git rev-parse --short HEAD)
}

prepare() {
    if [ -d _build ]; then
        rm -rf _build
    fi
    mkdir _build
}

build() {
    cd "_build"
    cmake $srcdir/$_branch -G Ninja \
        -D CMAKE_CXX_FLAGS="${CXXFLAGS}" \
        -D CMAKE_BUILD_TYPE=Release \
        -D CMAKE_INSTALL_PREFIX=/usr \
        -D WAYSIG_BUILD_TESTS=ON \
        -D WAYSIG_BUILD_TESTS_WAYLAND=ON
    ninja
}

check() {
    cd "_build"
    ninja test
}

package() {
    cd "_build"
    DESTDIR="$pkgdir" ninja install

    install -Dm644 "$srcdir/$_branch/LICENSE" \
        "$pkgdir/usr/share/licenses/waysig-git/LICENSE"
}