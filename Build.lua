local Target = "Lunix"
local Build = "build"
local Data = "data"
local OutputDir = "output"
local Output = OutputDir .. "/" .. Target

local Sources = {
	"Source",
	"Source/Arm9",
	"Source/Arm11",
	"Source/Common",
	"Source/Crypto",
	"Source/FatFs",
	"Source/Nand",
	"Source/Safety",
	"Source/Arm9/Drivers",
	"Source/Arm9/Shell",
}

local Includes = {
	"Include",
	"Include/Arm9",
	"Include/Arm11",
	"Include/Common",
	"Include/Crypto",
	"Include/FatFs",
	"Include/Nand",
	"Include/Safety",
	"Include/Arm9/Drivers",
}

local DevkitArm = os.getenv("DEVKITARM")
local DevkitPro = os.getenv("DEVKITPRO")
local Font = os.getenv("FONT")
local Open = os.getenv("OPEN")

local function Fail(Message)
	io.stderr:write(Message .. "\n")
	os.exit(1)
end

if not DevkitArm or DevkitArm == "" then
	Fail("Please set DEVKITARM in your environment. " .. "export DEVKITARM=<path to>devkitARM")
end

if not DevkitPro or DevkitPro == "" then
	Fail("Please set DEVKITPRO in your environment. " .. "export DEVKITPRO=<path to>devkitPro")
end

local Calico = DevkitPro .. "/calico"

local ToolPath = table.concat({
	DevkitPro .. "/portlibs/nds/bin",
	DevkitPro .. "/portlibs/armv5te/bin",
	DevkitPro .. "/portlibs/armv4t/bin",
	DevkitArm .. "/bin",
}, ":")

local CC = "arm-none-eabi-gcc"
local CXX = "arm-none-eabi-g++"
local Objcopy = "arm-none-eabi-objcopy"
local NM = "arm-none-eabi-nm"

local function Quote(Value)
	return "'" .. tostring(Value):gsub("'", "'\\''") .. "'"
end

local function Run(Command)
	local FullCommand = "PATH=" .. Quote(ToolPath .. ":" .. (os.getenv("PATH") or "")) .. " " .. Command

	local Success, Reason, Code = os.execute(FullCommand)

	if Success ~= true and Success ~= 0 then
		Fail("Command failed: " .. Command .. " (" .. tostring(Code or Reason or Success) .. ")")
	end
end

local function Capture(Command)
	local FullCommand = "PATH=" .. Quote(ToolPath .. ":" .. (os.getenv("PATH") or "")) .. " " .. Command

	local Pipe = io.popen(FullCommand, "r")
	if not Pipe then
		Fail("Could not execute: " .. Command)
	end

	local Result = Pipe:read("*a")
	local Success, Reason, Code = Pipe:close()

	if Success ~= true and Success ~= 0 then
		Fail("Command failed: " .. Command .. " (" .. tostring(Code or Reason or Success) .. ")")
	end

	return Result
end

local function Mkdir(Path)
	Run("mkdir -p " .. Quote(Path))
end

local function Exists(Path)
	local File = io.open(Path, "rb")
	if File then
		File:close()
		return true
	end
	return false
end

local function Mtime(Path)
	local Result = Capture("stat -c %Y " .. Quote(Path) .. " 2>/dev/null")
	return tonumber(Result:match("%d+"))
end

local function IsNewer(Input, Destination)
	if not Exists(Destination) then
		return true
	end

	local InputTime = Mtime(Input)
	local OutputTime = Mtime(Destination)

	return not InputTime or not OutputTime or InputTime > OutputTime
end

local function Basename(Path)
	return Path:match("([^/]+)$")
end

local function Stem(Path)
	return Basename(Path):gsub("%.[^.]+$", "")
end

local function Join(Values, Separator)
	return table.concat(Values, Separator or " ")
end

