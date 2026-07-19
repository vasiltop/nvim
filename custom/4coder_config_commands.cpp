/*
4coder_config_commands.cpp - Vim-style commands, leader dispatch, build and
format integration for the ported config.
*/

#if !defined(FCODER_CONFIG_COMMANDS_CPP)
#define FCODER_CONFIG_COMMANDS_CPP

#include <stdlib.h> // system()

global b32 vim_format_on_save = false;

////////////////////////////////
// Mode transitions

CUSTOM_COMMAND_SIG(vim_normal_mode)
CUSTOM_DOC("Enter Vim normal mode.")
{
    View_ID view = get_active_view(app, Access_ReadVisible);
    vim_pending_count = 0;
    vim_set_mode(app, view, VimMode_Normal);
}

CUSTOM_COMMAND_SIG(vim_insert_mode)
CUSTOM_DOC("Enter Vim insert mode at the cursor.")
{
    View_ID view = get_active_view(app, Access_ReadVisible);
    vim_set_mode(app, view, VimMode_Insert);
}

CUSTOM_COMMAND_SIG(vim_insert_append)
CUSTOM_DOC("Enter insert mode one character after the cursor (a).")
{
    View_ID view = get_active_view(app, Access_ReadWriteVisible);
    Buffer_ID buffer = view_get_buffer(app, view, Access_ReadWriteVisible);
    i64 pos = view_get_cursor_pos(app, view);
    i64 le = vim_line_end(app, buffer, pos);
    if (pos < le){ pos += 1; }
    view_set_cursor_and_preferred_x(app, view, seek_pos(pos));
    vim_set_mode(app, view, VimMode_Insert);
}

CUSTOM_COMMAND_SIG(vim_insert_append_eol)
CUSTOM_DOC("Enter insert mode at end of line (A).")
{
    View_ID view = get_active_view(app, Access_ReadWriteVisible);
    Buffer_ID buffer = view_get_buffer(app, view, Access_ReadWriteVisible);
    i64 pos = vim_line_end(app, buffer, view_get_cursor_pos(app, view));
    view_set_cursor_and_preferred_x(app, view, seek_pos(pos));
    vim_set_mode(app, view, VimMode_Insert);
}

CUSTOM_COMMAND_SIG(vim_insert_line_start)
CUSTOM_DOC("Enter insert mode at first non-blank (I).")
{
    View_ID view = get_active_view(app, Access_ReadWriteVisible);
    Buffer_ID buffer = view_get_buffer(app, view, Access_ReadWriteVisible);
    i64 pos = get_pos_past_lead_whitespace(app, buffer, view_get_cursor_pos(app, view));
    view_set_cursor_and_preferred_x(app, view, seek_pos(pos));
    vim_set_mode(app, view, VimMode_Insert);
}

CUSTOM_COMMAND_SIG(vim_open_below)
CUSTOM_DOC("Open a new line below and enter insert mode (o).")
{
    View_ID view = get_active_view(app, Access_ReadWriteVisible);
    Buffer_ID buffer = view_get_buffer(app, view, Access_ReadWriteVisible);
    i64 pos = vim_line_end(app, buffer, view_get_cursor_pos(app, view));
    buffer_replace_range(app, buffer, Ii64(pos, pos), string_u8_litexpr("\n"));
    view_set_cursor_and_preferred_x(app, view, seek_pos(pos + 1));
    vim_set_mode(app, view, VimMode_Insert);
    auto_indent_line_at_cursor(app);
}

CUSTOM_COMMAND_SIG(vim_open_above)
CUSTOM_DOC("Open a new line above and enter insert mode (O).")
{
    View_ID view = get_active_view(app, Access_ReadWriteVisible);
    Buffer_ID buffer = view_get_buffer(app, view, Access_ReadWriteVisible);
    i64 pos = vim_line_start(app, buffer, view_get_cursor_pos(app, view));
    buffer_replace_range(app, buffer, Ii64(pos, pos), string_u8_litexpr("\n"));
    view_set_cursor_and_preferred_x(app, view, seek_pos(pos));
    vim_set_mode(app, view, VimMode_Insert);
    auto_indent_line_at_cursor(app);
}

