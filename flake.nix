{
  description = "A flake for building thue";

  inputs.nixpkgs.url = github:NixOS/nixpkgs/nixos-unstable;

  outputs = { self, nixpkgs }: let

    allSystems = [ "x86_64-linux" "aarch64-linux" "i686-linux" "x86_64-darwin" "aarch64-darwin" ];

    forAllSystems = nixpkgs.lib.genAttrs allSystems;

    thue = system:
      with nixpkgs.legacyPackages.${system};
      stdenv.mkDerivation {
        pname = "thue";
        version = "0.0.1";
        src = ./src;
        doCheck = true;
      };

    thue-packages = system: {
      default = thue system;
    };

  in {

    packages = forAllSystems thue-packages;

  };
}
