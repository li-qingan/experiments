
a:     file format elf32-i386


Disassembly of section .init:

080482d4 <_init>:
 80482d4:	53                   	push   %ebx
 80482d5:	83 ec 08             	sub    $0x8,%esp
 80482d8:	e8 00 00 00 00       	call   80482dd <_init+0x9>
 80482dd:	5b                   	pop    %ebx
 80482de:	81 c3 17 1d 00 00    	add    $0x1d17,%ebx
 80482e4:	8b 83 fc ff ff ff    	mov    -0x4(%ebx),%eax
 80482ea:	85 c0                	test   %eax,%eax
 80482ec:	74 05                	je     80482f3 <_init+0x1f>
 80482ee:	e8 4d 00 00 00       	call   8048340 <__gmon_start__@plt>
 80482f3:	e8 28 01 00 00       	call   8048420 <frame_dummy>
 80482f8:	e8 d3 01 00 00       	call   80484d0 <__do_global_ctors_aux>
 80482fd:	83 c4 08             	add    $0x8,%esp
 8048300:	5b                   	pop    %ebx
 8048301:	c3                   	ret    

Disassembly of section .plt:

08048310 <free@plt-0x10>:
 8048310:	ff 35 f8 9f 04 08    	pushl  0x8049ff8
 8048316:	ff 25 fc 9f 04 08    	jmp    *0x8049ffc
 804831c:	00 00                	add    %al,(%eax)
	...

08048320 <free@plt>:
 8048320:	ff 25 00 a0 04 08    	jmp    *0x804a000
 8048326:	68 00 00 00 00       	push   $0x0
 804832b:	e9 e0 ff ff ff       	jmp    8048310 <_init+0x3c>

08048330 <malloc@plt>:
 8048330:	ff 25 04 a0 04 08    	jmp    *0x804a004
 8048336:	68 08 00 00 00       	push   $0x8
 804833b:	e9 d0 ff ff ff       	jmp    8048310 <_init+0x3c>

08048340 <__gmon_start__@plt>:
 8048340:	ff 25 08 a0 04 08    	jmp    *0x804a008
 8048346:	68 10 00 00 00       	push   $0x10
 804834b:	e9 c0 ff ff ff       	jmp    8048310 <_init+0x3c>

08048350 <__libc_start_main@plt>:
 8048350:	ff 25 0c a0 04 08    	jmp    *0x804a00c
 8048356:	68 18 00 00 00       	push   $0x18
 804835b:	e9 b0 ff ff ff       	jmp    8048310 <_init+0x3c>

Disassembly of section .text:

08048360 <main>:
 8048360:	55                   	push   %ebp
 8048361:	89 e5                	mov    %esp,%ebp
 8048363:	53                   	push   %ebx
 8048364:	31 db                	xor    %ebx,%ebx
 8048366:	83 e4 f0             	and    $0xfffffff0,%esp
 8048369:	83 ec 10             	sub    $0x10,%esp
 804836c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
 8048370:	c7 04 24 00 01 00 00 	movl   $0x100,(%esp)
 8048377:	e8 b4 ff ff ff       	call   8048330 <malloc@plt>
 804837c:	89 04 9d 40 a0 04 08 	mov    %eax,0x804a040(,%ebx,4)
 8048383:	83 c3 01             	add    $0x1,%ebx
 8048386:	89 04 24             	mov    %eax,(%esp)
 8048389:	e8 92 ff ff ff       	call   8048320 <free@plt>
 804838e:	83 fb 0a             	cmp    $0xa,%ebx
 8048391:	75 dd                	jne    8048370 <main+0x10>
 8048393:	31 c0                	xor    %eax,%eax
 8048395:	8b 5d fc             	mov    -0x4(%ebp),%ebx
 8048398:	c9                   	leave  
 8048399:	c3                   	ret    
 804839a:	90                   	nop
 804839b:	90                   	nop

0804839c <_start>:
 804839c:	31 ed                	xor    %ebp,%ebp
 804839e:	5e                   	pop    %esi
 804839f:	89 e1                	mov    %esp,%ecx
 80483a1:	83 e4 f0             	and    $0xfffffff0,%esp
 80483a4:	50                   	push   %eax
 80483a5:	54                   	push   %esp
 80483a6:	52                   	push   %edx
 80483a7:	68 c0 84 04 08       	push   $0x80484c0
 80483ac:	68 50 84 04 08       	push   $0x8048450
 80483b1:	51                   	push   %ecx
 80483b2:	56                   	push   %esi
 80483b3:	68 60 83 04 08       	push   $0x8048360
 80483b8:	e8 93 ff ff ff       	call   8048350 <__libc_start_main@plt>
 80483bd:	f4                   	hlt    
 80483be:	90                   	nop
 80483bf:	90                   	nop