CUSTOM_COMMAND_SIG(vim_visual_mode)
CUSTOM_DOC("Enter Vim visual mode (v).")
{
    View_ID view = get_active_view(app, Access_ReadVisible);
    i64 pos = view_get_cursor_pos(app, view);
    view_set_mark(app, view, seek_pos(pos));
    vim_set_mode(app, view, VimMode_Visual);
}

CUSTOM_COMMAND_SIG(vim_visual_line_mode)
CUSTOM_DOC("Enter Vim visual line mode (V).")
{
    View_ID view = get_active_view(app, Access_ReadVisible);
    i64 pos = view_get_cursor_pos(app, view);
    view_set_mark(app, view, seek_pos(pos));
    vim_set_mode(app, view, VimMode_VisualLine);
}

////////////////////////////////
// Motions

CUSTOM_COMMAND_SIG(vim_move)
CUSTOM_DOC("Apply the Vim motion bound to the triggering key.")
{
    User_Input in = get_current_input(app);
    Key_Code code = in.event.key.code;
    Input_Modifier_Set mods = in.event.key.modifiers;
    View_ID view = get_active_view(app, Access_ReadWriteVisible);
    Buffer_ID buffer = view_get_buffer(app, view, Access_ReadWriteVisible);
    i32 count = vim_take_count();

    // vertical motions use 4coder's preferred-x aware movement
    if (code == KeyCode_J || code == KeyCode_Down){
        move_vertical_lines(app, view, (i64)count);
        return;
    }
    if (code == KeyCode_K || code == KeyCode_Up){
        move_vertical_lines(app, view, -(i64)count);
        return;
    }

    i64 start = view_get_cursor_pos(app, view);
    Vim_Motion_Result m = vim_compute_motion(app, view, buffer, start, code, mods, count, false);
    if (m.valid){
        view_set_cursor_and_preferred_x(app, view, seek_pos(m.pos));
    }
}

CUSTOM_COMMAND_SIG(vim_zero)
CUSTOM_DOC("Move to start of line, or extend a pending count with 0.")
{
    if (vim_pending_count > 0){
        vim_pending_count *= 10;
        return;
    }
    View_ID view = get_active_view(app, Access_ReadVisible);
    Buffer_ID buffer = view_get_buffer(app, view, Access_ReadVisible);
    i64 pos = vim_line_start(app, buffer, view_get_cursor_pos(app, view));
    view_set_cursor_and_preferred_x(app, view, seek_pos(pos));
}

CUSTOM_COMMAND_SIG(vim_count_digit)
CUSTOM_DOC("Accumulate a numeric count (1-9).")
{
    User_Input in = get_current_input(app);
    i32 d = (i32)(in.event.key.code - KeyCode_0);
    if (d >= 0 && d <= 9){
        vim_pending_count = vim_pending_count*10 + d;
    }
}

CUSTOM_COMMAND_SIG(vim_g_prefix)
CUSTOM_DOC("Handle the g-prefixed commands (gg, gd).")
{
    User_Input in = get_next_input(app, EventProperty_AnyKey, EventProperty_Escape);
    if (in.abort || in.event.kind != InputEventKind_KeyStroke){
        return;
    }
    Key_Code code = in.event.key.code;
    if (code == KeyCode_G){
        View_ID view = get_active_view(app, Access_ReadVisible);
        Buffer_ID buffer = view_get_buffer(app, view, Access_ReadVisible);
        i64 line = (vim_pending_count > 0) ? vim_take_count() : 1;
        i64 pos = get_line_side_pos(app, buffer, line, Side_Min);
        view_set_cursor_and_preferred_x(app, view, seek_pos(pos));
    }
    else if (code == KeyCode_D){
        jump_to_definition_at_cursor(app);
    }
}

CUSTOM_COMMAND_SIG(vim_goto_end)
CUSTOM_DOC("Go to the last line, or count'th line (G).")
{
    View_ID view = get_active_view(app, Access_ReadVisible);
    Buffer_ID buffer = view_get_buffer(app, view, Access_ReadVisible);
    i64 pos = 0;
    if (vim_pending_count > 0){
        pos = get_line_side_pos(app, buffer, vim_take_count(), Side_Min);
    }
    else{
        pos = buffer_get_size(app, buffer);
    }
    view_set_cursor_and_preferred_x(app, view, seek_pos(pos));
}

