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


## test2.s

```shell
$ make
$ objdump -d -M intel build/test2

...
00000000004004ed <main>:
  4004ed: 48 c7 c0 2a 00 00 00  mov    rax,0x2a
  4004f4: c3                    ret
  4004f5: 66 2e 0f 1f 84 00 00  nop    WORD PTR cs:[rax+rax*1+0x0]
  4004fc: 00 00 00
  4004ff: 90                    nop
...
```

## plus1.c

```shell
$ make
$ objdump -d -M intel build/plus1

...
00000000004004ed <plus>:
  4004ed: 55                    push   rbp
  4004ee: 48 89 e5              mov    rbp,rsp
  4004f1: 89 7d fc              mov    DWORD PTR [rbp-0x4],edi
  4004f4: 89 75 f8              mov    DWORD PTR [rbp-0x8],esi
  4004f7: 8b 45 f8              mov    eax,DWORD PTR [rbp-0x8]
  4004fa: 8b 55 fc              mov    edx,DWORD PTR [rbp-0x4]
  4004fd: 01 d0                 add    eax,edx
  4004ff: 5d                    pop    rbp
  400500: c3                    ret

0000000000400501 <main>:
  400501: 55                    push   rbp
  400502: 48 89 e5              mov    rbp,rsp
  400505: be 04 00 00 00        mov    esi,0x4
  40050a: bf 03 00 00 00        mov    edi,0x3
  40050f: e8 d9 ff ff ff        call   4004ed <plus>
  400514: 5d                    pop    rbp
  400515: c3                    ret
  400516: 66 2e 0f 1f 84 00 00  nop    WORD PTR cs:[rax+rax*1+0x0]
  40051d: 00 00 00
...
```

`%rdi` is used to pass 1st argument, `%rsi` is used to pass 2nd argument.
`%rsp` is a stack pointer and `%rbp` is used as a frame pointer (a base register).

Ref: https://software.intel.com/sites/default/files/article/402129/mpx-linux64-abi.pdf "Figure 3.4: Register Usage"
