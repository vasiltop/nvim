local parsers = {
	"c",
	"cpp",
	"lua",
	"python",
	"typescript",
	"vim",
	"rust",
	"vue",
	"sql",
	"html",
	"css",
	"bash",
	"gdscript",
}

require("nvim-treesitter").setup()

local installed = require("nvim-treesitter.config").get_installed()
local missing = vim.tbl_filter(function(parser)
	return not vim.tbl_contains(installed, parser)
end, parsers)

if #missing > 0 then
	require("nvim-treesitter").install(missing):wait(300000)
end

vim.api.nvim_create_autocmd("FileType", {
	callback = function()
		pcall(vim.treesitter.start)
		vim.bo.indentexpr = "v:lua.require'nvim-treesitter'.indentexpr()"
	end,
})
