{ pkgs? import <nixpkgs> {}}:
with pkgs;
gcc9Stdenv.mkDerivation {
    name = "waysig-env";
    buildInputs = [ cmake pkgconfig catch2 wayland extra-cmake-modules clang gdb ];
}
