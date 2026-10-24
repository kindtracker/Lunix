local Target = "Lunix"
local BuildFolder = "Build"
local OutputFolder = "Output"
local IncludeFolder = "Include"

local Python = os.getenv("PYTHON") or "python3"
local Prefix = os.getenv("CROSSCOMPILE") or "arm-none-eabi-"

local CC = Prefix .. "gcc"
local AS = Prefix .. "gcc"
local LD = Prefix .. "gcc"

local Objcopy = Prefix .. "objcopy"
local Firm = OutputFolder .. "/" .. Target .. ".firm"

local Arm9Arch = "-mcpu=arm946e-s -march=armv5te -marm"
local Arm9CFlags = Arm9Arch
	.. " -mthumb-interwork -ffreestanding -fno-builtin -nostdlib -nostartfiles"
	.. " -Wall -Wextra -g -O2 -I"
	.. IncludeFolder
local Arm9AsFlags = Arm9Arch .. " -mthumb-interwork"
local Arm9LdFlags =
	"-T Arm9.ld -nostdlib -nostartfiles -Wl,--fatal-warnings,--no-warn-rwx-segments,--build-id=none -Wl,--gc-sections"

local Arm11Arch = "-mcpu=mpcore -march=armv6k -marm"
local Arm11CFlags = Arm11Arch
	.. " -mno-unaligned-access -mthumb-interwork -ffreestanding -fno-builtin"
	.. " -nostdlib -nostartfiles -Wall -Wextra -g -O2 -I"
	.. IncludeFolder
local Arm11AsFlags = Arm11Arch .. " -mthumb-interwork"
local Arm11LdFlags = "-T Arm11.ld -nostdlib -nostartfiles -Wl,--fatal-warnings,--no-warn-rwx-segments,--build-id=none"

local Arm9LoadAddr = "0x08006800"
local Arm9Entry = "0x08006800"
local Arm11LoadAddr = "0x1FF80000"
local Arm11Entry = "0x1FF80000"

local Arm9CSources = {
	"Source/Arm9/Main.c",
}

local Arm9SSources = {
	"Source/Arm9/Start.s",
}

local Arm11SSources = {
	"Source/Arm11/Start.s",
}

local Core11Sources = {}

local function Quote(Value)
	Value = tostring(Value)
	return '"' .. Value:gsub('"', '\\"') .. '"'
end

local function Run(Command)
	print(Command)
	local Result = os.execute(Command)
	if Result ~= true and Result ~= 0 then
		error("Command returned non-zero status")
	end
end

local function EnsureFolders()
	Run('mkdir -p "' .. BuildFolder .. '" "' .. OutputFolder .. '" "Files"')
end

local function RemoveTree(Path)
	Run('rm -rf "' .. Path .. '"')
end

local function CompileC(Source, Object, Flags, Extra)
	Run(CC .. " " .. Flags .. " " .. (Extra or "") .. " -c " .. Quote(Source) .. " -o " .. Quote(Object))
end

local function Assemble(Source, Object, Flags)
	Run(AS .. " " .. Flags .. " -c " .. Quote(Source) .. " -o " .. Quote(Object))
end

local function Link(Flags, Objects, Output)
	Run(LD .. " " .. Flags .. " " .. table.concat(Objects, " ") .. " -o " .. Quote(Output) .. " -lgcc")
end

local function ToBinary(Input, Output)
	Run(Objcopy .. " -O binary " .. Quote(Input) .. " " .. Quote(Output))
end

local function CompileLunix()
	local Arm9Objects = {}
	local Arm11Objects = {}

	for _, Source in ipairs(Arm9SSources) do
		local Name = Source:match("([^/]+)%.s$")
		local Object = BuildFolder .. "/Arm9" .. Name .. ".o"
		Assemble(Source, Object, Arm9AsFlags)
		table.insert(Arm9Objects, Object)
	end

	for _, Source in ipairs(Arm9CSources) do
		local Name = Source:match("([^/]+)%.c$")
		local Object = BuildFolder .. "/Arm9" .. Name .. ".o"
		CompileC(Source, Object, Arm9CFlags)
		table.insert(Arm9Objects, Object)
	end

	for _, Source in ipairs(Arm11SSources) do
		local Name = Source:match("([^/]+)%.s$")
		local Object = BuildFolder .. "/Arm11" .. Name .. ".o"
		Assemble(Source, Object, Arm11AsFlags)
		table.insert(Arm11Objects, Object)
	end

	Link(Arm9LdFlags, Arm9Objects, BuildFolder .. "/Arm9.elf")
	ToBinary(BuildFolder .. "/Arm9.elf", BuildFolder .. "/Arm9.bin")

	Link(Arm11LdFlags, Arm11Objects, BuildFolder .. "/Arm11.elf")
	ToBinary(BuildFolder .. "/Arm11.elf", BuildFolder .. "/Arm11.bin")

	local FirmTool = "firmtool"

	print("Building " .. Firm)
	Run(
		FirmTool
			.. " build "
			.. Quote(Firm)
			.. " -n "
			.. Arm9Entry
			.. " -e "
			.. Arm11Entry
			.. " -D "
			.. Quote(BuildFolder .. "/Arm9.bin")
			.. " "
			.. Quote(BuildFolder .. "/Arm11.bin")
			.. " -A "
			.. Arm9LoadAddr
			.. " "
			.. Arm11LoadAddr
			.. " -C NDMA XDMA"
	)
	print("Generated file: " .. Firm)
end

local function Clean()
	RemoveTree(BuildFolder)
	RemoveTree(OutputFolder)
end

local Commands = {
	all = function()
		Clean()
		EnsureFolders()
		CompileLunix()
	end,
	clean = Clean,
	build = function()
		EnsureFolders()
		CompileLunix()
	end,
}

local Command = arg[1]
local Action = Commands[Command]
if not Action then
	io.stderr:write("Usage: lua Build.lua [all|clean|rebuild]\n")
	os.exit(2)
end

local Success, ErrorMessage = pcall(Action)
if not Success then
	io.stderr:write("Build error: " .. tostring(ErrorMessage) .. "\n")
	os.exit(1)
end
