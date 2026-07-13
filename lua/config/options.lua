vim.g.mapleader = " "

vim.opt.tabstop = 2
vim.opt.shiftwidth = 2
vim.opt.nu = true
vim.opt.signcolumn = "yes"
vim.opt.relativenumber = true
vim.opt.scrolloff = 4
vim.opt.wrap = false
vim.opt.autoindent = true
vim.opt.smartindent = true
vim.opt.swapfile = false
vim.opt.ruler = false
vim.opt.cmdheight = 0

vim.schedule(function()
	vim.opt.clipboard = "unnamedplus"
end)
