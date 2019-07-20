{ pkgs? import <nixpkgs> {}}:
with pkgs;
mkShell {
    buildInputs = [ meson ninja pkgconfig catch2 ];
}