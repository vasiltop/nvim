/*
4coder_config_vim.cpp - A compact Vim-style modal editing engine for 4coder.

Provides NORMAL / INSERT / VISUAL modes on top of 4coder's default API:
  - mode is stored per-view (view_vim_mode managed attachment)
  - vim_implicit_map picks keys_vim_normal / _insert / _visual per keystroke
  - motions (h j k l w W b e 0 ^ $ gg G f F t T { } %) work as movements and
    as operator targets
  - operators d c y with counts, plus doubled forms dd cc yy and D C Y
  - a single unnamed register (linewise-aware) drives p / P

Where 4coder already ships an adequate built-in it is reused instead of being
re-implemented (movement primitives, clipboard, search, listers, etc.).
*/

#if !defined(FCODER_CONFIG_VIM_CPP)
#define FCODER_CONFIG_VIM_CPP

CUSTOM_ID(attachment, view_vim_mode);

enum Vim_Mode{
    VimMode_Normal,
    VimMode_Insert,
    VimMode_Visual,
    VimMode_VisualLine,
};

global i32 vim_pending_count = 0;

global Arena vim_reg_arena = {};
global String_Const_u8 vim_register = {};
global b32 vim_register_linewise = false;

////////////////////////////////
// Mode state

function Vim_Mode
vim_get_mode(Application_Links *app, View_ID view){
    Managed_Scope scope = view_get_managed_scope(app, view);
    Vim_Mode *m = scope_attachment(app, scope, view_vim_mode, Vim_Mode);
    if (m != 0){
        return(*m);
    }
    return(VimMode_Normal);
}

function void
vim_set_mode(Application_Links *app, View_ID view, Vim_Mode mode){
    Managed_Scope scope = view_get_managed_scope(app, view);
    Vim_Mode *m = scope_attachment(app, scope, view_vim_mode, Vim_Mode);
    if (m != 0){
        *m = mode;
    }
}

function i32
vim_take_count(void){
    i32 c = vim_pending_count;
    vim_pending_count = 0;
    if (c <= 0){
        c = 1;
    }
    return(c);
}

function void
vim_set_register(String_Const_u8 s, b32 linewise){
    if (vim_reg_arena.base_allocator == 0){
        vim_reg_arena = make_arena_system();
    }
    linalloc_clear(&vim_reg_arena);
    vim_register = push_string_copy(&vim_reg_arena, s);
    vim_register_linewise = linewise;
}

////////////////////////////////
// Implicit map: choose the key map for the current view's mode.

function Implicit_Map_Result
vim_implicit_map(Application_Links *app, String_ID lang, String_ID mode, Input_Event *event){
    Implicit_Map_Result result = {};
    View_ID view = get_this_ctx_view(app, Access_Always);
    Vim_Mode vmode = vim_get_mode(app, view);
    Command_Map_ID map_id = 0;
    switch (vmode){
        case VimMode_Insert:
        {
            map_id = vars_save_string_lit("keys_vim_insert");
        }break;
        case VimMode_Visual:
        case VimMode_VisualLine:
        {
            map_id = vars_save_string_lit("keys_vim_visual");
        }break;
        default:
        {
            map_id = vars_save_string_lit("keys_vim_normal");
        }break;
    }
    Command_Binding binding = map_get_binding_recursive(&framework_mapping, map_id, event);
    result.map = 0;
    result.command = binding.custom;
    return(result);
}

////////////////////////////////
// Motion primitives

struct Vim_Motion_Result{
    i64 pos;
    b32 valid;
    b32 linewise;
    b32 inclusive;
};

function i64
vim_clamp_pos(Application_Links *app, Buffer_ID buffer, i64 pos){
    i64 size = buffer_get_size(app, buffer);
    return(clamp(0, pos, size));
}

function i64
vim_line_start(Application_Links *app, Buffer_ID buffer, i64 pos){
    return(get_line_side_pos_from_pos(app, buffer, pos, Side_Min));
}

function i64
vim_line_end(Application_Links *app, Buffer_ID buffer, i64 pos){
    return(get_line_side_pos_from_pos(app, buffer, pos, Side_Max));
}