local function ListFiles(Directory, Pattern)
	local Files = {}
	local Command = "find "
		.. Quote(Directory)
		.. " -maxdepth 1 -type f -name "
		.. Quote(Pattern)
		.. " -print 2>/dev/null | sort"

	for Path in Capture(Command):gmatch("[^\r\n]+") do
		Files[#Files + 1] = Path
	end

	return Files
end

local Arch = "-mthumb -mthumb-interwork"

local CFlags = table.concat({
	"-g",
	"-Wall",
	"-Wextra",
	"-Wpedantic",
	"-Wcast-align",
	"-Wno-main",
	"-O0",
	"-march=armv5te",
	"-mtune=arm946e-s",
	"-fomit-frame-pointer",
	"-ffast-math",
	"-std=gnu11",
	"-fno-builtin-memcpy",
	Arch,
	"-fdata-sections",
	"-ffunction-sections",
}, " ")

local function GetFontFlags()
	if Font == "ORIG" then
		return "-DFONT_ORIGINAL"
	elseif Font == "6X10" then
		return "-DFONT_6X10"
	elseif Font == "ACORN" then
		return "-DFONT_ACORN"
	elseif Font == "GB" then
		return "-DFONT_GB"
	end

	return "-DFONT_6X10"
end

local IncludeFlags = {}

for _, Directory in ipairs(Includes) do
	IncludeFlags[#IncludeFlags + 1] = "-I" .. Quote(Directory)
end

for _, Directory in ipairs({}) do
	IncludeFlags[#IncludeFlags + 1] = "-I" .. Quote(Directory .. "/include")
end

IncludeFlags[#IncludeFlags + 1] = "-I" .. Quote(Build)
IncludeFlags[#IncludeFlags + 1] = "-I" .. Quote(Calico .. "/include")

local function GetBuildName()
	return Target .. " " .. Capture("date +'%Y/%m/%d'"):gsub("%s+$", "")
end

CFlags = CFlags
	.. " "
	.. Join(IncludeFlags)
	.. " -D__NDS__ -DARM9"
	.. " "
	.. GetFontFlags()
	.. " "
	.. Quote('-DBUILD_NAME="' .. GetBuildName() .. '"')

if Open == "1" then
	CFlags = CFlags .. " -DOPEN_INSTALLER"
end

local CXXFlags = CFlags .. " -fno-rtti -fno-exceptions"

local ASFlags = "-g -mcpu=arm946e-s " .. Arch

local LdFlags = "-TArm9.ld -nostartfiles -g " .. Arch

local Libs = ""
local LibDirs = {}

local function GetLibFlags()
	local Flags = {}

	Flags[#Flags + 1] = "-L" .. Quote(Calico .. "/lib")

	for _, Directory in ipairs(LibDirs) do
		Flags[#Flags + 1] = "-L" .. Quote(Directory .. "/lib")
	end

	return Join(Flags)
end

local function GetSourceFiles()
	local CFiles = {}
	local CppFiles = {}
	local SFiles = {}
	local BinFiles = {}

	for _, Directory in ipairs(Sources) do
		for _, Path in ipairs(ListFiles(Directory, "*.c")) do
			CFiles[#CFiles + 1] = Path
		end

		for _, Path in ipairs(ListFiles(Directory, "*.cpp")) do
			CppFiles[#CppFiles + 1] = Path
		end

		for _, Path in ipairs(ListFiles(Directory, "*.s")) do
			SFiles[#SFiles + 1] = Path
		end

		for _, Path in ipairs(ListFiles(Directory, "*.S")) do
			SFiles[#SFiles + 1] = Path
		end
	end

	for _, Path in ipairs(ListFiles(Data, "*.*")) do
		BinFiles[#BinFiles + 1] = Path
	end

	table.sort(CFiles)
	table.sort(CppFiles)
	table.sort(SFiles)
	table.sort(BinFiles)

	return CFiles, CppFiles, SFiles, BinFiles
end

local function ReadDependencies(Path)
	local Dependencies = {}

	if not Exists(Path) then
		return Dependencies
	end

	local File = io.open(Path, "r")
	local Content = File:read("*a")
	File:close()

	Content = Content:gsub("\\\r?\n", " ")
	local DependencyText = Content:match("^[^:]+:%s*(.*)")

	if DependencyText then
		for Dependency in DependencyText:gmatch("%S+") do
			Dependencies[#Dependencies + 1] = Dependency:gsub("\\ ", " ")
		end
	end

	return Dependencies
end

local function NeedsCompile(Source, Object, DependencyFile)
	if IsNewer(Source, Object) then
		return true
	end

	for _, Dependency in ipairs(ReadDependencies(DependencyFile)) do
		if IsNewer(Dependency, Object) then
			return true
		end
	end

	return false
end

local function CompileFile(Source, Kind)
	local Object = Build .. "/" .. Stem(Source) .. ".o"
	local DependencyFile = Build .. "/" .. Stem(Source) .. ".d"

	if not NeedsCompile(Source, Object, DependencyFile) then
		return Object
	end

	print(Basename(Source))

	local Compiler = CC
	local Flags = CFlags
	local ExtraFlags = ""

	if Kind == "cpp" then
		Compiler = CXX
		Flags = CXXFlags
	elseif Kind == "asm" then
		ExtraFlags = " -x assembler-with-cpp"
	end

	Run(
		Compiler
			.. " -MMD -MP"
			.. " -MF "
			.. Quote(DependencyFile)
			.. " -MT "
			.. Quote(Object)
			.. " "
			.. Flags
			.. ExtraFlags
			.. " -c "
			.. Quote(Source)
			.. " -o "
			.. Quote(Object)
	)

	return Object
end

local function CompileBinary(Source)
	local Object = Build .. "/" .. Stem(Source) .. ".o"

	if not IsNewer(Source, Object) then
		return Object
	end

	print(Basename(Source))

	Run(Objcopy .. " -I binary -O elf32-littlearm -B arm" .. " " .. Quote(Source) .. " " .. Quote(Object))

	return Object
end

local function BuildObjects()
	Mkdir(Build)
	Mkdir(OutputDir)

	local CFiles, CppFiles, SFiles, BinFiles = GetSourceFiles()
	local Objects = {}

	for _, Source in ipairs(BinFiles) do
		Objects[#Objects + 1] = CompileBinary(Source)
	end

	for _, Source in ipairs(CppFiles) do
		Objects[#Objects + 1] = CompileFile(Source, "cpp")
	end

	for _, Source in ipairs(CFiles) do
		Objects[#Objects + 1] = CompileFile(Source, "c")
	end

	for _, Source in ipairs(SFiles) do
		Objects[#Objects + 1] = CompileFile(Source, "asm")
	end

	return Objects, #CppFiles > 0
end

local function Link(Objects, HasCpp)
	local Elf = Output .. ".elf"
	local Map = Build .. "/" .. Target .. ".map"
	local List = Build .. "/" .. Target .. ".lst"

	local NeedsLink = not Exists(Elf) or IsNewer("Arm9.ld", Elf)

	for _, Object in ipairs(Objects) do
		if IsNewer(Object, Elf) then
			NeedsLink = true
			break
		end
	end

	if not NeedsLink then
		return Elf
	end

	print("linking " .. Basename(Elf))

	local Linker = HasCpp and CXX or CC

	Run(Linker .. " " .. GetLibFlags() .. " " .. LdFlags .. " " .. Join(Objects) .. " " .. Libs .. " -o " .. Quote(Elf))

	Run(NM .. " -CSn " .. Quote(Elf) .. " > " .. Quote(List))

	return Elf
end

local function BuildBinary()
	local Objects, HasCpp = BuildObjects()
	local Elf = Link(Objects, HasCpp)
	local Binary = Output .. ".bin"

	if IsNewer(Elf, Binary) then
		Run(
			Objcopy
				.. " --set-section-flags .bss=alloc,load,contents"
				.. " -O binary "
				.. Quote(Elf)
				.. " "
				.. Quote(Binary)
		)

		print("built ... " .. Basename(Binary))
	end
end

local function BuildFirm()
	BuildBinary()

	Run(
		"firmtool build "
			.. Quote(Output .. ".firm")
			.. " -n 0x23F00000"
			.. " -e 0"
			.. " -D "
			.. Quote(Output .. ".bin")
			.. " -A 0x23F00000"
			.. " -C NDMA -i"
	)
end

local function Clean()
	print("clean " .. Target .. "...")
	Run("rm -rf " .. Quote(Build) .. " " .. Quote(OutputDir))
end

local function UpdateSubmodules()
	Run("git submodule update --init --recursive")
end

local Commands = {
	all = BuildFirm,
	firm = BuildFirm,
	binary = BuildBinary,
	clean = Clean,
	submodules = UpdateSubmodules,
}

local Command = arg[1] or "all"
local Action = Commands[Command]

if not Action then
	Fail("Unknown target: " .. Command .. "\nUsage: lua build.lua [all|binary|firm|clean|submodules]")
end

Action()
