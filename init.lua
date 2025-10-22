vim.g.mapleader = " "
vim.opt.tabstop = 2
vim.opt.shiftwidth = 2
vim.opt.nu = true -- enable line numbers
vim.opt.signcolumn = "yes"
vim.opt.relativenumber = true
vim.opt.scrolloff = 4
vim.opt.wrap = false
vim.opt.autoindent = true
vim.opt.smartindent = true

-- clipboard
vim.schedule(function()
	vim.opt.clipboard = "unnamedplus"
end)

-- command line
vim.api.nvim_set_keymap("n", ":", "q:i", { noremap = true, silent = true })

-- explorer
vim.api.nvim_set_keymap("n", "<Space>e", ":Ex<CR>", { noremap = true, silent = true })

vim.pack.add({
	{ src = "https://github.com/echasnovski/mini.pick" },
	{ src = "https://github.com/echasnovski/mini.extra" },
	{ src = "https://github.com/echasnovski/mini.pairs" },

	{ src = "https://github.com/mason-org/mason-lspconfig.nvim" },
	{ src = "https://github.com/mason-org/mason.nvim" },
	{ src = "https://github.com/neovim/nvim-lspconfig" },

	{ src = "https://github.com/nvim-lua/plenary.nvim" },
	{ src = "https://github.com/nvim-treesitter/nvim-treesitter" },
	{ src = "https://github.com/stevearc/oil.nvim" },
	{ src = "https://github.com/vague2k/vague.nvim", name = "vague" },
})

require "vague".setup {
	transparent = true
}
vim.cmd "colorscheme vague"

require "mini.pick".setup()
require "mini.extra".setup()
require "mini.pairs".setup()

require "mason".setup()
require "mason-lspconfig".setup()

require "nvim-treesitter".setup()
require "oil".setup()

local treesitter_ok, treesitter_configs = pcall(require, "nvim-treesitter.configs")
if treesitter_ok then
    treesitter_configs.setup {
        ensure_installed = { "c", "cpp", "lua", "python", "typescript",
            "vim", "rust", "vue", "sql", "html", "css", "bash" },
        sync_install = true,
        ignore_install = {},
        auto_install = true,
        highlight = {
            enable = true,
        },
        modules = {},
        rainbow = {
            enable = true,
            extended_mode = true,
            max_file_lines = 10000,
        }
    }
end

-- lsp
vim.lsp.enable({ "lua_ls", "basedpyright" })

vim.lsp.config("basedpyright", {
    settings = {
        basedpyright = {
            analysis = {
                diagnosticSeverityOverrides = {
                    reportUnknownParameterType = false,
                    reportUnknownVariableType = false,
                    reportUnknownLambdaType = false,
                    reportUnknownMemberType = false,
                    reportUnknownArgumentType = false,
                    reportMissingParameterType = false,
                    reportUnusedCallResult = false,
                    reportAny = false,
                }
            }
        }
    }
})

vim.api.nvim_create_autocmd("LspAttach", {
    callback = function(ev)
        local client = vim.lsp.get_client_by_id(ev.data.client_id)
        if client ~= nil and client:supports_method("textDocument/completion") then
            vim.lsp.completion.enable(true, client.id, ev.buf, { autotrigger = false })
        end
    end,
})

-- keybinds
vim.api.nvim_set_keymap('n', '<C-k>', ':wincmd k<CR>', { noremap = true, silent = true })
vim.api.nvim_set_keymap('n', '<C-j>', ':wincmd j<CR>', { noremap = true, silent = true })
vim.api.nvim_set_keymap('n', '<C-h>', ':wincmd h<MiniPick.registryCR>', { noremap = true, silent = true })
vim.api.nvim_set_keymap('n', '<C-l>', ':wincmd l<CR>', { noremap = true, silent = true })
vim.api.nvim_set_keymap('n', '<Space>h', ':split<CR>', { noremap = true, silent = true })
vim.api.nvim_set_keymap('n', '<Space>v', ':vsplit<CR>', { noremap = true, silent = true })

vim.keymap.set("n", "<leader>e", ":Oil<CR>")
vim.keymap.set("n", "<leader>/", ":nohlsearch<CR>")

vim.keymap.set("n", "<leader>pf", ":Pick files tool='rg'<CR>", { desc = "Pick files" })
vim.keymap.set("n", "<leader>ph", ":Pick help<CR>", { desc = "Pick help" })
vim.keymap.set("n", "<leader>pH", ":Pick hl_groups<CR>", { desc = "Pick highlight groups" })
vim.keymap.set("n", "<leader>pb", ":Pick buffers<CR>", { desc = "Pick buffers" })
vim.keymap.set("n", "<leader>pd", ":Pick diagnostic<CR>", { desc = "Pick diagnostic" })
vim.keymap.set("n", "<leader>pb", ":Pick buffers<CR>", { desc = "Pick buffers" })
vim.keymap.set("n", "<leader>pd", ":Pick diagnostic<CR>", { desc = "Pick diagnostic" })
vim.keymap.set("n", "<leader>pe", ":Pick explorer<CR>", { desc = "Pick explorer" })

vim.keymap.set("n", "<C-Space>", vim.lsp.buf.hover)
vim.keymap.set("i", "<C-Space>", "<C-x><C-o>") -- omnifunc autocomplete
vim.keymap.set("n", "<leader>cf", vim.lsp.buf.format, { desc = "Code format" })
vim.keymap.set("n", "gi", vim.lsp.buf.implementation, { desc = "Go to implementation" })
vim.keymap.set("n", "gd", vim.lsp.buf.definition, { desc = "Go to definition" })
vim.keymap.set("n", "gD", vim.lsp.buf.declaration, { desc = "Go to declaration" })
vim.keymap.set("n", "gt", vim.lsp.buf.type_definition, { desc = "Go to type definition" })
vim.keymap.set("n", "<leader>rn", vim.lsp.buf.rename, { desc = "Rename" })
vim.keymap.set("n", "<leader>ca", vim.lsp.buf.code_action, { desc = "Code action" })

vim.keymap.set("n", "<leader>d", vim.diagnostic.open_float, { desc = "View diagnostic" })

vim.keymap.set("i", "<C-H>", "<C-W>") -- delete word with ctrl-backspace
