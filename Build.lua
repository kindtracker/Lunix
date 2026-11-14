local lfs = require("lfs")

local AuroraOS = os.getenv("AURORA_OS")

if not AuroraOS then
	error("AURORA_OS environment variable is not set")
end

local AurCompiler = AuroraOS .. "/sdk/aurcc.py"
local Prefix = "arm-none-eabi-"

local Sup = {}
local App = {}

local function Quote(Value)
	return '"' .. Value:gsub('"', '\\"') .. '"'
end

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

local function BuildSup()
	local Objects = {}

	lfs.mkdir("Build")
	lfs.mkdir("Build/Sup")

	for _, Source in ipairs(Sup) do
		local Object = "Build/Sup/" .. Source:match("([^/]+)$"):gsub("%.%w+$", ".o")

		local Command = Prefix
			.. "gcc -c "
			.. Quote(Source)
			.. " -o "
			.. Quote(Object)
			.. " -march=armv6k -mtune=mpcore -marm -mfloat-abi=soft"
			.. " -O2 -ffreestanding -fno-strict-aliasing"

		Run(Command)
		table.insert(Objects, Object)
	end

	local Command = Prefix .. "gcc" .. " -T Source/Sup/Sup.ld" .. " -nostartfiles -nostdlib" .. " -Wl,--gc-sections"

	for _, Object in ipairs(Objects) do
		Command = Command .. " " .. Quote(Object)
	end

	Command = Command .. " -lgcc -o Build/Sup.elf"

	Run(Command)

	Run(Prefix .. "objcopy -O binary " .. Quote("Build/Sup.elf") .. " " .. Quote("Build/Sup.bin"))
end

local function BuildApp()
	table.sort(App)

	if #App == 0 then
		error("No source files found for Application")
	end

	local Command = "python3 " .. Quote(AurCompiler) .. " build"

	for _, Source in ipairs(App) do
		Command = Command .. " " .. Quote(Source)
	end

	Command = Command .. " -o " .. Quote("Lunix.bin")

	Run(Command)
end

FindSources("Source/Sup", Sup)
FindSources("Source/App", App)

BuildSup()
BuildApp()
