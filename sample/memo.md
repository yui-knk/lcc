## test1.c

```shell
$ make
$ objdump -d -M intel build/test1

...
00000000004004ed <main>:
  4004ed: 55                    push   rbp
  4004ee: 48 89 e5              mov    rbp,rsp
  4004f1: b8 2a 00 00 00        mov    eax,0x2a
  4004f6: 5d                    pop    rbp
  4004f7: c3                    ret
  4004f8: 0f 1f 84 00 00 00 00  nop    DWORD PTR [rax+rax*1+0x0]
  4004ff: 00
...
```

In this case `eax` is a backward-compatible fashion as (the lower) 32-bit register of `rax` general purpose 64-bit register.

Ref: https://software.intel.com/en-us/articles/introduction-to-x64-assembly


