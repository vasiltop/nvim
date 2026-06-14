local port = os.getenv("GDScript_Port") or "6005"
local cmd = vim.lsp.rpc.connect("127.0.0.1", tonumber(port))

vim.lsp.config("gdscript", {
	cmd = cmd,
	filetypes = { "gd", "gdscript", "gdscript3" },
	root_markers = { "project.godot", ".git" },
})

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
				},
			},
		},
	},
})

local vue_language_server_path = vim.fn.expand("$MASON/packages")
	.. "/vue-language-server"
	.. "/node_modules/@vue/language-server"
local tsserver_filetypes = { "typescript", "javascript", "javascriptreact", "typescriptreact", "vue" }
local vue_plugin = {
	name = "@vue/typescript-plugin",
	location = vue_language_server_path,
	languages = { "vue" },
	configNamespace = "typescript",
}

vim.lsp.config("vtsls", {
	settings = {
		vtsls = {
			tsserver = {
				globalPlugins = {
					vue_plugin,
				},
			},
		},
	},
	filetypes = tsserver_filetypes,
})

vim.lsp.config("ts_ls", {
	init_options = {
		plugins = {
			vue_plugin,
		},
	},
	filetypes = tsserver_filetypes,
})

vim.lsp.config("clangd", {
	cmd = {
		"clangd",
		"--clang-tidy",
		"--background-index",
		"--header-insertion=never",
	},
})

vim.lsp.enable({ "lua_ls", "basedpyright", "rust_analyzer", "vtsls", "ts_ls", "clangd", "gdscript" })