////////////////////////////////
// Operators

CUSTOM_COMMAND_SIG(vim_delete)
CUSTOM_DOC("Delete operator (d).")
{
    vim_operator_pending(app, VimOp_Delete, KeyCode_D);
}

CUSTOM_COMMAND_SIG(vim_change)
CUSTOM_DOC("Change operator (c).")
{
    vim_operator_pending(app, VimOp_Change, KeyCode_C);
}

CUSTOM_COMMAND_SIG(vim_yank)
CUSTOM_DOC("Yank operator (y).")
{
    vim_operator_pending(app, VimOp_Yank, KeyCode_Y);
}

function void
vim_to_eol_operator(Application_Links *app, Vim_Operator op){
    View_ID view = get_active_view(app, Access_ReadWriteVisible);
    Buffer_ID buffer = view_get_buffer(app, view, Access_ReadWriteVisible);
    i64 pos = view_get_cursor_pos(app, view);
    i64 le = vim_line_end(app, buffer, pos);
    vim_apply_operator(app, view, buffer, op, Ii64(pos, le), false);
}

CUSTOM_COMMAND_SIG(vim_delete_eol)
CUSTOM_DOC("Delete to end of line (D).")
{
    vim_to_eol_operator(app, VimOp_Delete);
}

CUSTOM_COMMAND_SIG(vim_change_eol)
CUSTOM_DOC("Change to end of line (C).")
{
    vim_to_eol_operator(app, VimOp_Change);
}

CUSTOM_COMMAND_SIG(vim_yank_line)
CUSTOM_DOC("Yank the current line (Y).")
{
    View_ID view = get_active_view(app, Access_ReadWriteVisible);
    Buffer_ID buffer = view_get_buffer(app, view, Access_ReadWriteVisible);
    i64 pos = view_get_cursor_pos(app, view);
    i32 count = vim_take_count();
    i64 end_line = get_line_number_from_pos(app, buffer, pos) + (count - 1);
    i64 end_pos = get_line_side_pos(app, buffer, end_line, Side_Min);
    Range_i64 range = vim_linewise_range(app, buffer, pos, end_pos);
    vim_apply_operator(app, view, buffer, VimOp_Yank, range, true);
}

CUSTOM_COMMAND_SIG(vim_delete_char)
CUSTOM_DOC("Delete character(s) under the cursor (x).")
{
    View_ID view = get_active_view(app, Access_ReadWriteVisible);
    Buffer_ID buffer = view_get_buffer(app, view, Access_ReadWriteVisible);
    i64 pos = view_get_cursor_pos(app, view);
    i64 le = vim_line_end(app, buffer, pos);
    i64 end = clamp_top(pos + vim_take_count(), le);
    if (end > pos){
        Scratch_Block scratch(app);
        String_Const_u8 text = push_buffer_range(app, scratch, buffer, Ii64(pos, end));
        vim_set_register(text, false);
        clipboard_post(0, text);
        buffer_replace_range(app, buffer, Ii64(pos, end), string_u8_empty);
    }
}

CUSTOM_COMMAND_SIG(vim_delete_char_back)
CUSTOM_DOC("Delete character(s) before the cursor (X).")
{
    View_ID view = get_active_view(app, Access_ReadWriteVisible);
    Buffer_ID buffer = view_get_buffer(app, view, Access_ReadWriteVisible);
    i64 pos = view_get_cursor_pos(app, view);
    i64 ls = vim_line_start(app, buffer, pos);
    i64 start = clamp_bot(ls, pos - vim_take_count());
    if (pos > start){
        buffer_replace_range(app, buffer, Ii64(start, pos), string_u8_empty);
        view_set_cursor_and_preferred_x(app, view, seek_pos(start));
    }
}

CUSTOM_COMMAND_SIG(vim_substitute_char)
CUSTOM_DOC("Delete character under cursor and enter insert mode (s).")
{
    View_ID view = get_active_view(app, Access_ReadWriteVisible);
    Buffer_ID buffer = view_get_buffer(app, view, Access_ReadWriteVisible);
    i64 pos = view_get_cursor_pos(app, view);
    i64 le = vim_line_end(app, buffer, pos);
    i64 end = clamp_top(pos + vim_take_count(), le);
    if (end > pos){
        buffer_replace_range(app, buffer, Ii64(pos, end), string_u8_empty);
    }
    vim_set_mode(app, view, VimMode_Insert);
}

