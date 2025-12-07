<CsoundSynthesizer>

<CsOptions>
-odac
</CsOptions>

<CsInstruments>
sr = 48000
ksmps = 64
nchnls = 2
0dbfs = 1

ierr lua_dofile "./obj_test.lua"
print ierr

; kperf 
instr 1 
    iArr[] init 1 
    kArr[] init 1 
    ; Random curve 
    kArr[0] = rspline:k(1, 10, 0.1, 5)
    kres = lua_obj("myclass", iArr, kArr)
    printk2 kres

    ao = vco2(0.2, 100) * kres
    outs ao, ao
endin
; aperf 
instr 2 
    iArr[] init 1 
    aArr[] init 1 
    ; Random curve 
    aArr[0] = rspline:a(200, 600, 0.1, 5)
    ares = lua_obj:a("myclass", iArr, aArr)
    ares *= 0.2
    outs ares, ares
endin

</CsInstruments>

<CsScore>
i 1 0 10
i 2 10 10
</CsScore>

</CsoundSynthesizer>

