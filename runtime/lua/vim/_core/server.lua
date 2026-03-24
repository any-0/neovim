-- For "--listen" and related functionality.

local M = {}
local default_select = vim.ui.select

--- Called by builtin serverlist(). Returns all running servers in stdpath("run").
---
--- - TODO: track TCP servers, somehow.
--- - TODO: support Windows named pipes.
---
--- @param listed string[] Already listed servers
--- @return string[] # List of servers found on the current machine in stdpath("run").
function M.serverlist(listed)
  local root = vim.fs.normalize(vim.fn.stdpath('run') .. '/..')
  local socket_paths = vim.fs.find(function(name, _)
    return name:match('nvim.*')
  end, { path = root, type = 'socket', limit = math.huge })

  local found = {} ---@type string[]
  for _, socket in ipairs(socket_paths) do
    -- Don't list servers twice
    if not vim.list_contains(listed, socket) then
      local ok, chan = pcall(vim.fn.sockconnect, 'pipe', socket, { rpc = true })
      if ok and chan then
        -- Check that the server is responding
        -- TODO: do we need a timeout or error handling here?
        if vim.fn.rpcrequest(chan, 'nvim_get_chan_info', 0).id then
          table.insert(found, socket)
        end
        vim.fn.chanclose(chan)
      end
    end
  end

  return found
end

---@return string[]
local function connectable_servers()
  local found = {} ---@type string[]
  local local_servers = {} ---@type table<string, boolean>
  local listed = vim.fn.serverlist({ peer = true })

  for _, server in ipairs(vim.fn.serverlist()) do
    local_servers[server] = true
  end

  for _, server in ipairs(listed) do
    if server ~= '' and not local_servers[server] then
      table.insert(found, server)
    end
  end

  return found
end

--- Internal helper for |:connect| without an address.
---
--- @param bang boolean
--- @param mods vim.api.keyset.cmd_mods
--- @return boolean # `false` if no other servers were found, `true` if the picker was presented.
local function connect(bang, mods)
  mods = mods or {}
  local servers = connectable_servers()
  if #servers == 0 then
    return false
  end
  if #vim.api.nvim_list_uis() == 0 and vim.ui.select == default_select then
    return 'no_ui'
  end

  -- Defer past the current command execution so the picker can open cleanly.
  vim.schedule(function()
    vim.ui.select(servers, {
      prompt = 'Connect to server:',
      kind = 'nvim_server',
    }, function(server)
      if not server then
        return
      end

      -- Defer past the picker UI callback before issuing the next Ex command.
      vim.schedule(function()
        local ok, err = pcall(vim.api.nvim_cmd, {
          cmd = 'connect',
          bang = bang,
          args = { server },
          mods = mods,
        }, {})
        if not ok and not mods.emsg_silent then
          vim.api.nvim_err_writeln((err:gsub('\n+$', '')))
        end
      end)
    end)
  end)

  return true
end

M.connect = connect

return M