CUSTOM_COMMAND_SIG(vim_substitute_line)
CUSTOM_DOC("Change the whole line (S).")
{
    View_ID view = get_active_view(app, Access_ReadWriteVisible);
    Buffer_ID buffer = view_get_buffer(app, view, Access_ReadWriteVisible);
    i64 pos = view_get_cursor_pos(app, view);
    Range_i64 line = get_line_pos_range(app, buffer, get_line_number_from_pos(app, buffer, pos));
    i64 indent = get_pos_past_lead_whitespace(app, buffer, line.start);
    buffer_replace_range(app, buffer, Ii64(indent, line.end), string_u8_empty);
    view_set_cursor_and_preferred_x(app, view, seek_pos(indent));
    vim_set_mode(app, view, VimMode_Insert);
}

CUSTOM_COMMAND_SIG(vim_replace_char)
CUSTOM_DOC("Replace the character under the cursor (r).")
{
    View_ID view = get_active_view(app, Access_ReadWriteVisible);
    Buffer_ID buffer = view_get_buffer(app, view, Access_ReadWriteVisible);
    u8 ch = vim_read_char_key(app);
    if (ch == 0){ return; }
    i64 pos = view_get_cursor_pos(app, view);
    i64 le = vim_line_end(app, buffer, pos);
    if (pos < le){
        u8 buf[1]; buf[0] = ch;
        String_Const_u8 s = SCu8(buf, (u64)1);
        buffer_replace_range(app, buffer, Ii64(pos, pos + 1), s);
        view_set_cursor_and_preferred_x(app, view, seek_pos(pos));
    }
}

CUSTOM_COMMAND_SIG(vim_join_lines)
CUSTOM_DOC("Join the current line with the next (J).")
{
    View_ID view = get_active_view(app, Access_ReadWriteVisible);
    Buffer_ID buffer = view_get_buffer(app, view, Access_ReadWriteVisible);
    i64 pos = view_get_cursor_pos(app, view);
    i64 le = vim_line_end(app, buffer, pos);
    i64 next_line = get_line_number_from_pos(app, buffer, pos) + 1;
    i64 next_start = get_line_side_pos(app, buffer, next_line, Side_Min);
    i64 next_text = get_pos_past_lead_whitespace(app, buffer, next_start);
    if (next_start > le){
        buffer_replace_range(app, buffer, Ii64(le, next_text), string_u8_litexpr(" "));
        view_set_cursor_and_preferred_x(app, view, seek_pos(le));
    }
}

CUSTOM_COMMAND_SIG(vim_paste_after)
CUSTOM_DOC("Paste the register after the cursor / below the line (p).")
{
    View_ID view = get_active_view(app, Access_ReadWriteVisible);
    Buffer_ID buffer = view_get_buffer(app, view, Access_ReadWriteVisible);
    if (vim_register.size == 0){ return; }
    i64 pos = view_get_cursor_pos(app, view);
    if (vim_register_linewise){
        i64 line = get_line_number_from_pos(app, buffer, pos);
        i64 insert_at = get_line_side_pos(app, buffer, line, Side_Max);
        Scratch_Block scratch(app);
        String_Const_u8 body = vim_register;
        while (body.size > 0 && body.str[body.size-1] == '\n'){ body.size -= 1; }
        String_Const_u8 text = push_u8_stringf(scratch, "\n%.*s", string_expand(body));
        buffer_replace_range(app, buffer, Ii64(insert_at, insert_at), text);
        i64 new_line_start = get_line_side_pos(app, buffer, line + 1, Side_Min);
        view_set_cursor_and_preferred_x(app, view, seek_pos(new_line_start));
    }
    else{
        i64 le = vim_line_end(app, buffer, pos);
        i64 at = (pos < le) ? pos + 1 : pos;
        buffer_replace_range(app, buffer, Ii64(at, at), vim_register);
        view_set_cursor_and_preferred_x(app, view, seek_pos(at + vim_register.size - 1));
    }
}

