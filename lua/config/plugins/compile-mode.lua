vim.g.compile_mode = {
    bang_expansion = true,
    default_command = {
        [""] = "make -k ",
        cpp = "g++ -std=c++23 -o %:r % && ./%:r",
        c = "gcc -o %:r % && ./%:r",
    },
}
