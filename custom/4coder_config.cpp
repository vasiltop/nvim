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

    // Our Vim maps.
    String_ID normal_id = vars_save_string_lit("keys_vim_normal");
    String_ID insert_id = vars_save_string_lit("keys_vim_insert");
    String_ID visual_id = vars_save_string_lit("keys_vim_visual");
    setup_vim_normal_map(&framework_mapping, global_map_id, normal_id);
    setup_vim_visual_map(&framework_mapping, global_map_id, visual_id);
    setup_vim_insert_map(&framework_mapping, code_map_id, insert_id);
}

#endif //FCODER_CONFIG_CPP

// BOTTOM
