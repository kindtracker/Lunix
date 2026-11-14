local lfs = require("lfs")

local AuroraOS = os.getenv("AURORA_OS")

if not AuroraOS then
	error("AURORA_OS environment variable is not set")
end

local Compiler = AuroraOS .. "/sdk/aurcc.py"
local Sources = {}

local function Run(Command)
	print(Command)

	local Process = io.popen(Command .. " 2>&1", "r")

	for Line in Process:lines() do
		print(Line)
	end
end

local function FindSources(Folder)
	for Entry in lfs.dir(Folder) do
		if Entry ~= "." and Entry ~= ".." then
			local Path = Folder .. "/" .. Entry
			local Attributes = lfs.attributes(Path)

			if Attributes.mode == "directory" then
				FindSources(Path)
			elseif Attributes.mode == "file" then
				table.insert(Sources, Path)
			end
		end
	end
end

FindSources("Source")
table.sort(Sources)

local function Quote(Value)
	return '"' .. Value:gsub('"', '\\"') .. '"'
end

local BuildCommand = string.format("python3 %s build -o Lunix.bin", AuroraOS .. "/sdk/aurcc.py")

for _, Source in ipairs(Sources) do
	BuildCommand = BuildCommand .. " " .. Quote(Source)
end

Run(BuildCommand)
