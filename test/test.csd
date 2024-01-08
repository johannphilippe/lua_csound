<CsoundSynthesizer>

<CsOptions>
-odac
</CsOptions>

<CsInstruments>
sr = 44100
ksmps = 64
nchnls = 2
0dbfs = 1

ierr lua_dofile "./test.lua"
print ierr
instr 1
    aArr[] init 2
    aArr[0] = phasor(linseg(100, p3, 300))
    aArr[1] = linseg(0, p3/2, 1, p3/2, 0)

    kArr[] init 2
    kArr[0] = phasor:k(440)
    kArr[1] = linseg:k(0.3, p3, 1)
    ko = lua_call("simple_square", kArr)
    printk2 ko
    
    ao = lua_call("squarewave", aArr)
    outs ao,ao
endin

</CsInstruments>

<CsScore>

i 1 0 10
</CsScore>

</CsoundSynthesizer>