// Reads a single character key (a-z, symbols) for f/F/t/T; returns 0 on abort.
function u8
vim_read_char_key(Application_Links *app){
    User_Input in = get_next_input(app, EventPropertyGroup_Any, EventProperty_Escape);
    if (in.abort){
        return(0);
    }
    String_Const_u8 str = to_writable(&in);
    if (str.str != 0 && str.size > 0){
        return(str.str[0]);
    }
    return(0);
}

function i64
vim_find_char_on_line(Application_Links *app, Buffer_ID buffer, i64 pos, u8 ch,
                      Scan_Direction dir, b32 till){
    Range_i64 line = get_line_pos_range(app, buffer, get_line_number_from_pos(app, buffer, pos));
    if (dir == Scan_Forward){
        for (i64 p = pos + 1; p <= line.end; p += 1){
            u8 c = buffer_get_char(app, buffer, p);
            if (c == ch){
                return(till ? p - 1 : p);
            }
        }
    }
    else{
        for (i64 p = pos - 1; p >= line.start; p -= 1){
            u8 c = buffer_get_char(app, buffer, p);
            if (c == ch){
                return(till ? p + 1 : p);
            }
        }
    }
    return(pos);
}

function i64
vim_match_pair(Application_Links *app, Buffer_ID buffer, i64 pos){
    i64 size = buffer_get_size(app, buffer);
    u8 open_set[] = {'(', '[', '{'};
    u8 close_set[] = {')', ']', '}'};
    for (i64 p = pos; p < size; p += 1){
        u8 c = buffer_get_char(app, buffer, p);
        for (i32 i = 0; i < 3; i += 1){
            if (c == open_set[i]){
                i32 depth = 0;
                for (i64 q = p; q < size; q += 1){
                    u8 d = buffer_get_char(app, buffer, q);
                    if (d == open_set[i]){ depth += 1; }
                    else if (d == close_set[i]){ depth -= 1; if (depth == 0){ return(q); } }
                }
                return(pos);
            }
            if (c == close_set[i]){
                i32 depth = 0;
                for (i64 q = p; q >= 0; q -= 1){
                    u8 d = buffer_get_char(app, buffer, q);
                    if (d == close_set[i]){ depth += 1; }
                    else if (d == open_set[i]){ depth -= 1; if (depth == 0){ return(q); } }
                }
                return(pos);
            }
        }
        // only inspect the character under the cursor, then the rest of the line
        u8 cur = buffer_get_char(app, buffer, p);
        if (cur == '\n'){
            break;
        }
    }
    return(pos);
}

