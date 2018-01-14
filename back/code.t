
f_f [1] [4         ]
	store s0 0
	store s1 1
	store s2 2
	store s3 3
	s0 = a0
	s1 = s0
	s2 = 2
	s1 = s1 < s2
	if s1 == x0 goto l0
	s1 = 1
	a0 = s1
	goto l7
l0:
	s1 = s0
	s2 = 1
	s1 = s1 - s2
	s2 = 2
	s0 = s0 - s2
	s2 = 0
	s3 = s2
	a0 = s1
	call f_f
	s1 = a0
	s1 = s3 + s1
	s2 = s1
	s1 = s2
	a0 = s0
	call f_f
	s0 = a0
	s0 = s1 + s0
	s2 = s0
	s0 = s2
	a0 = s0
	goto l7
l7:
	load 3 s3
	load 2 s2
	load 1 s1
	load 0 s0
	return
end f_f


f_g [1] [161       ]
	loadaddr 0 a1
	a2 = 1
	a3 = 0
	store a6 160
	a6 = 4
	a3 = a3 * a6
	a3 = a3 + a1
	a3 [0] = a2
	a2 = 1
	a3 = 1
	a6 = 4
	a3 = a3 * a6
	a3 = a3 + a1
	a3 [0] = a2
	a2 = 2
l2:
	a3 = a2
	a4 = a0
	a5 = 1
	a4 = a4 + a5
	a3 = a3 < a4
	if a3 == x0 goto l3
	a3 = a2
	a4 = 1
	a3 = a3 - a4
	a6 = 4
	a3 = a3 * a6
	a3 = a3 + a1
	a3 = a3 [0]
	a4 = a2
	a5 = 2
	a4 = a4 - a5
	a6 = 4
	a4 = a4 * a6
	a4 = a4 + a1
	a4 = a4 [0]
	a3 = a3 + a4
	a4 = a2
	a6 = 4
	a4 = a4 * a6
	a4 = a4 + a1
	a4 [0] = a3
	a3 = a2
	a4 = 1
	a3 = a3 + a4
	a2 = a3
	goto l2
l3:
	a6 = 4
	a0 = a0 * a6
	a0 = a0 + a1
	a0 = a0 [0]
	a0 = a0
	goto l8
l8:
	load 160 a6
	return
end f_g