CUSTOM_COMMAND_SIG(vim_paste_before)
CUSTOM_DOC("Paste the register before the cursor / above the line (P).")
{
    View_ID view = get_active_view(app, Access_ReadWriteVisible);
    Buffer_ID buffer = view_get_buffer(app, view, Access_ReadWriteVisible);
    if (vim_register.size == 0){ return; }
    i64 pos = view_get_cursor_pos(app, view);
    if (vim_register_linewise){
        i64 insert_at = vim_line_start(app, buffer, pos);
        buffer_replace_range(app, buffer, Ii64(insert_at, insert_at), vim_register);
        view_set_cursor_and_preferred_x(app, view, seek_pos(insert_at));
    }
    else{
        buffer_replace_range(app, buffer, Ii64(pos, pos), vim_register);
        view_set_cursor_and_preferred_x(app, view, seek_pos(pos + vim_register.size - 1));
    }
}

////////////////////////////////
// Visual mode

function Range_i64
vim_visual_range(Application_Links *app, View_ID view, Buffer_ID buffer, b32 *linewise){
    i64 cursor = view_get_cursor_pos(app, view);
    i64 mark = view_get_mark_pos(app, view);
    Range_i64 range = rectify(Ii64(mark, cursor));
    b32 line_mode = (vim_get_mode(app, view) == VimMode_VisualLine);
    *linewise = line_mode;
    if (line_mode){
        range = vim_linewise_range(app, buffer, range.first, range.one_past_last);
    }
    else{
        i64 size = buffer_get_size(app, buffer);
        if (range.one_past_last < size){ range.one_past_last += 1; } // inclusive
    }
    return(range);
}

CUSTOM_COMMAND_SIG(vim_visual_delete)
CUSTOM_DOC("Delete the visual selection (d/x).")
{
    View_ID view = get_active_view(app, Access_ReadWriteVisible);
    Buffer_ID buffer = view_get_buffer(app, view, Access_ReadWriteVisible);
    b32 linewise = false;
    Range_i64 range = vim_visual_range(app, view, buffer, &linewise);
    vim_apply_operator(app, view, buffer, VimOp_Delete, range, linewise);
    vim_set_mode(app, view, VimMode_Normal);
}

CUSTOM_COMMAND_SIG(vim_visual_change)
CUSTOM_DOC("Change the visual selection (c/s).")
{
    View_ID view = get_active_view(app, Access_ReadWriteVisible);
    Buffer_ID buffer = view_get_buffer(app, view, Access_ReadWriteVisible);
    b32 linewise = false;
    Range_i64 range = vim_visual_range(app, view, buffer, &linewise);
    vim_apply_operator(app, view, buffer, VimOp_Change, range, linewise);
}

CUSTOM_COMMAND_SIG(vim_visual_yank)
CUSTOM_DOC("Yank the visual selection (y).")
{
    View_ID view = get_active_view(app, Access_ReadWriteVisible);
    Buffer_ID buffer = view_get_buffer(app, view, Access_ReadWriteVisible);
    b32 linewise = false;
    Range_i64 range = vim_visual_range(app, view, buffer, &linewise);
    vim_apply_operator(app, view, buffer, VimOp_Yank, range, linewise);
    vim_set_mode(app, view, VimMode_Normal);
}

CUSTOM_COMMAND_SIG(vim_visual_swap)
CUSTOM_DOC("Swap cursor and mark in visual mode (o).")
{
    cursor_mark_swap(app);
}

////////////////////////////////
// Insert-mode helpers

CUSTOM_COMMAND_SIG(vim_insert_delete_word)
CUSTOM_DOC("Delete the word before the cursor (insert-mode C-w / C-h).")
{
    backspace_alpha_numeric_boundary(app);
}

// Minimal auto-pairs (port of mini.pairs): insert the matching close and leave
// the cursor between the pair. Typing over the close / smart deletion are not
// reproduced, but the common "type an opener, get the pair" behavior is.
function void
vim_insert_pair(Application_Links *app, String_Const_u8 pair){
    View_ID view = get_active_view(app, Access_ReadWriteVisible);
    Buffer_ID buffer = view_get_buffer(app, view, Access_ReadWriteVisible);
    i64 pos = view_get_cursor_pos(app, view);
    buffer_replace_range(app, buffer, Ii64(pos, pos), pair);
    view_set_cursor_and_preferred_x(app, view, seek_pos(pos + 1));
}

