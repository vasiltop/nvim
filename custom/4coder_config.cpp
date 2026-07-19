/*
4coder_config.cpp - Custom layer entry point for the ported Neovim config.

Builds a Vim-style modal editing experience on top of 4coder's default layer:
  - Flexoki (dark) theme
  - NORMAL / INSERT / VISUAL modes with motions, operators and counts
  - the repo's leader keymaps, window/split navigation, build and format commands

Build this file into custom_4coder.{so,dll,dylib} with the scripts in the repo
root (build-mac.sh / build-linux.sh / build.bat).
*/

// TOP

#if !defined(FCODER_CONFIG_CPP)
#define FCODER_CONFIG_CPP

#include "4coder_default_include.cpp"

// Our modules
#include "4coder_config_theme.cpp"
#include "4coder_config_vim.cpp"
#include "4coder_config_commands.cpp"
#include "4coder_config_bindings.cpp"

#if !defined(META_PASS)
#include "generated/managed_id_metadata.cpp"
#endif

void
custom_layer_init(Application_Links *app){
    Thread_Context *tctx = get_thread_context(app);

    // default framework + hooks
    default_framework_init(app);
    set_all_default_hooks(app);

    // route each keystroke through our per-mode map selector
    implicit_map_function = vim_implicit_map;

    // Flexoki color scheme
    set_flexoki_color_scheme(app);

    mapping_init(tctx, &framework_mapping);

    String_ID global_map_id = vars_save_string_lit("keys_global");
    String_ID file_map_id   = vars_save_string_lit("keys_file");
    String_ID code_map_id   = vars_save_string_lit("keys_code");

    // Keep 4coder's default maps working (mouse, text input, power shortcuts).
#if OS_MAC
    setup_mac_mapping(&framework_mapping, global_map_id, file_map_id, code_map_id);
#else
    setup_default_mapping(&framework_mapping, global_map_id, file_map_id, code_map_id);
#endif
    setup_essential_mapping(&framework_mapping, global_map_id, file_map_id, code_map_id);

    // Create the Vim maps now so keys_vim_normal / _insert / _visual EXIST for the
    // very first dispatched event. Every input (including the startup Core event) is
    // routed through vim_implicit_map, which selects one of these maps; the startup
    // command is found by walking that map's parent chain up to keys_global. If the
    // maps did not exist yet, the startup event would resolve to nothing and 4coder
    // would come up as a bare *scratch* buffer with no layout.
    setup_vim_maps();

    // Override the startup hook: vim_startup does everything default_startup does,
    // then reinstalls our Vim maps. default_4coder_initialize (run inside startup)
    // calls mapping_init, which zeroes the whole mapping and rebuilds only 4coder's
    // own maps; reinstalling afterwards is what keeps the Vim maps alive.
    {
        MappingScope();
        SelectMapping(&framework_mapping);
        SelectMap(global_map_id);
        BindCore(vim_startup, CoreCode_Startup);
    }
}

#endif //FCODER_CONFIG_CPP

// BOTTOM
