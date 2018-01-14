
f_f [3]
	var t0
	var t1
	var t2
	t2 = p1
	var t3
	t3 = p0
	t1 = t2 > t3
	var t4
	var t5
	t5 = p1
	var t6
	t6 = p0
	t4 = t5 == t6
	t0 = t1 || t4
	if t0 == 0 goto l0
	var t7
	t7 = 0
	return t7
l0:
	var T0
	var T1
	var T2
	var t8
	t8 = p1
	T0 = t8
	var t9
	t9 = p0
	T1 = t9
	var t10
	var t11
	t11 = T0
	t11 = t11 * 4
	t10 = p2 [t11]
	T2 = t10
l1:
	var t12
	var t13
	t13 = T0
	var t14
	t14 = T1
	t12 = t13 != t14
	if t12 == 0 goto l2
l3:
	var t15
	var t16
	var t17
	t17 = T0
	var t18
	t18 = T1
	t16 = t17 < t18
	var t19
	var t20
	var t21
	t21 = T2
	var t22
	var t23
	t23 = T1
	t23 = t23 * 4
	t22 = p2 [t23]
	t20 = t21 < t22
	var t24
	var t25
	t25 = T2
	var t26
	var t27
	t27 = T1
	t27 = t27 * 4
	t26 = p2 [t27]
	t24 = t25 == t26
	t19 = t20 || t24
	t15 = t16 && t19
	if t15 == 0 goto l4
	var t28
	var t29
	t29 = T1
	var t30
	t30 = 1
	t28 = t29 - t30
	T1 = t28
	goto l3
l4:
l5:
	var t31
	var t32
	var t33
	t33 = T0
	var t34
	t34 = T1
	t32 = t33 < t34
	var t35
	var t36
	var t37
	t37 = T2
	var t38
	var t39
	t39 = T0
	t39 = t39 * 4
	t38 = p2 [t39]
	t36 = t37 > t38
	var t40
	var t41
	t41 = T2
	var t42
	var t43
	t43 = T0
	t43 = t43 * 4
	t42 = p2 [t43]
	t40 = t41 == t42
	t35 = t36 || t40
	t31 = t32 && t35
	if t31 == 0 goto l6
	var t44
	var t45
	t45 = T0
	var t46
	t46 = 1
	t44 = t45 + t46
	T0 = t44
	goto l5
l6:
	var t47
	var t48
	t48 = T0
	var t49
	t49 = T1
	t47 = t48 < t49
	if t47 == 0 goto l7
	var T3
	var t50
	var t51
	t51 = T0
	t51 = t51 * 4
	t50 = p2 [t51]
	T3 = t50
	var t52
	var t53
	t53 = T1
	t53 = t53 * 4
	t52 = p2 [t53]
	var t54
	t54 = T0
	t54 = t54 * 4
	p2 [t54] = t52
	var t55
	t55 = T3
	var t56
	t56 = T1
	t56 = t56 * 4
	p2 [t56] = t55
l7:
	goto l1
l2:
	var t57
	var t58
	t58 = T0
	t58 = t58 * 4
	t57 = p2 [t58]
	var t59
	t59 = p1
	t59 = t59 * 4
	p2 [t59] = t57
	var t60
	t60 = T2
	var t61
	t61 = T0
	t61 = t61 * 4
	p2 [t61] = t60
	var T4
	var T5
	var t62
	var t63
	t63 = T0
	var t64
	t64 = 1
	t62 = t63 - t64
	T4 = t62
	var t65
	var t66
	t66 = T0
	var t67
	t67 = 1
	t65 = t66 + t67
	T5 = t65
	var t68
	param T4
	param p1
	param p2
	t68 = call f_f
	T2 = t68
	var t69
	param p0
	param T5
	param p2
	t69 = call f_f
	T2 = t69
	var t70
	t70 = 0
	return t70
end f_f


f_main [0]
	var T6
	var T7
	var 40000 T8
	var t71
	t71 = 0
	T6 = t71
	var t72
	t72 = 10000
	T7 = t72
l8:
	var t73
	var t74
	t74 = T6
	var t75
	t75 = T7
	t73 = t74 < t75
	if t73 == 0 goto l9
	var t76
	var t77
	var t78
	t78 = T6
	var t79
	t79 = 137
	t77 = t78 % t79
	var t80
	var t81
	t81 = T6
	var t82
	t82 = 13
	t80 = t81 % t82
	t76 = t77 + t80
	var t83
	t83 = T6
	t83 = t83 * 4
	T8 [t83] = t76
	var t84
	var t85
	t85 = T6
	var t86
	t86 = 1
	t84 = t85 + t86
	T6 = t84
	goto l8
l9:
	var T9
	var T10
	var t87
	t87 = 0
	T9 = t87
	var t88
	var t89
	t89 = T7
	var t90
	t90 = 1
	t88 = t89 - t90
	T10 = t88
	var t91
	param T10
	param T9
	param T8
	t91 = call f_f
	T9 = t91
	var t92
	t92 = 0
	T6 = t92
	var t93
	t93 = 10
	T9 = t93
l10:
	var t94
	var t95
	t95 = T6
	var t96
	t96 = T7
	t94 = t95 < t96
	if t94 == 0 goto l11
	var t97
	var t98
	t98 = T6
	t98 = t98 * 4
	t97 = T8 [t98]
	T10 = t97
	var t99
	param T10
	t99 = call f_putint
	T10 = t99
	var t100
	param T9
	t100 = call f_putchar
	T10 = t100
	var t101
	var t102
	t102 = T6
	var t103
	t103 = 1
	t101 = t102 + t103
	T6 = t101
	goto l10
l11:
	var t104
	t104 = 0
	return t104
end f_main