CUSTOM_COMMAND_SIG(vim_pair_paren)
CUSTOM_DOC("Insert () and place the cursor between them.")
{
    vim_insert_pair(app, string_u8_litexpr("()"));
}

CUSTOM_COMMAND_SIG(vim_pair_bracket)
CUSTOM_DOC("Insert [] and place the cursor between them.")
{
    vim_insert_pair(app, string_u8_litexpr("[]"));
}

CUSTOM_COMMAND_SIG(vim_pair_brace)
CUSTOM_DOC("Insert {} and place the cursor between them.")
{
    vim_insert_pair(app, string_u8_litexpr("{}"));
}

CUSTOM_COMMAND_SIG(vim_pair_dquote)
CUSTOM_DOC("Insert \"\" and place the cursor between them.")
{
    vim_insert_pair(app, string_u8_litexpr("\"\""));
}

CUSTOM_COMMAND_SIG(vim_pair_squote)
CUSTOM_DOC("Insert '' and place the cursor between them.")
{
    vim_insert_pair(app, string_u8_litexpr("''"));
}

////////////////////////////////
// Formatting

function void
vim_run_formatter(Application_Links *app, View_ID view, Buffer_ID buffer){
    Scratch_Block scratch(app);
    String_Const_u8 file_name = push_buffer_file_name(app, scratch, buffer);
    if (file_name.size == 0){ return; }
    String_Const_u8 ext = string_file_extension(file_name);

    String_Const_u8 cmd = {};
    if (string_match(ext, string_u8_litexpr("py"))){
        cmd = push_u8_stringf(scratch, "black -q \"%.*s\"", string_expand(file_name));
    }
    else if (string_match(ext, string_u8_litexpr("c")) ||
             string_match(ext, string_u8_litexpr("h")) ||
             string_match(ext, string_u8_litexpr("cpp")) ||
             string_match(ext, string_u8_litexpr("cc")) ||
             string_match(ext, string_u8_litexpr("hpp")) ||
             string_match(ext, string_u8_litexpr("cxx"))){
        cmd = push_u8_stringf(scratch, "clang-format -i \"%.*s\"", string_expand(file_name));
    }
    if (cmd.size == 0){ return; }

    // Run the external formatter synchronously so the on-disk file is updated
    // before we reopen the buffer (an async exec would race the reopen).
    char *cstr = push_array(scratch, char, (i32)(cmd.size + 1));
    block_copy(cstr, cmd.str, cmd.size);
    cstr[cmd.size] = 0;
    system(cstr);

    // Reload from disk to pick up the formatter's changes.
    buffer_reopen(app, buffer, 0);
}

CUSTOM_COMMAND_SIG(vim_format_buffer)
CUSTOM_DOC("Format the current buffer with clang-format or black (leader cf).")
{
    View_ID view = get_active_view(app, Access_ReadWriteVisible);
    Buffer_ID buffer = view_get_buffer(app, view, Access_ReadWriteVisible);
    save(app);
    vim_run_formatter(app, view, buffer);
}

CUSTOM_COMMAND_SIG(vim_write)
CUSTOM_DOC("Save the current buffer, formatting first when format-on-save is on (:w).")
{
    View_ID view = get_active_view(app, Access_ReadWriteVisible);
    Buffer_ID buffer = view_get_buffer(app, view, Access_ReadWriteVisible);
    save(app);
    if (vim_format_on_save){
        vim_run_formatter(app, view, buffer);
    }
}

CUSTOM_COMMAND_SIG(vim_format_toggle)
CUSTOM_DOC("Toggle format-on-save (FormatToggle).")
{
    vim_format_on_save = !vim_format_on_save;
    if (vim_format_on_save){
        print_message(app, string_u8_litexpr("Format on save: Enabled\n"));
    }
    else{
        print_message(app, string_u8_litexpr("Format on save: Disabled\n"));
    }
}

////////////////////////////////
// Build (compile-mode port)

