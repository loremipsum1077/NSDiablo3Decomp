; DIAGNOSTIC CONTROL ONLY: same reduced callallgcTM IR with volatile ud load.  Only target and
; function attributes, argument/local stores, the unused ud load, the live L
; load and call are retained.  Debug metadata is omitted.  This fixture tests
; load elimination, not original frame layout or whole-function equivalence.
target datalayout = "e-m:e-i8:8:32-i16:16:32-i64:64-i128:128-n32:64-S128"
target triple = "aarch64--linux-gnu"
%struct.lua_State = type opaque

define void @callallgcTM(%struct.lua_State*, i8*) #0 {
  %3 = alloca %struct.lua_State*, align 8
  %4 = alloca i8*, align 8
  store %struct.lua_State* %0, %struct.lua_State** %3, align 8
  store i8* %1, i8** %4, align 8
  %5 = load volatile i8*, i8** %4, align 8
  %6 = load %struct.lua_State*, %struct.lua_State** %3, align 8
  call void @luaC_callGCTM(%struct.lua_State* %6)
  ret void
}

declare void @luaC_callGCTM(%struct.lua_State*)
attributes #0 = { noimplicitfloat noinline nounwind optnone uwtable "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "target-cpu"="cortex-a57" "target-features"="+crc,+crypto,+fp-armv8,+neon" "use-soft-float"="false" }