080483c0 <__do_global_dtors_aux>:
 80483c0:	55                   	push   %ebp
 80483c1:	89 e5                	mov    %esp,%ebp
 80483c3:	53                   	push   %ebx
 80483c4:	83 ec 04             	sub    $0x4,%esp
 80483c7:	80 3d 20 a0 04 08 00 	cmpb   $0x0,0x804a020
 80483ce:	75 3f                	jne    804840f <__do_global_dtors_aux+0x4f>
 80483d0:	a1 24 a0 04 08       	mov    0x804a024,%eax
 80483d5:	bb 20 9f 04 08       	mov    $0x8049f20,%ebx
 80483da:	81 eb 1c 9f 04 08    	sub    $0x8049f1c,%ebx
 80483e0:	c1 fb 02             	sar    $0x2,%ebx
 80483e3:	83 eb 01             	sub    $0x1,%ebx
 80483e6:	39 d8                	cmp    %ebx,%eax
 80483e8:	73 1e                	jae    8048408 <__do_global_dtors_aux+0x48>
 80483ea:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
 80483f0:	83 c0 01             	add    $0x1,%eax
 80483f3:	a3 24 a0 04 08       	mov    %eax,0x804a024
 80483f8:	ff 14 85 1c 9f 04 08 	call   *0x8049f1c(,%eax,4)
 80483ff:	a1 24 a0 04 08       	mov    0x804a024,%eax
 8048404:	39 d8                	cmp    %ebx,%eax
 8048406:	72 e8                	jb     80483f0 <__do_global_dtors_aux+0x30>
 8048408:	c6 05 20 a0 04 08 01 	movb   $0x1,0x804a020
 804840f:	83 c4 04             	add    $0x4,%esp
 8048412:	5b                   	pop    %ebx
 8048413:	5d                   	pop    %ebp
 8048414:	c3                   	ret    
 8048415:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
 8048419:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

08048420 <frame_dummy>:
 8048420:	55                   	push   %ebp
 8048421:	89 e5                	mov    %esp,%ebp
 8048423:	83 ec 18             	sub    $0x18,%esp
 8048426:	a1 24 9f 04 08       	mov    0x8049f24,%eax
 804842b:	85 c0                	test   %eax,%eax
 804842d:	74 12                	je     8048441 <frame_dummy+0x21>
 804842f:	b8 00 00 00 00       	mov    $0x0,%eax
 8048434:	85 c0                	test   %eax,%eax
 8048436:	74 09                	je     8048441 <frame_dummy+0x21>
 8048438:	c7 04 24 24 9f 04 08 	movl   $0x8049f24,(%esp)
 804843f:	ff d0                	call   *%eax
 8048441:	c9                   	leave  
 8048442:	c3                   	ret    
 8048443:	90                   	nop
 8048444:	90                   	nop
 8048445:	90                   	nop
 8048446:	90                   	nop
 8048447:	90                   	nop
 8048448:	90                   	nop
 8048449:	90                   	nop
 804844a:	90                   	nop
 804844b:	90                   	nop
 804844c:	90                   	nop
 804844d:	90                   	nop
 804844e:	90                   	nop
 804844f:	90                   	nop

