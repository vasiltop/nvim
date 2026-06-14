local output_buf = nil

local function output_to_buffer(cmd)
	if output_buf then
		vim.api.nvim_buf_delete(output_buf, { force = false })
	end

	output_buf = vim.api.nvim_create_buf(false, true)
	vim.api.nvim_buf_set_name(output_buf, "Command Output")

	local result = vim.fn.systemlist(cmd)
	vim.api.nvim_buf_set_lines(output_buf, 0, -1, false, result)

	vim.cmd("botright split")
	vim.api.nvim_win_set_buf(0, output_buf)
end

vim.api.nvim_create_user_command("Run", function(opts)
	output_to_buffer(opts.args)
end, {
	nargs = "+",
	complete = "shellcmd",
	desc = "Run shell command into reusable buffer",
})

vim.api.nvim_create_user_command("FormatToggle", function()
	vim.g.format_on_save = not vim.g.format_on_save
	print("Format on save: " .. (vim.g.format_on_save and "Enabled" or "Disabled"))
end, {})
