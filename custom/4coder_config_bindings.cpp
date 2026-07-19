/*
4coder_config_bindings.cpp - Key maps for the ported Vim config.

Three per-mode maps (keys_vim_normal / _insert / _visual) are selected each
keystroke by vim_implicit_map. Insert inherits the default code map (so ordinary
4coder editing + power-user Ctrl/Cmd shortcuts + mouse keep working); normal and
visual inherit only the global map (no self-insert of text).
*/

#if !defined(FCODER_CONFIG_BINDINGS_CPP)
#define FCODER_CONFIG_BINDINGS_CPP

function void
setup_vim_normal_map(Mapping *mapping, i64 global_id, i64 normal_id){
    MappingScope();
    SelectMapping(mapping);

    SelectMap(normal_id);
    ParentMap(global_id);

    // mouse
    BindMouse(click_set_cursor_and_mark, MouseCode_Left);
    BindMouseRelease(click_set_cursor, MouseCode_Left);
    BindMouseMove(click_set_cursor_if_lbutton);

    // counts
    Bind(vim_count_digit, KeyCode_1);
    Bind(vim_count_digit, KeyCode_2);
    Bind(vim_count_digit, KeyCode_3);
    Bind(vim_count_digit, KeyCode_4);
    Bind(vim_count_digit, KeyCode_5);
    Bind(vim_count_digit, KeyCode_6);
    Bind(vim_count_digit, KeyCode_7);
    Bind(vim_count_digit, KeyCode_8);
    Bind(vim_count_digit, KeyCode_9);
    Bind(vim_zero, KeyCode_0);

    // motions
    Bind(vim_move, KeyCode_H);
    Bind(vim_move, KeyCode_J);
    Bind(vim_move, KeyCode_K);
    Bind(vim_move, KeyCode_L);
    Bind(vim_move, KeyCode_Left);
    Bind(vim_move, KeyCode_Down);
    Bind(vim_move, KeyCode_Up);
    Bind(vim_move, KeyCode_Right);
    Bind(vim_move, KeyCode_W);
    Bind(vim_move, KeyCode_W, KeyCode_Shift);
    Bind(vim_move, KeyCode_B);
    Bind(vim_move, KeyCode_E);
    Bind(vim_move, KeyCode_4, KeyCode_Shift); // $
    Bind(vim_move, KeyCode_6, KeyCode_Shift); // ^
    Bind(vim_move, KeyCode_5, KeyCode_Shift); // %
    Bind(vim_move, KeyCode_LeftBracket, KeyCode_Shift);  // {
    Bind(vim_move, KeyCode_RightBracket, KeyCode_Shift); // }
    Bind(vim_move, KeyCode_F);
    Bind(vim_move, KeyCode_F, KeyCode_Shift);
    Bind(vim_move, KeyCode_T);
    Bind(vim_move, KeyCode_T, KeyCode_Shift);
    Bind(vim_g_prefix, KeyCode_G);
    Bind(vim_goto_end, KeyCode_G, KeyCode_Shift);

    // mode transitions
    Bind(vim_insert_mode, KeyCode_I);
    Bind(vim_insert_line_start, KeyCode_I, KeyCode_Shift);
    Bind(vim_insert_append, KeyCode_A);
    Bind(vim_insert_append_eol, KeyCode_A, KeyCode_Shift);
    Bind(vim_open_below, KeyCode_O);
    Bind(vim_open_above, KeyCode_O, KeyCode_Shift);
    Bind(vim_visual_mode, KeyCode_V);
    Bind(vim_visual_line_mode, KeyCode_V, KeyCode_Shift);

    // operators & edits
    Bind(vim_delete, KeyCode_D);
    Bind(vim_delete_eol, KeyCode_D, KeyCode_Shift);
    Bind(vim_change, KeyCode_C);
    Bind(vim_change_eol, KeyCode_C, KeyCode_Shift);
    Bind(vim_yank, KeyCode_Y);
    Bind(vim_yank_line, KeyCode_Y, KeyCode_Shift);
    Bind(vim_delete_char, KeyCode_X);
    Bind(vim_delete_char_back, KeyCode_X, KeyCode_Shift);
    Bind(vim_substitute_char, KeyCode_S);
    Bind(vim_substitute_line, KeyCode_S, KeyCode_Shift);
    Bind(vim_replace_char, KeyCode_R);
    Bind(vim_join_lines, KeyCode_J, KeyCode_Shift);
    Bind(vim_paste_after, KeyCode_P);
    Bind(vim_paste_before, KeyCode_P, KeyCode_Shift);

    // history / search
    Bind(undo, KeyCode_U);
    Bind(redo, KeyCode_R, KeyCode_Control);
    Bind(search, KeyCode_ForwardSlash);
    Bind(goto_next_jump, KeyCode_N);
    Bind(goto_prev_jump, KeyCode_N, KeyCode_Shift);
    Bind(command_lister, KeyCode_Semicolon, KeyCode_Shift); // ':'

    // save (route through vim_write so format-on-save can hook in)
    Bind(vim_write, KeyCode_S, KeyCode_Control);
#if OS_MAC
    Bind(vim_write, KeyCode_S, KeyCode_Command);
#endif

    // window navigation (C-h/j/k/l) + splits
    Bind(change_active_panel_backwards, KeyCode_H, KeyCode_Control);
    Bind(change_active_panel,           KeyCode_J, KeyCode_Control);
    Bind(change_active_panel_backwards, KeyCode_K, KeyCode_Control);
    Bind(change_active_panel,           KeyCode_L, KeyCode_Control);

    // leader
    Bind(vim_leader, KeyCode_Space);

    Bind(vim_normal_mode, KeyCode_Escape);
}