08048450 <__libc_csu_init>:
 8048450:	55                   	push   %ebp
 8048451:	57                   	push   %edi
 8048452:	56                   	push   %esi
 8048453:	53                   	push   %ebx
 8048454:	e8 69 00 00 00       	call   80484c2 <__i686.get_pc_thunk.bx>
 8048459:	81 c3 9b 1b 00 00    	add    $0x1b9b,%ebx
 804845f:	83 ec 1c             	sub    $0x1c,%esp
 8048462:	8b 6c 24 30          	mov    0x30(%esp),%ebp
 8048466:	8d bb 20 ff ff ff    	lea    -0xe0(%ebx),%edi
 804846c:	e8 63 fe ff ff       	call   80482d4 <_init>
 8048471:	8d 83 20 ff ff ff    	lea    -0xe0(%ebx),%eax
 8048477:	29 c7                	sub    %eax,%edi
 8048479:	c1 ff 02             	sar    $0x2,%edi
 804847c:	85 ff                	test   %edi,%edi
 804847e:	74 29                	je     80484a9 <__libc_csu_init+0x59>
 8048480:	31 f6                	xor    %esi,%esi
 8048482:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
 8048488:	8b 44 24 38          	mov    0x38(%esp),%eax
 804848c:	89 2c 24             	mov    %ebp,(%esp)
 804848f:	89 44 24 08          	mov    %eax,0x8(%esp)
 8048493:	8b 44 24 34          	mov    0x34(%esp),%eax
 8048497:	89 44 24 04          	mov    %eax,0x4(%esp)
 804849b:	ff 94 b3 20 ff ff ff 	call   *-0xe0(%ebx,%esi,4)
 80484a2:	83 c6 01             	add    $0x1,%esi
 80484a5:	39 fe                	cmp    %edi,%esi
 80484a7:	75 df                	jne    8048488 <__libc_csu_init+0x38>
 80484a9:	83 c4 1c             	add    $0x1c,%esp
 80484ac:	5b                   	pop    %ebx
 80484ad:	5e                   	pop    %esi
 80484ae:	5f                   	pop    %edi
 80484af:	5d                   	pop    %ebp
 80484b0:	c3                   	ret    
 80484b1:	eb 0d                	jmp    80484c0 <__libc_csu_fini>
 80484b3:	90                   	nop
 80484b4:	90                   	nop
 80484b5:	90                   	nop
 80484b6:	90                   	nop
 80484b7:	90                   	nop
 80484b8:	90                   	nop
 80484b9:	90                   	nop
 80484ba:	90                   	nop
 80484bb:	90                   	nop
 80484bc:	90                   	nop
 80484bd:	90                   	nop
 80484be:	90                   	nop
 80484bf:	90                   	nop

080484c0 <__libc_csu_fini>:
 80484c0:	f3 c3                	repz ret 

080484c2 <__i686.get_pc_thunk.bx>:
 80484c2:	8b 1c 24             	mov    (%esp),%ebx
 80484c5:	c3                   	ret    
 80484c6:	90                   	nop
 80484c7:	90                   	nop
 80484c8:	90                   	nop
 80484c9:	90                   	nop
 80484ca:	90                   	nop
 80484cb:	90                   	nop
 80484cc:	90                   	nop
 80484cd:	90                   	nop
 80484ce:	90                   	nop
 80484cf:	90                   	nop

080484d0 <__do_global_ctors_aux>:
 80484d0:	55                   	push   %ebp
 80484d1:	89 e5                	mov    %esp,%ebp
 80484d3:	53                   	push   %ebx
 80484d4:	83 ec 04             	sub    $0x4,%esp
 80484d7:	a1 14 9f 04 08       	mov    0x8049f14,%eax
 80484dc:	83 f8 ff             	cmp    $0xffffffff,%eax
 80484df:	74 13                	je     80484f4 <__do_global_ctors_aux+0x24>
 80484e1:	bb 14 9f 04 08       	mov    $0x8049f14,%ebx
 80484e6:	66 90                	xchg   %ax,%ax
 80484e8:	83 eb 04             	sub    $0x4,%ebx
 80484eb:	ff d0                	call   *%eax
 80484ed:	8b 03                	mov    (%ebx),%eax
 80484ef:	83 f8 ff             	cmp    $0xffffffff,%eax
 80484f2:	75 f4                	jne    80484e8 <__do_global_ctors_aux+0x18>
 80484f4:	83 c4 04             	add    $0x4,%esp
 80484f7:	5b                   	pop    %ebx
 80484f8:	5d                   	pop    %ebp
 80484f9:	c3                   	ret    
 80484fa:	90                   	nop
 80484fb:	90                   	nop

Disassembly of section .fini:

080484fc <_fini>:
 80484fc:	53                   	push   %ebx
 80484fd:	83 ec 08             	sub    $0x8,%esp
 8048500:	e8 00 00 00 00       	call   8048505 <_fini+0x9>
 8048505:	5b                   	pop    %ebx
 8048506:	81 c3 ef 1a 00 00    	add    $0x1aef,%ebx
 804850c:	e8 af fe ff ff       	call   80483c0 <__do_global_dtors_aux>
 8048511:	83 c4 08             	add    $0x8,%esp
 8048514:	5b                   	pop    %ebx
 8048515:	c3                   	ret    
