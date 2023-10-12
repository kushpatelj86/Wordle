{ pkgs }:

let llvmPackages = pkgs.llvmPackages_12;

in {
	deps = with pkgs; [
    clang-tools
    clang_12
    lsb-release
		llvmPackages.clang
		ccls
		gdb
		gnumake
    asio
    nodejs-16_x
    nodePackages.typescript
    nodePackages.typescript-language-server
    cmake
    gtest
    llvm
    nlohmann_json
	];
}