function void
setup_vim_visual_map(Mapping *mapping, i64 global_id, i64 visual_id){
    MappingScope();
    SelectMapping(mapping);

    SelectMap(visual_id);
    ParentMap(global_id);

    BindMouse(click_set_cursor_and_mark, MouseCode_Left);
    BindMouseRelease(click_set_cursor, MouseCode_Left);
    BindMouseMove(click_set_cursor_if_lbutton);

    // counts
    Bind(vim_count_digit, KeyCode_1);
    Bind(vim_count_digit, KeyCode_2);
    Bind(vim_count_digit, KeyCode_3);
    Bind(vim_count_digit, KeyCode_4);
    Bind(vim_count_digit, KeyCode_5);
    Bind(vim_count_digit, KeyCode_6);
    Bind(vim_count_digit, KeyCode_7);
    Bind(vim_count_digit, KeyCode_8);
    Bind(vim_count_digit, KeyCode_9);
    Bind(vim_zero, KeyCode_0);

    // motions (extend selection)
    Bind(vim_move, KeyCode_H);
    Bind(vim_move, KeyCode_J);
    Bind(vim_move, KeyCode_K);
    Bind(vim_move, KeyCode_L);
    Bind(vim_move, KeyCode_Left);
    Bind(vim_move, KeyCode_Down);
    Bind(vim_move, KeyCode_Up);
    Bind(vim_move, KeyCode_Right);
    Bind(vim_move, KeyCode_W);
    Bind(vim_move, KeyCode_W, KeyCode_Shift);
    Bind(vim_move, KeyCode_B);
    Bind(vim_move, KeyCode_E);
    Bind(vim_move, KeyCode_4, KeyCode_Shift);
    Bind(vim_move, KeyCode_6, KeyCode_Shift);
    Bind(vim_move, KeyCode_5, KeyCode_Shift);
    Bind(vim_move, KeyCode_LeftBracket, KeyCode_Shift);
    Bind(vim_move, KeyCode_RightBracket, KeyCode_Shift);
    Bind(vim_move, KeyCode_F);
    Bind(vim_move, KeyCode_F, KeyCode_Shift);
    Bind(vim_move, KeyCode_T);
    Bind(vim_move, KeyCode_T, KeyCode_Shift);
    Bind(vim_g_prefix, KeyCode_G);
    Bind(vim_goto_end, KeyCode_G, KeyCode_Shift);

    // operators on the selection
    Bind(vim_visual_delete, KeyCode_D);
    Bind(vim_visual_delete, KeyCode_X);
    Bind(vim_visual_change, KeyCode_C);
    Bind(vim_visual_change, KeyCode_S);
    Bind(vim_visual_yank,   KeyCode_Y);
    Bind(vim_visual_swap,   KeyCode_O);

    Bind(vim_normal_mode, KeyCode_Escape);
}

