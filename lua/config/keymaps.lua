local dap = require("config.dap")

-- command line
vim.api.nvim_set_keymap("n", ":", "q:i", { noremap = true, silent = true })

-- explorer
vim.api.nvim_set_keymap("n", "<Space>e", ":Ex<CR>", { noremap = true, silent = true })

-- window navigation
vim.api.nvim_set_keymap("n", "<C-k>", ":wincmd k<CR>", { noremap = true, silent = true })
vim.api.nvim_set_keymap("n", "<C-j>", ":wincmd j<CR>", { noremap = true, silent = true })
vim.api.nvim_set_keymap("n", "<C-h>", ":wincmd h<CR>", { noremap = true, silent = true })
vim.api.nvim_set_keymap("n", "<C-l>", ":wincmd l<CR>", { noremap = true, silent = true })
vim.api.nvim_set_keymap("n", "<Space>h", ":split<CR>", { noremap = true, silent = true })
vim.api.nvim_set_keymap("n", "<Space>v", ":vsplit<CR>", { noremap = true, silent = true })

vim.keymap.set("n", "<leader>e", ":Oil<CR>")
vim.keymap.set("n", "<leader>/", ":nohlsearch<CR>")

-- mini.pick
vim.keymap.set("n", "<leader>pf", ":Pick files tool='rg'<CR>", { desc = "Pick files" })
vim.keymap.set("n", "<leader>pg", ":Pick grep_live<CR>", { desc = "Pick grep" })
vim.keymap.set("n", "<leader>ph", ":Pick help<CR>", { desc = "Pick help" })
vim.keymap.set("n", "<leader>pH", ":Pick hl_groups<CR>", { desc = "Pick highlight groups" })
vim.keymap.set("n", "<leader>pb", ":Pick buffers<CR>", { desc = "Pick buffers" })
vim.keymap.set("n", "<leader>pd", ":Pick diagnostic<CR>", { desc = "Pick diagnostic" })
vim.keymap.set("n", "<leader>pe", ":Pick explorer<CR>", { desc = "Pick explorer" })

-- lsp
vim.keymap.set("n", "<C-Space>", vim.lsp.buf.hover)
vim.keymap.set("i", "<C-Space>", "<C-x><C-o>")
vim.keymap.set("n", "<leader>cf", vim.lsp.buf.format, { desc = "Code format" })
vim.keymap.set("n", "gi", vim.lsp.buf.implementation, { desc = "Go to implementation" })
vim.keymap.set("n", "gd", vim.lsp.buf.definition, { desc = "Go to definition" })
vim.keymap.set("n", "gD", vim.lsp.buf.declaration, { desc = "Go to declaration" })
vim.keymap.set("n", "gt", vim.lsp.buf.type_definition, { desc = "Go to type definition" })
vim.keymap.set("n", "<leader>rn", vim.lsp.buf.rename, { desc = "Rename" })
vim.keymap.set("n", "<leader>ca", vim.lsp.buf.code_action, { desc = "Code action" })
vim.keymap.set("n", "<leader>d", vim.diagnostic.open_float, { desc = "View diagnostic" })

-- insert mode
vim.keymap.set("i", "<C-H>", "<C-W>")

-- compile-mode
vim.keymap.set("n", "<leader>rc", ":Recompile<CR>")
vim.keymap.set("n", "<leader>ne", ":NextError<CR>")
vim.keymap.set("n", "<leader>pe", ":PrevError<CR>")

-- dap
vim.keymap.set("n", "<leader>b", function() dap.toggle_breakpoint() end, { desc = "Toggle Breakpoint" })
vim.keymap.set("n", "<F5>", function() dap.continue() end, { desc = "Start/Continue Debugging" })
vim.keymap.set("n", "<leader>so", function() dap.step_over() end, { desc = "Step Over" })
vim.keymap.set("n", "<leader>si", function() dap.step_into() end, { desc = "Step Into" })

-- neogit
vim.keymap.set("n", "<leader>ng", ":Neogit<CR>")