f_h [0] [27        ]
	store s0 9
	store s1 10
	store s2 11
	store s3 12
	store s4 13
	store s5 14
	store s6 15
	store s7 16
	store s8 17
	store s9 18
	store s10 19
	store s11 20
	s0 = 1
	s1 = 2
	s2 = 3
	s3 = 4
	s4 = 5
	s5 = 6
	s6 = 7
	s7 = 8
	s8 = 9
	s9 = 10
	s10 = 11
	s11 = 12
	a0 = 13
	a1 = 14
	a2 = 15
	a3 = 16
	a4 = 17
	a5 = 18
	a6 = 19
	a7 = 20
	t0 = 21
	t1 = 22
	t2 = 23
	t3 = 24
	t4 = 25
	t5 = 26
	t6 = 27
	store t6 0
	t6 = 28
	store t6 1
	t6 = 29
	store t6 2
	t6 = 30
	store t6 3
	t6 = 31
	store t6 4
	t6 = 32
	store t6 5
	t6 = 33
	store t6 6
	t6 = 34
	store t6 7
	t6 = 35
	store t6 8
	t6 = 36
	store a0 21
	a0 = s0
	call f_putint
	s0 = a0
	load 21 a0
	store a0 21
	a0 = s1
	call f_putint
	s0 = a0
	load 21 a0
	store a0 21
	a0 = s2
	call f_putint
	s0 = a0
	load 21 a0
	store a0 21
	a0 = s3
	call f_putint
	s0 = a0
	load 21 a0
	store a0 21
	a0 = s4
	call f_putint
	s0 = a0
	load 21 a0
	store a0 21
	a0 = s5
	call f_putint
	s0 = a0
	load 21 a0
	store a0 21
	a0 = s6
	call f_putint
	s0 = a0
	load 21 a0
	store a0 21
	a0 = s7
	call f_putint
	s0 = a0
	load 21 a0
	store a0 21
	a0 = s8
	call f_putint
	s0 = a0
	load 21 a0
	store a0 21
	a0 = s9
	call f_putint
	s0 = a0
	load 21 a0
	store a0 21
	a0 = s10
	call f_putint
	s0 = a0
	load 21 a0
	store a0 21
	a0 = s11
	call f_putint
	s0 = a0
	load 21 a0
	store a0 21
	load 21 a0
	call f_putint
	s0 = a0
	load 21 a0
	store a0 21
	a0 = a1
	call f_putint
	s0 = a0
	load 21 a0
	store a0 21
	a0 = a2
	call f_putint
	s0 = a0
	load 21 a0
	store a0 21
	a0 = a3
	call f_putint
	s0 = a0
	load 21 a0
	store a0 21
	a0 = a4
	call f_putint
	s0 = a0
	load 21 a0
	store a0 21
	a0 = a5
	call f_putint
	s0 = a0
	load 21 a0
	store a0 21
	a0 = a6
	call f_putint
	s0 = a0
	load 21 a0
	store a0 21
	a0 = a7
	call f_putint
	s0 = a0
	load 21 a0
	store a0 21
	a0 = t0
	call f_putint
	s0 = a0
	load 21 a0
	store a0 21
	a0 = t1
	call f_putint
	s0 = a0
	load 21 a0
	store a0 21
	a0 = t2
	call f_putint
	s0 = a0
	load 21 a0
	store a0 21
	a0 = t3
	call f_putint
	s0 = a0
	load 21 a0
	store a0 21
	a0 = t4
	call f_putint
	s0 = a0
	load 21 a0
	store a0 21
	a0 = t5
	call f_putint
	s0 = a0
	load 21 a0
	store a0 21
	load 0 a0
	call f_putint
	s0 = a0
	load 21 a0
	store a0 21
	load 1 a0
	call f_putint
	s0 = a0
	load 21 a0
	store a0 21
	load 2 a0
	call f_putint
	s0 = a0
	load 21 a0
	store a0 21
	load 3 a0
	call f_putint
	s0 = a0
	load 21 a0
	store a0 21
	load 4 a0
	call f_putint
	s0 = a0
	load 21 a0
	store a0 21
	load 5 a0
	call f_putint
	s0 = a0
	load 21 a0
	store a0 21
	load 6 a0
	call f_putint
	s0 = a0
	load 21 a0
	store a0 21
	load 7 a0
	call f_putint
	s0 = a0
	load 21 a0
	store a0 21
	load 8 a0
	call f_putint
	s0 = a0
	load 21 a0
	store a0 21
	a0 = t6
	call f_putint
	s0 = a0
	load 21 a0
	s0 = 0
l4:
	s1 = s0
	s2 = 10
	s1 = s1 != s2
	if s1 == x0 goto l5
	store a0 21
	a0 = s0
	call f_putint
	s1 = a0
	load 21 a0
	s1 = s0
	s2 = 1
	s1 = s1 + s2
	s0 = s1
	goto l4
l5:
	s0 = 10
	store a0 21
	store a1 22
	store a2 23
	store a3 24
	store a4 25
	store a5 26
	a0 = s0
	call f_g
	s1 = a0
	load 26 a5
	load 25 a4
	load 24 a3
	load 23 a2
	load 22 a1
	load 21 a0
	store a0 21
	a0 = s1
	call f_putint
	s2 = a0
	load 21 a0
	s1 = s2
	store a0 21
	a0 = s0
	call f_f
	s0 = a0
	load 21 a0
	s1 = s0
	store a0 21
	a0 = s1
	call f_putint
	s0 = a0
	load 21 a0
	s0 = 0
	a0 = s0
	goto l9
l9:
	load 20 s11
	load 19 s10
	load 18 s9
	load 17 s8
	load 16 s7
	load 15 s6
	load 14 s5
	load 13 s4
	load 12 s3
	load 11 s2
	load 10 s1
	load 9 s0
	return
end f_h

v0 = 0

f_main [0] [4         ]
	store s0 0
	store s1 1
	store s2 2
	store s3 3
	call f_getint
	s0 = a0
	s1 = s0
	s2 = 0
	s1 = s1 < s2
	s2 = s0
	s3 = 30
	s2 = s2 > s3
	s1 = s1 || s2
	if s1 == x0 goto l6
	s1 = 1
	a0 = s1
	goto l10
l6:
	s1 = 10
	a0 = s0
	call f_f
	s2 = a0
	a0 = s0
	call f_g
	s3 = a0
	a0 = s2
	call f_putint
	s2 = a0
	a0 = s1
	call f_putchar
	s2 = a0
	a0 = s3
	call f_putint
	s2 = a0
	a0 = s1
	call f_putchar
	s1 = a0
	call f_h
	s1 = a0
	s1 = 0
	a0 = s1
	goto l10
l10:
	load 3 s3
	load 2 s2
	load 1 s1
	load 0 s0
	return
end f_main

