{ nixpkgs ? import <nixpkgs> { } }:

with nixpkgs;
callPackage ./derivation.nix { }