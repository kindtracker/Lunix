local lfs = require("lfs")

local Prefix = "/opt/devkitpro/devkitARM/bin/arm-none-eabi-"
local Examples = "Examples"
local Build = "Build"

local Arch = "-march=armv6k -mtune=mpcore -marm -mfloat-abi=soft"
local CFlags = Arch
	.. " -O2 -std=gnu11"
	.. " -ffreestanding"
	.. " -fno-strict-aliasing"
	.. " -fdata-sections"
	.. " -Wall -Wextra"

local function Run(Command)
	print(Command)

	local Process = assert(io.popen(Command .. " 2>&1", "r"))

	for Line in Process:lines() do
		print(Line)
	end

	local Success, Type, Code = Process:close()

	if not Success then
		error("Command failed: " .. tostring(Code))
	end
end

local function Quote(Value)
	return '"' .. Value:gsub('"', '\\"') .. '"'
end

local function FindSources(Folder, Output)
	for Entry in lfs.dir(Folder) do
		if Entry ~= "." and Entry ~= ".." then
			local Path = Folder .. "/" .. Entry
			local Attributes = lfs.attributes(Path)

			if Attributes.mode == "directory" then
				FindSources(Path, Output)
			elseif Attributes.mode == "file" and (Path:match("%.c$") or Path:match("%.s$") or Path:match("%.S$")) then
				table.insert(Output, Path)
			end
		end
	end
end

local function BuildFile(Source)
	local Name = Source:match("([^/]+)%.c$")
	local Output = Examples .. "/" .. Name .. ".elf"

	Run(
		Prefix
			.. "gcc "
			.. Quote(Source)
			.. " -o "
			.. Quote(Output)
			.. " "
			.. CFlags
			.. " -nostartfiles -nostdlib"
			.. " -Wl,--gc-sections"
			.. " -lgcc"
	)
end

lfs.mkdir(Build)

for Entry in lfs.dir(Examples) do
	if Entry ~= "." and Entry ~= ".." then
		local Path = Examples .. "/" .. Entry
		local Attributes = lfs.attributes(Path)

		if Attributes.mode == "file" and Entry:match("%.c$") then
			BuildFile(Path)
		end
	end
end
