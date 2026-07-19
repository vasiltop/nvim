# 4coder config

A [4coder](https://github.com/4coder-archive/4coder) custom layer that ports this
repository's former Neovim configuration: a Vim-style modal editor (normal /
insert / visual modes, motions, operators, counts, registers), the leader
keymaps, the build/compile workflow, format-on-save, and the Flexoki color
scheme — all as a compiled C++ layer (`custom_4coder.{so,dll,dylib}`).

4coder's "configuration" is not a script that is interpreted at runtime; it is a
shared library built against 4coder's API headers and loaded by the editor at
startup. Everything here lives in [`custom/`](custom/) and is compiled with the
scripts in the repo root.

## Layout

| File | Purpose |
|---|---|
| `custom/4coder_config.cpp` | Entry point: `custom_layer_init`, hook + mapping wiring. |
| `custom/4coder_config_vim.cpp` | Modal engine: per-view mode, motions, operator-pending, register. |
| `custom/4coder_config_commands.cpp` | All bound commands, leader dispatch, build, format, auto-pairs. |
| `custom/4coder_config_bindings.cpp` | The normal / insert / visual key maps. |
| `custom/4coder_config_theme.cpp` | Flexoki (dark) theme. |
| `config.4coder` | User options (indentation, line numbers, wrapping…). |
| `build-mac.sh` / `build-linux.sh` / `build.bat` | Build the layer against a 4coder distribution. |

## Building

You need a 4coder source tree / distribution — specifically its `code/custom`
directory, which ships `4coder_default_include.cpp`,
`4coder_metadata_generator.cpp` and the `generated/` headers.

```sh
# macOS
./build-mac.sh /path/to/4coder/code/custom
# Linux
./build-linux.sh /path/to/4coder/code/custom
# Windows (from an x64 developer command prompt)
build.bat C:\path\to\4coder\code\custom
```

Each script writes `custom_4coder.{so,dll}` into `./build/` (override with a
second argument, or set `FCODER_CUSTOM` instead of passing the path). The build
runs the same three steps 4coder's own `buildsuper` scripts do: preprocess with
`-DMETA_PASS`, run the metadata generator, then compile the layer as a shared
library.

### Installing

4coder loads `custom_4coder.*` and `config.4coder` from its working directory
(and a couple of fallback locations). The simplest setup:

```sh
cp build/custom_4coder.so /path/to/4coder/custom_4coder.so
cp config.4coder          /path/to/4coder/config.4coder
```

then launch `4ed` from that directory.

### Building 4coder itself

4coder is open source but its build is unmaintained, so a current toolchain
needs a couple of patches. On **arm64 macOS** (Homebrew clang) two edits to the
4coder tree are required before `bin/build-mac.sh` succeeds:

1. `code/bin/4ed_build.cpp` — replace the bundled x86 `libfreetype-mac.a` link
   with Homebrew's arm64 freetype:
   `-L/opt/homebrew/opt/freetype/lib -lfreetype`.
2. `code/custom/4coder_audio.cpp` — under `__aarch64__`, skip `<immintrin.h>`
   and define `_mm_pause()` as `__asm__ __volatile__("yield")` (only
   `_InterlockedExchangeAdd` and `_mm_pause` are actually used).

After that, `cd code && bash bin/build-mac.sh` produces `4ed`. Copy the runtime
assets once (`4coder-non-source/dist_files/fonts` → `build/fonts`), drop this
repo's `config.4coder` and the built `custom_4coder.so` next to `4ed`, and run
it.

## Keys

Modes: **Esc** → normal, `i`/`I`/`a`/`A`/`o`/`O` → insert, `v`/`V` → visual.

**Motions** (also usable as operator targets): `h j k l`, `w W b e`, `0 ^ $`,
`gg G`, `{ }`, `%`, `f F t T`, plus counts (`3w`, `d2j`, …).

**Operators / edits**: `d c y` (with doubled `dd cc yy` and `D C Y`), `x X`,
`s S`, `r`, `J`, `p P`, `u` / `Ctrl-r` (undo/redo). A single unnamed,
linewise-aware register drives `p`/`P` and also posts to the system clipboard.

**Leader** is `Space`:

| Keys | Action | Original |
|---|---|---|
| `Space e` | open / create file | `<Space>e` (`:Ex`) |
| `Space h` / `Space v` | horizontal / vertical split | `:split` / `:vsplit` |
| `Space p f` | open file | mini.pick files |
| `Space p g` | search in files | mini.pick grep |
| `Space p b` | switch buffer | mini.pick buffers |
| `Space p h` | command list | mini.pick help |
| `Space p e` | previous error | `:PrevError` |
| `Space n e` | next error | `:NextError` |
| `Space r c` | build / run current file | `:Recompile` |
| `Space c f` | format buffer | `<leader>cf` |

Other bindings: `Ctrl-h/j/k/l` move between panels, `/` searches, `n`/`N`
jump to next/previous match, `:` opens the command list, `gd` jumps to
definition. In insert mode `Ctrl-w` / `Ctrl-h` delete the previous word and
`( [ { " '` auto-insert their closing pair (mini.pairs).

**Build**: for `.cpp/.cc/.cxx` runs `g++ -std=c++23 -o <stem> <file> && ./<stem>`,
for `.c` runs `gcc -o <stem> <file> && ./<stem>`, otherwise `make -k` — into the
`*compilation*` footer, with `Space n e` / `Space p e` walking the errors
(ported from compile-mode.nvim).

**Format on save**: run the `vim_format_toggle` command (via `:` / the command
list) to toggle it. When on, saving runs `clang-format -i` (C/C++) or `black`
(Python) and reloads the buffer. `Space c f` formats on demand. Ported from the
null-ls + `:FormatToggle` setup.

## What changed from the Neovim config

Kept, mapped onto 4coder built-ins where one was close enough (parity is not
1:1 by design):

- Modal editing, counts, registers → the custom engine.
- Options (`tabstop`/`shiftwidth` = 2, line numbers, no wrap, autoindent) →
  `config.4coder`.
- Pickers (mini.pick) → native listers.
- compile-mode → native build system + `*compilation*` + error navigation.
- Window splits/navigation → native panel commands (direction is approximated;
  4coder cycles panels rather than moving by compass direction).
- flexoki colorscheme → ported theme (4coder can't be transparent, so the
  original `transparent = true` becomes the Flexoki black background).
- mini.pairs → a minimal insert-mode auto-pair.

Dropped (no 4coder equivalent, and confirmed out of scope):

- LSP (clangd / rust_analyzer / pyright / vtsls / ts_ls / lua_ls / gdscript),
  Mason, and all LSP-driven keymaps (hover, rename, code action, diagnostics,
  `gi`/`gD`/`gt`).
- DAP / debugging and the Godot editor-server integration.
- lazydev, oil (file explorer), neogit / git, the `:Run` command,
  `:nohlsearch`, and `cpp_extract_definitions`.
- treesitter — replaced by 4coder's built-in C/C++ lexer + highlighting.
