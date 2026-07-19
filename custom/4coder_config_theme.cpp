/*
4coder_config_theme.cpp - Flexoki (dark) color scheme ported from the Neovim
flexoki colorscheme. 4coder windows cannot be truly transparent, so the original
`transparent = true` maps to the flexoki black background instead.
*/

#if !defined(FCODER_CONFIG_THEME_CPP)
#define FCODER_CONFIG_THEME_CPP

// Flexoki dark palette (Steph Ango)
#define FLEXOKI_BLACK   0xFF100F0F
#define FLEXOKI_950     0xFF1C1B1A
#define FLEXOKI_900     0xFF282726
#define FLEXOKI_850     0xFF343331
#define FLEXOKI_800     0xFF403E3C
#define FLEXOKI_700     0xFF575653
#define FLEXOKI_600     0xFF6F6E69
#define FLEXOKI_500     0xFF878580
#define FLEXOKI_300     0xFFB7B5AC
#define FLEXOKI_200     0xFFCECDC3
#define FLEXOKI_RED     0xFFD14D41
#define FLEXOKI_ORANGE  0xFFDA702C
#define FLEXOKI_YELLOW  0xFFD0A215
#define FLEXOKI_GREEN   0xFF879A39
#define FLEXOKI_CYAN    0xFF3AA99F
#define FLEXOKI_BLUE    0xFF4385BE
#define FLEXOKI_PURPLE  0xFF8B7EC8
#define FLEXOKI_MAGENTA 0xFFCE5D97

function void
set_flexoki_color_scheme(Application_Links *app){
    if (global_theme_arena.base_allocator == 0){
        global_theme_arena = make_arena_system();
    }
    Arena *arena = &global_theme_arena;

    Color_Table table = make_color_table(app, arena);

    table.arrays[0]                             = make_colors(arena, FLEXOKI_200);
    table.arrays[defcolor_bar]                  = make_colors(arena, FLEXOKI_900);
    table.arrays[defcolor_base]                 = make_colors(arena, FLEXOKI_200);
    table.arrays[defcolor_pop1]                 = make_colors(arena, FLEXOKI_BLUE);
    table.arrays[defcolor_pop2]                 = make_colors(arena, FLEXOKI_RED);
    table.arrays[defcolor_back]                 = make_colors(arena, FLEXOKI_BLACK);
    table.arrays[defcolor_margin]               = make_colors(arena, FLEXOKI_950);
    table.arrays[defcolor_margin_hover]         = make_colors(arena, FLEXOKI_900);
    table.arrays[defcolor_margin_active]        = make_colors(arena, FLEXOKI_850);
    table.arrays[defcolor_list_item]            = make_colors(arena, FLEXOKI_950, FLEXOKI_BLACK);
    table.arrays[defcolor_list_item_hover]      = make_colors(arena, FLEXOKI_900, FLEXOKI_950);
    table.arrays[defcolor_list_item_active]     = make_colors(arena, FLEXOKI_850, FLEXOKI_850);
    table.arrays[defcolor_cursor]               = make_colors(arena, FLEXOKI_GREEN, FLEXOKI_ORANGE);
    table.arrays[defcolor_at_cursor]            = make_colors(arena, FLEXOKI_BLACK);
    table.arrays[defcolor_highlight_cursor_line]= make_colors(arena, FLEXOKI_950);
    table.arrays[defcolor_highlight]            = make_colors(arena, FLEXOKI_YELLOW);
    table.arrays[defcolor_at_highlight]         = make_colors(arena, FLEXOKI_BLACK);
    table.arrays[defcolor_mark]                 = make_colors(arena, FLEXOKI_800);
    table.arrays[defcolor_text_default]         = make_colors(arena, FLEXOKI_200);
    table.arrays[defcolor_comment]              = make_colors(arena, FLEXOKI_500);
    table.arrays[defcolor_comment_pop]          = make_colors(arena, FLEXOKI_GREEN, FLEXOKI_RED);
    table.arrays[defcolor_keyword]              = make_colors(arena, FLEXOKI_GREEN);
    table.arrays[defcolor_str_constant]         = make_colors(arena, FLEXOKI_CYAN);
    table.arrays[defcolor_char_constant]        = make_colors(arena, FLEXOKI_CYAN);
    table.arrays[defcolor_int_constant]         = make_colors(arena, FLEXOKI_ORANGE);
    table.arrays[defcolor_float_constant]       = make_colors(arena, FLEXOKI_ORANGE);
    table.arrays[defcolor_bool_constant]        = make_colors(arena, FLEXOKI_ORANGE);
    table.arrays[defcolor_preproc]              = make_colors(arena, FLEXOKI_PURPLE);
    table.arrays[defcolor_include]              = make_colors(arena, FLEXOKI_CYAN);
    table.arrays[defcolor_special_character]    = make_colors(arena, FLEXOKI_RED);
    table.arrays[defcolor_ghost_character]      = make_colors(arena, FLEXOKI_700);
    table.arrays[defcolor_highlight_junk]       = make_colors(arena, 0xFF3A0F0C);
    table.arrays[defcolor_highlight_white]      = make_colors(arena, 0xFF003A3A);
    table.arrays[defcolor_paste]                = make_colors(arena, FLEXOKI_YELLOW);
    table.arrays[defcolor_undo]                 = make_colors(arena, FLEXOKI_CYAN);
    table.arrays[defcolor_back_cycle]           = make_colors(arena, FLEXOKI_950, FLEXOKI_900, FLEXOKI_850, FLEXOKI_800);
    table.arrays[defcolor_text_cycle]           = make_colors(arena, FLEXOKI_RED, FLEXOKI_GREEN, FLEXOKI_BLUE, FLEXOKI_YELLOW);
    table.arrays[defcolor_line_numbers_back]    = make_colors(arena, FLEXOKI_BLACK);
    table.arrays[defcolor_line_numbers_text]    = make_colors(arena, FLEXOKI_600);

    save_theme(table, string_u8_litexpr("flexoki"));
    active_color_table = table;
}

#endif //FCODER_CONFIG_THEME_CPP
