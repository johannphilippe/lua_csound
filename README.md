# Lua Csound
A set of plugin opcodes for Csound allowing to use Lua functions in Csound.  
[LuaJIT](https://luajit.org/) is a Just-In-Time Compiler (JIT) for the Lua programming language. Lua is a powerful, dynamic and light-weight programming language. It may be embedded or used as a general-purpose, stand-alone language. 

# Opcodes
There are different opcodes, with different input and output models : 

- ```ierr lua_dofile Sfile``` must be called first. It creates an instance of Lua machine, and loads Lua and Lua functions in Sfile.
- ```xresult lua_call Sfunction, xargs[]``` executes Sfunction passing, dispatching xargs as its arguments, and returns the result. It works at i, k and a-rate. 
- ```xresult lua_obj Sclass, iargs[], xargs[]``` initialises and object of a Lua class, and calls its method aperf or kperf every k-pass.


Those are not tested.
- ```lua_push_array Sname, iArr[]``` pushes iArr in the global table called "global_values" at index Sname
- ```lua_push_string Sname, Sval``` pushes Sval in the global table "global_values" at index Sname
- ```Sresult lua_str Sfunction, Sargs[]``` does the same as lua_call, but with string inputs and outputs. Works at i and k_rate. 
- ```Sresult lua_nstr Sfunction, xargs[]``` the same with number inputs, and string output. Works at i and k-rate. 
- ```iresult lua_strn Sfunction, Sargs[]``` does the opposite as lua_nstr. 


Those tools can provide facilities to prototype opcodes (particularly lua_obj), or simply to experiment playing with combined Csound - Lua code. 


# Examples 
## lua_call example

We have the following Lua code, save in the luacsound.lua file (Lua files use .lua extension). 
```lua
function squarewave(phasor, amp)
        local arr = {}
        for i = 1, #phasor do
                if(phasor[i]  < 0.5) then
                        arr[i] = amp[i]
                else
                        arr[i] = amp[i] * -1
                end
                --print(i, arr[i])
        end
        return arr
end

```

And now we call it inside Csound.
```csound
ires lua_dofile "/home/myname/luacsound.lua"
; if it went well, ires is supposed to be 0

instr 1 
	aArgs[] init 2
	aArgs[0] = phasor(p4) 
	aArgs[1] = linseg(0, p3/2, 1, p3/2, 0) ; envelope
	ares = lua_call "squarewave", aArgs
	outs ares*0.3, ares*0.3
endin

; now in score 
i 1 0 5 300

```

## lua_obj example
Consider the following Lua class.
```lua

-- simple linseg example
myclass = {} -- first create an empty table

function myclass:new(o) -- base constructor for lua classes
	o=o or {}
	setmetatable(o,self)
	self.__index = self
	return o
end

function myclass:init(from, time, to) -- then the init function allows to store and initialize members
	self.samples_passed = 0
	self.from = from
	self.time = time
	self.to = to
	self.ambitus = self.to - self.from
	self.res_array = {}
end

function myclass:kperf(nothing) -- when performance doesn't need arguments, it is still necessary to pass at leas one dummy argument
	local res = self.perform(self)
	self.samples_passed = self.samples_passed + ksmps -- ksmps is declared to Lua machine as a global variable, as well as sr
	return res
end

function myclass:aperf(nothing) -- if opcode is running at k-rate, it will call kperf, else if will call aperf (ksmps times) every k-pass.  
	for i = 1, ksmps do
		self.res_array[i] = self.perform()
		self.samples_passed = self.samples_passed + 1
	end

	return self.res_array
end

function myclass:perform()
	local time_secs = self.samples_passed / sr
	local time_index = time_secs / self.time
	return ( (time_index * self.ambitus) + self.from )
end
```

Now we call it inside Csound.
```csound
ierr = lua_dofile("class.lua")

instr 1
	iArgs[] init 3
	iArgs[0] = 1
	iArgs[1] = p3
	iArgs[2] = 0

	aArgs[] init 1
	aArgs[0] = 0

	aline = lua_obj:a("line", iArgs, aArgs)
	ao = oscili:a(0.3 * aline, p4)
	outs ao,ao 
endin

; and a score line
i 1 0 5 400
```


# Build

You can prepare build for this project using CMake. First make sure to clone the repository with submodules : `git clone https://framagit.org/johannphilippe/lua_csound --recurse-submodules`.
Then : 
```
cd lua_csound
mkdir build && cd build
cmake ..
make
```

# Alternative build

Alternatively, the old make_script.sh can give you information about how to compile the plugin opcode. You will need to download and link with [LuaJIT](https://luajit.org/luajit.html). Once compiled, the opcode can be placed in the plugin directory of Csound.

## MacOS 

On MacOS, you will need to build luajit 2.1 with GC64 enabled, and MACOSX Target specified. 
You can download sources here : [luajit](https://luajit.org/download.html). 
Make sure `make` command is installed with `brew install make`.
Replace the macOS version after MACOSX DEPLOYMENT TARGET and run command below. 

`make XCFLAGS='-DLUAJIT_ENABLE_GC64' MACOSX_DEPLOYMENT_TARGET=11.1`


## Windows

To build for Windows, I recommand to use the make_win.bat script. You will need to build LuaJIT from source first. 
I could build succesfully following these steps : 

- Clone LuaJIT source from [LuaJIIT](https://github.com/LuaJIT/LuaJIT)
- Open a Visual Studio x64 command prompt. 
- Change directory to LuaJIT source folder, and enter `src`
- Execute msvcbuild
- Now move the libraries (*.lib, *.dll, *.exp) to somewhere in your Path. 
- Change directory to lua_csound folder. 
- Execute `make_win.bat` script. 
- Copy libluacsound.lib and libluacsound.exp to your Csound plugin path. 