// Computes the target of a motion. `code`/`mods` is the motion key; `g_prefix`
// indicates a preceding 'g'.
function Vim_Motion_Result
vim_compute_motion(Application_Links *app, View_ID view, Buffer_ID buffer, i64 start,
                   Key_Code code, Input_Modifier_Set mods, i32 count, b32 g_prefix){
    Vim_Motion_Result r = {};
    r.pos = start;
    r.valid = true;
    b32 shift = has_modifier(&mods, KeyCode_Shift);
    Scratch_Block scratch(app);

    if (g_prefix){
        if (code == KeyCode_G){
            // gg -> first line (or count'th line)
            i64 line = (vim_pending_count > 0) ? count : 1;
            r.pos = get_line_side_pos(app, buffer, line, Side_Min);
            r.linewise = true;
        }
        else{
            r.valid = false;
        }
        return(r);
    }

    switch (code){
        case KeyCode_H:
        case KeyCode_Left:
        {
            i64 ls = vim_line_start(app, buffer, start);
            r.pos = clamp_bot(ls, start - count);
        }break;
        case KeyCode_L:
        case KeyCode_Right:
        {
            i64 le = vim_line_end(app, buffer, start);
            r.pos = clamp_top(start + count, le);
        }break;
        case KeyCode_J:
        case KeyCode_Down:
        {
            i64 line = get_line_number_from_pos(app, buffer, start) + count;
            r.pos = get_line_side_pos(app, buffer, line, Side_Min);
            r.linewise = true;
        }break;
        case KeyCode_K:
        case KeyCode_Up:
        {
            i64 line = get_line_number_from_pos(app, buffer, start) - count;
            if (line < 1){ line = 1; }
            r.pos = get_line_side_pos(app, buffer, line, Side_Min);
            r.linewise = true;
        }break;
        case KeyCode_0:
        {
            r.pos = vim_line_start(app, buffer, start);
        }break;
        case KeyCode_4: // '$' is Shift+4
        {
            i64 pos = start;
            for (i32 i = 0; i < count; i += 1){
                i64 line = get_line_number_from_pos(app, buffer, pos) + (i == 0 ? 0 : 1);
                pos = get_line_side_pos(app, buffer, line, Side_Max);
            }
            r.pos = pos;
            r.inclusive = true;
        }break;
        case KeyCode_6: // '^' is Shift+6
        {
            r.pos = get_pos_past_lead_whitespace(app, buffer, start);
        }break;
        case KeyCode_W:
        {
            i64 pos = start;
            Boundary_Function_List funcs = shift ?
                push_boundary_list(scratch, boundary_non_whitespace) :
                push_boundary_list(scratch, boundary_alpha_numeric_unicode);
            for (i32 i = 0; i < count; i += 1){
                pos = scan(app, funcs, buffer, Scan_Forward, pos);
            }
            r.pos = vim_clamp_pos(app, buffer, pos);
        }break;
        case KeyCode_B:
        {
            i64 pos = start;
            Boundary_Function_List funcs = shift ?
                push_boundary_list(scratch, boundary_non_whitespace) :
                push_boundary_list(scratch, boundary_alpha_numeric_unicode);
            for (i32 i = 0; i < count; i += 1){
                pos = scan(app, funcs, buffer, Scan_Backward, pos);
            }
            r.pos = vim_clamp_pos(app, buffer, pos);
        }break;
        case KeyCode_E:
        {
            i64 pos = start;
            Boundary_Function_List funcs =
                push_boundary_list(scratch, boundary_alpha_numeric_unicode);
            for (i32 i = 0; i < count; i += 1){
                pos = scan(app, funcs, buffer, Scan_Forward, pos + 1);
            }
            r.pos = vim_clamp_pos(app, buffer, pos);
            r.inclusive = true;
        }break;
        case KeyCode_G:
        {
            // G -> last line (or count'th line if a count was given)
            i64 line = (vim_pending_count > 0) ? count : get_line_number_from_pos(app, buffer, buffer_get_size(app, buffer));
            r.pos = get_line_side_pos(app, buffer, line, Side_Min);
            r.linewise = true;
        }break;
        case KeyCode_LeftBracket: // '{'
        {
            r.pos = get_pos_of_blank_line_grouped(app, buffer, Scan_Backward, start);
            r.linewise = true;
        }break;
        case KeyCode_RightBracket: // '}'
        {
            r.pos = get_pos_of_blank_line_grouped(app, buffer, Scan_Forward, start);
            r.linewise = true;
        }break;
        case KeyCode_5: // '%'
        {
            r.pos = vim_match_pair(app, buffer, start);
            r.inclusive = true;
        }break;
        case KeyCode_F:
        {
            u8 ch = vim_read_char_key(app);
            if (ch == 0){ r.valid = false; break; }
            r.pos = vim_find_char_on_line(app, buffer, start, ch, shift ? Scan_Backward : Scan_Forward, false);
            r.inclusive = !shift;
        }break;
        case KeyCode_T:
        {
            u8 ch = vim_read_char_key(app);
            if (ch == 0){ r.valid = false; break; }
            r.pos = vim_find_char_on_line(app, buffer, start, ch, shift ? Scan_Backward : Scan_Forward, true);
            r.inclusive = !shift;
        }break;
        default:
        {
            r.valid = false;
        }break;
    }
    return(r);
}

////////////////////////////////
// Applying operators

enum Vim_Operator{
    VimOp_Delete,
    VimOp_Change,
    VimOp_Yank,
};

function Range_i64
vim_linewise_range(Application_Links *app, Buffer_ID buffer, i64 a, i64 b){
    i64 la = get_line_number_from_pos(app, buffer, a);
    i64 lb = get_line_number_from_pos(app, buffer, b);
    if (la > lb){ i64 t = la; la = lb; lb = t; }
    i64 start = get_line_side_pos(app, buffer, la, Side_Min);
    i64 end = get_line_side_pos(app, buffer, lb, Side_Max);
    i64 size = buffer_get_size(app, buffer);
    if (end < size){ end += 1; } // include trailing newline
    return(Ii64(start, end));
}