CUSTOM_COMMAND_SIG(vim_build)
CUSTOM_DOC("Compile the current file (leader rc). g++/gcc for C/C++, make -k otherwise.")
{
    View_ID view = get_active_view(app, Access_Always);
    Buffer_ID buffer = view_get_buffer(app, view, Access_Always);
    Scratch_Block scratch(app);

    save_all_dirty_buffers(app);

    String_Const_u8 file_name = push_buffer_file_name(app, scratch, buffer);
    String_Const_u8 dir = push_hot_directory(app, scratch);
    String_Const_u8 cmd = {};

    if (file_name.size > 0){
        dir = string_remove_last_folder(file_name);
        String_Const_u8 ext = string_file_extension(file_name);
        // file name without directory, without extension -> output binary name
        String_Const_u8 name_only = string_front_of_path(file_name);
        String_Const_u8 stem = string_file_without_extension(name_only);
        if (string_match(ext, string_u8_litexpr("cpp")) ||
            string_match(ext, string_u8_litexpr("cc"))  ||
            string_match(ext, string_u8_litexpr("cxx"))){
            cmd = push_u8_stringf(scratch, "g++ -std=c++23 -o \"%.*s\" \"%.*s\" && ./\"%.*s\"",
                                  string_expand(stem), string_expand(name_only), string_expand(stem));
        }
        else if (string_match(ext, string_u8_litexpr("c"))){
            cmd = push_u8_stringf(scratch, "gcc -o \"%.*s\" \"%.*s\" && ./\"%.*s\"",
                                  string_expand(stem), string_expand(name_only), string_expand(stem));
        }
    }
    if (cmd.size == 0){
        cmd = string_u8_litexpr("make -k");
    }

    View_ID build_view = view;
    Buffer_ID comp = get_buffer_by_name(app, string_u8_litexpr("*compilation*"), Access_Always);
    if (comp != 0){
        View_ID v = get_first_view_with_buffer(app, comp);
        if (v != 0){ build_view = v; }
        else{ build_view = open_build_footer_panel(app); }
    }
    else{
        build_view = open_build_footer_panel(app);
    }

    exec_system_command(app, build_view,
                        buffer_identifier(string_u8_litexpr("*compilation*")),
                        dir, cmd,
                        CLI_OverlapWithConflict|CLI_SendEndSignal);
    block_zero_struct(&prev_location);
    lock_jump_buffer(app, string_u8_litexpr("*compilation*"));
}

////////////////////////////////
// Leader dispatch (Space)

function Key_Code
vim_read_key(Application_Links *app){
    User_Input in = get_next_input(app, EventProperty_AnyKey, EventProperty_Escape);
    if (in.abort || in.event.kind != InputEventKind_KeyStroke){
        return(0);
    }
    return(in.event.key.code);
}

CUSTOM_COMMAND_SIG(vim_leader)
CUSTOM_DOC("Space leader dispatch.")
{
    Key_Code k = vim_read_key(app);
    switch (k){
        case KeyCode_E:
        {
            interactive_open_or_new(app);
        }break;
        case KeyCode_H:
        {
            open_panel_hsplit(app);
        }break;
        case KeyCode_V:
        {
            open_panel_vsplit(app);
        }break;
        case KeyCode_P:
        {
            Key_Code k2 = vim_read_key(app);
            switch (k2){
                case KeyCode_F: interactive_open_or_new(app); break;
                case KeyCode_G: list_all_substring_locations_case_insensitive(app); break;
                case KeyCode_H: command_lister(app); break;
                case KeyCode_B: interactive_switch_buffer(app); break;
                case KeyCode_E: goto_prev_jump(app); break;
            }
        }break;
        case KeyCode_C:
        {
            Key_Code k2 = vim_read_key(app);
            if (k2 == KeyCode_F){ vim_format_buffer(app); }
        }break;
        case KeyCode_R:
        {
            Key_Code k2 = vim_read_key(app);
            if (k2 == KeyCode_C){ vim_build(app); }
        }break;
        case KeyCode_N:
        {
            Key_Code k2 = vim_read_key(app);
            if (k2 == KeyCode_E){ goto_next_jump(app); }
        }break;
    }
}

#endif //FCODER_CONFIG_COMMANDS_CPP