function void
setup_vim_insert_map(Mapping *mapping, i64 code_id, i64 insert_id){
    MappingScope();
    SelectMapping(mapping);

    SelectMap(insert_id);
    ParentMap(code_id); // inherit text input, editing, mouse, power bindings

    Bind(vim_normal_mode, KeyCode_Escape);
    // Neovim: insert-mode <C-H> is mapped to <C-W> (delete word backwards)
    Bind(vim_insert_delete_word, KeyCode_H, KeyCode_Control);
    Bind(vim_insert_delete_word, KeyCode_Backspace, KeyCode_Control);

    // auto-pairs (port of mini.pairs)
    Bind(vim_pair_paren,   KeyCode_9, KeyCode_Shift);            // (
    Bind(vim_pair_bracket, KeyCode_LeftBracket);                // [
    Bind(vim_pair_brace,   KeyCode_LeftBracket, KeyCode_Shift); // {
    Bind(vim_pair_dquote,  KeyCode_Quote, KeyCode_Shift);       // "
    Bind(vim_pair_squote,  KeyCode_Quote);                      // '

    // save (route through vim_write so format-on-save can hook in)
    Bind(vim_write, KeyCode_S, KeyCode_Control);
#if OS_MAC
    Bind(vim_write, KeyCode_S, KeyCode_Command);
#endif
}

// Install the three Vim maps on top of 4coder's freshly-built mapping.
function void
setup_vim_maps(void){
    String_ID global_map_id = vars_save_string_lit("keys_global");
    String_ID code_map_id   = vars_save_string_lit("keys_code");
    String_ID normal_id = vars_save_string_lit("keys_vim_normal");
    String_ID insert_id = vars_save_string_lit("keys_vim_insert");
    String_ID visual_id = vars_save_string_lit("keys_vim_visual");
    setup_vim_normal_map(&framework_mapping, global_map_id, normal_id);
    setup_vim_visual_map(&framework_mapping, global_map_id, visual_id);
    setup_vim_insert_map(&framework_mapping, code_map_id, insert_id);
}

// Stock default_startup plus setup_vim_maps() after default_4coder_initialize.
CUSTOM_COMMAND_SIG(vim_startup)
CUSTOM_DOC("Vim custom-layer startup: default startup + install Vim key maps")
{
    ProfileScope(app, "vim startup");
    User_Input input = get_current_input(app);
    if (match_core_code(&input, CoreCode_Startup)){
        String_Const_u8_Array file_names = input.event.core.file_names;
        load_themes_default_folder(app);
        default_4coder_initialize(app, file_names);
        setup_vim_maps();
        default_4coder_side_by_side_panels(app, file_names);
        b32 auto_load = def_get_config_b32(vars_save_string_lit("automatically_load_project"));
        if (auto_load){
            load_project(app);
        }
    }

    {
        def_audio_init();
    }

    {
        def_enable_virtual_whitespace = def_get_config_b32(vars_save_string_lit("enable_virtual_whitespace"));
        clear_all_layouts(app);
    }
}

#endif //FCODER_CONFIG_BINDINGS_CPP
