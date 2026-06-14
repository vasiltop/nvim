local paths_to_check = { "/", "/../" }
local cwd = vim.fn.getcwd()

for _, path in ipairs(paths_to_check) do
	if vim.uv.fs_stat(cwd .. path .. "project.godot") then
		local godot_project_path = cwd .. path
		local is_server_running = vim.uv.fs_stat(godot_project_path .. "/server.pipe")

		if not is_server_running then
			vim.fn.serverstart(godot_project_path .. "/server.pipe")
		end

		break
	end
end
