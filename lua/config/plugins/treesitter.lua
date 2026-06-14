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
require("nvim-treesitter").install(parsers):wait(300000)