function void
vim_apply_operator(Application_Links *app, View_ID view, Buffer_ID buffer,
                   Vim_Operator op, Range_i64 range, b32 linewise){
    range = rectify(range);
    Scratch_Block scratch(app);
    String_Const_u8 text = push_buffer_range(app, scratch, buffer, range);

    if (op == VimOp_Yank){
        vim_set_register(text, linewise);
        clipboard_post(0, text);
        view_set_cursor_and_preferred_x(app, view, seek_pos(range.first));
        return;
    }

    // delete or change
    vim_set_register(text, linewise);
    clipboard_post(0, text);
    buffer_replace_range(app, buffer, range, string_u8_empty);
    view_set_cursor_and_preferred_x(app, view, seek_pos(range.first));

    if (op == VimOp_Change){
        vim_set_mode(app, view, VimMode_Insert);
    }
}

// Reads a motion key after an operator and applies it. Handles doubled operators
// (dd/cc/yy) for linewise current-line operations.
function void
vim_operator_pending(Application_Links *app, Vim_Operator op, Key_Code op_key){
    View_ID view = get_active_view(app, Access_ReadWriteVisible);
    Buffer_ID buffer = view_get_buffer(app, view, Access_ReadWriteVisible);
    i64 start = view_get_cursor_pos(app, view);

    i32 count = vim_take_count();

    User_Input in = get_next_input(app, EventPropertyGroup_Any, EventProperty_Escape);
    if (in.abort || in.event.kind != InputEventKind_KeyStroke){
        return;
    }
    Key_Code code = in.event.key.code;
    Input_Modifier_Set mods = in.event.key.modifiers;

    // extra count digits after the operator (e.g. d3w)
    if (code >= KeyCode_1 && code <= KeyCode_9 && !has_modifier(&mods, KeyCode_Shift)){
        i32 acc = (i32)(code - KeyCode_0);
        for (;;){
            User_Input d = get_next_input(app, EventPropertyGroup_Any, EventProperty_Escape);
            if (d.abort || d.event.kind != InputEventKind_KeyStroke){ return; }
            Key_Code dc = d.event.key.code;
            if (dc >= KeyCode_0 && dc <= KeyCode_9 && !has_modifier(&d.event.key.modifiers, KeyCode_Shift)){
                acc = acc*10 + (i32)(dc - KeyCode_0);
            }
            else{
                code = dc;
                mods = d.event.key.modifiers;
                break;
            }
        }
        count = count * acc;
    }

    b32 g_prefix = false;
    if (code == KeyCode_G && !has_modifier(&mods, KeyCode_Shift)){
        // could be 'gg' target; peek next key
        User_Input g2 = get_next_input(app, EventPropertyGroup_Any, EventProperty_Escape);
        if (g2.abort){ return; }
        if (g2.event.kind == InputEventKind_KeyStroke && g2.event.key.code == KeyCode_G){
            g_prefix = true;
            code = KeyCode_G;
            mods = g2.event.key.modifiers;
        }
        else{
            return;
        }
    }

    // doubled operator -> linewise current line(s)
    if (code == op_key && !g_prefix){
        i64 end_line = get_line_number_from_pos(app, buffer, start) + (count - 1);
        i64 end_pos = get_line_side_pos(app, buffer, end_line, Side_Min);
        Range_i64 range = vim_linewise_range(app, buffer, start, end_pos);
        vim_apply_operator(app, view, buffer, op, range, true);
        return;
    }

    Vim_Motion_Result m = vim_compute_motion(app, view, buffer, start, code, mods, count, g_prefix);
    if (!m.valid){
        return;
    }

    Range_i64 range = {};
    if (m.linewise){
        range = vim_linewise_range(app, buffer, start, m.pos);
    }
    else{
        range = Ii64(start, m.pos);
        range = rectify(range);
        if (m.inclusive){
            i64 size = buffer_get_size(app, buffer);
            if (range.one_past_last < size){ range.one_past_last += 1; }
        }
    }
    vim_apply_operator(app, view, buffer, op, range, m.linewise);
}

#endif //FCODER_CONFIG_VIM_CPP
