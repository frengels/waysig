{ pkgs? import <nixpkgs> {}}:
with pkgs;
gcc9Stdenv.mkDerivation {
    name = "waysig-env";
    buildInputs = [ meson ninja pkgconfig catch2 wayland clang gdb ];
}
